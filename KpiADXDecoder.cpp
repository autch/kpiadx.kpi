#include "stdafx.h"
#include "KpiADXDecoder.h"
#include "KpiPartialFile.h"
#include "aax.h"
#include "bitstream.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include <limits>
#include <numbers>

namespace
{
    template<typename T, typename V, typename R = V>
    R clip(V v)
    {
        if (v > std::numeric_limits<T>::max()) return std::numeric_limits<T>::max();
        if (v < std::numeric_limits<T>::min()) return std::numeric_limits<T>::min();
        return static_cast<R>(v);
    }

    template<typename T>
    T get_be(BYTE* p);

    template<>
    int16_t get_be(BYTE* p)
    {
        const int16_t i = *reinterpret_cast<int16_t*>(p);
        return ntohs(i);
    }

    int16_t predictor(const int16_t coeffs[2], int d, int scale, int16_t s[2])
    {
        const int predict = coeffs[0] * s[0] + coeffs[1] * s[1];
        const int s0 = d * scale + (predict >> 12);
        const auto r = clip<int16_t, int, int16_t>(s0);
        s[1] = s[0];  s[0] = r;
        return r;
    }
}


// pFile の先頭が ADX ならヘッダを読んで true を返す
bool KpiADXDecoder::readADXHeader(IKpiFile* pFile, ADXheader& header)
{
    BYTE buf[sizeof(ADXheader)]{};
    uint8_t signature[6];

    pFile->Seek(0, FILE_BEGIN);
    const DWORD got = pFile->Read(buf, sizeof buf);
    if (got < ADX_HEADER_SIZE)
    {
        ::OutputDebugStringW(L"isADX: cannot read whole common\n");
        return false;
    }
    // ヘッダの短い ADX(AAX のセグメントなど)では後半は未使用
    memset(buf + got, 0, sizeof buf - got);
    memcpy(&header, buf, sizeof header);
    header.prepare();

    if (header.common.sig_8000 != ADX_SIGNATURE)
    {
        ::OutputDebugStringW(L"isADX: cannot read leading 0x8000\n");
        return false;
    }

    if (header.common.copyright_offset < 2)
        return false;
    pFile->Seek(static_cast<INT64>(header.common.copyright_offset) - 2, FILE_BEGIN);
    if (pFile->Read(signature, 6) != 6)
    {
        ::OutputDebugStringW(L"isADX: cannot read at signature offset\n");
        return false;
    }

    if (memcmp(signature, ADX_COPYRIGHT_SIGNATURE, 6) != 0)
    {
        ::OutputDebugStringW(L"isADX: Signature is not (c)CRI\n");
        return false;
    }

    const auto& c = header.common;
    if (c.channel_count == 0 || c.sample_rate == 0
        || c.sample_bitdepth == 0 || c.sample_bitdepth > 16
        || c.block_size <= sizeof(uint16_t)
        || (c.block_size - sizeof(uint16_t)) * 8 / c.sample_bitdepth == 0)
    {
        ::OutputDebugStringW(L"isADX: invalid header\n");
        return false;
    }

    return true;
}

DWORD KpiADXDecoder::readAFS(IKpiFile* pFile)
{
    AFSheader afs_header{};

    pFile->Seek(0, FILE_BEGIN);
    if (pFile->Read(&afs_header, sizeof afs_header) < sizeof afs_header)
    {
        ::OutputDebugStringW(L"isAFS: cannot read afs header\n");
        return 0;
    }

    if (memcmp(afs_header.signature, AFS_SIGNATURE, 4) != 0)
    {
        ::OutputDebugStringW(L"isAFS: signature is not AFS\\0\n");
        return 0;
    }

    const UINT64 file_size = pFile->GetSize();
    if (file_size != KPI_FILE_EOF
        && sizeof afs_header + static_cast<UINT64>(sizeof(AFSitem)) * afs_header.num_of_files > file_size)
    {
        ::OutputDebugStringW(L"isAFS: file is small to hold items\n");
        return 0;
    }

    afs_items_.resize(afs_header.num_of_files);

    const auto bytes_to_read = sizeof(AFSitem) * afs_header.num_of_files;
    if (pFile->Read(afs_items_.data(), static_cast<DWORD>(bytes_to_read)) < bytes_to_read)
    {
        ::OutputDebugStringW(L"isAFS: file is small to hold items\n");
        return 0;
    }

    return afs_header.num_of_files;
}


DWORD KpiADXDecoder::Open(const KPI_MEDIAINFO* cpRequest, IKpiFile* pFile)
{
    kpi_InitMediaInfo(&mediainfo_);

    ADXheader header{};
    std::vector<AAXsegment> aax_segments;
    DWORD dwSongs;
    if (readADXHeader(pFile, header))
    {
        dwSongs = 1;
        source_type_ = SourceType::SOURCE_ADX;
    }
    else if (parseAAX(pFile, aax_segments))
    {
        dwSongs = 1;
        source_type_ = SourceType::SOURCE_AAX;
    }
    else if ((dwSongs = readAFS(pFile)) > 0)
    {
        source_type_ = SourceType::SOURCE_AFS;
    }
    else
    {
        return 0;
    }

    pFile->AddRef();
    kpi_file_ = pFile;
    mediainfo_.dwCount = dwSongs;
    return dwSongs;
}

void KpiADXDecoder::close()
{
    clearSong();
    if (kpi_file_ != nullptr)
    {
        kpi_file_->Release();
        kpi_file_ = nullptr;
    }
}

void KpiADXDecoder::clearSong()
{
    for (auto& seg : segments_)
    {
        if (seg.file != nullptr)
            seg.file->Release();
    }
    segments_.clear();
    total_samples_ = 0;
    loop_enabled_ = false;
    loop_start_sample_ = loop_end_sample_ = 0;
    song_is_aax_ = false;
    state_ = PlayState{};
    loop_state_ = PlayState{};
    loop_state_valid_ = false;
    history_exact_ = false;
}

// pFile の ADX を曲の末尾にセグメントとして加える
bool KpiADXDecoder::addSegment(IKpiFile* pFile, bool aax_loop)
{
    Segment seg;
    if (!readADXHeader(pFile, seg.header))
        return false;

    const auto& c = seg.header.common;
    switch (c.encoding_type)
    {
    case ADXEncodingType::ADX_FIXED_COEF:
    case ADXEncodingType::ADX_LINEAR_SCALE:
    case ADXEncodingType::ADX_EXP_SCALE:
        // supported
        break;
    case ADXEncodingType::ADX_AHX_10:
    case ADXEncodingType::ADX_AHX_11:
    default:
        ::OutputDebugStringW(L"select: unsupported encoding type\n");
        return false;
    }

    // 全セグメントで同じでなければならない
    if (segments_.empty())
    {
        channels_ = c.channel_count;
        block_size_ = c.block_size;
    }
    else
    {
        const auto& first = segments_.front().header.common;
        if (c.channel_count != first.channel_count || c.sample_rate != first.sample_rate
            || c.block_size != first.block_size || c.sample_bitdepth != first.sample_bitdepth)
        {
            ::OutputDebugStringW(L"select: segments have different formats\n");
            return false;
        }
    }

    {
        const double a = std::numbers::sqrt2 - std::cos(
            2.0 * std::numbers::pi * (static_cast<double>(c.highpass_frequency) / static_cast<double>(c.sample_rate)));
        const double b = std::numbers::sqrt2 - 1.0;
        const double d = (a - std::sqrt((a + b) * (a - b))) / b; //(a+b)*(a-b) = a*a-b*b, however the simpler formula loses accuracy in floating point

        seg.coeff[0] = static_cast<int16_t>((d * 2.0) * 4096.0);
        seg.coeff[1] = static_cast<int16_t>(-(d * d) * 4096.0);
    }

    seg.samples_per_block = (c.block_size - sizeof(uint16_t)) * 8 / c.sample_bitdepth;
    seg.data_offset = static_cast<UINT64>(c.copyright_offset) + 4;
    seg.start_sample = total_samples_;
    seg.samples = c.total_samples;
    seg.aax_loop = aax_loop;

    pFile->AddRef();
    seg.file = pFile;
    segments_.push_back(seg);
    total_samples_ += seg.samples;
    return true;
}

bool KpiADXDecoder::loadSong(IKpiFile* pSong)
{
    clearSong();

    ADXheader header{};
    std::vector<AAXsegment> aax_segments;
    if (readADXHeader(pSong, header))
    {
        if (!addSegment(pSong, false))
            return false;

        // ループ情報は ADX のヘッダにある
        const auto& seg = segments_.front();
        const auto& h = seg.header;
        uint32_t loop_start = 0, loop_end = 0;
        switch (h.common.version)
        {
        case ADXVersion::ADX_VERSION_3:
            loop_enabled_ = (h.common.copyright_offset + 4) > ADX_V3_HEADER_SIZE
                && (h.v3.loop_enabled_1 != 0 || h.v3.loop_enabled_2 != 0);
            loop_start = h.v3.loop_begin_sample_index;
            loop_end = h.v3.loop_end_sample_index;
            break;
        case ADXVersion::ADX_VERSION_4:
            loop_enabled_ = (h.common.copyright_offset + 4) > ADX_V4_HEADER_SIZE
                && h.v4.loop_enabled != 0;
            loop_start = h.v4.loop_begin_sample_index;
            loop_end = h.v4.loop_end_sample_index;
            break;
        default:
            loop_enabled_ = false;
            break;
        }
        loop_start_sample_ = loop_start;
        loop_end_sample_ = loop_end;
    }
    else if (parseAAX(pSong, aax_segments))
    {
        song_is_aax_ = true;
        for (const auto& s : aax_segments)
        {
            IKpiFile* segment_file = new KpiPartialFile(pSong, s.offset, s.offset + s.size);
            const bool ok = addSegment(segment_file, s.loop);
            segment_file->Release();
            if (!ok)
                return false;
        }

        // lpflg の立っている最初のセグメントの先頭から最後のセグメントの末尾までをループする
        bool found = false;
        for (const auto& seg : segments_)
        {
            if (!seg.aax_loop) continue;
            if (!found)
                loop_start_sample_ = seg.start_sample;
            loop_end_sample_ = seg.start_sample + seg.samples;
            found = true;
        }
        loop_enabled_ = found;
    }
    else
    {
        return false;
    }

    if (loop_enabled_ && (loop_end_sample_ > total_samples_ || loop_start_sample_ >= loop_end_sample_))
    {
        ::OutputDebugStringW(L"select: invalid loop points\n");
        loop_enabled_ = false;
    }
    if (!loop_enabled_)
        loop_start_sample_ = loop_end_sample_ = 0;

    const size_t samples_per_block = std::max_element(segments_.begin(), segments_.end(),
        [](const Segment& a, const Segment& b) { return a.samples_per_block < b.samples_per_block; })->samples_per_block;
    source_buffer_.resize(static_cast<size_t>(block_size_) * channels_);
    state_.decoded.assign(samples_per_block * channels_, 0);
    state_.ctx.assign(channels_, ADXContext{});
    state_.tail.assign(channels_, ADXContext{});
    return true;
}

void KpiADXDecoder::extract_tags(IKpiTagInfo* pTagInfo) const
{
    auto buf = std::make_shared<wchar_t[]>(128);
    auto set_tag = [buf, pTagInfo](auto name, auto format, auto value)
    {
        wsprintfW(buf.get(), format, value);
        pTagInfo->wSetValueW(name, -1, buf.get(), -1);
    };

    const auto& common = segments_.front().header.common;
    const auto bps = (common.sample_bitdepth * common.sample_rate);
    wsprintf(buf.get(), L"%d.%03dkbps", bps / 1000, bps % 1000);
    pTagInfo->wSetValueW(SZ_KMP_NAME_BITRATE_W, -1, buf.get(), -1);

    switch(common.encoding_type)
    {
    case ADXEncodingType::ADX_EXP_SCALE:
        pTagInfo->wSetValueW(L"ADX_EncodingType", -1, L"4 - Exponent Scale", -1);
        break;
    case ADXEncodingType::ADX_LINEAR_SCALE:
        pTagInfo->wSetValueW(L"ADX_EncodingType", -1, L"3 - Linear Scale", -1);
        break;
    case ADXEncodingType::ADX_FIXED_COEF:
        pTagInfo->wSetValueW(L"ADX_EncodingType", -1, L"2 - Fixed Coefficient", -1);
        break;
    default:
        wsprintf(buf.get(), L"%d (unsupported)", common.encoding_type);
        pTagInfo->wSetValueW(L"ADX_EncodingType", -1, buf.get(), -1);
    }
    set_tag(L"ADX_Version", L"%u", common.version);
    set_tag(L"ADX_BlockSize", L"%u", common.block_size);
    set_tag(L"ADX_SampleBitDepth", L"%u", common.sample_bitdepth);
    set_tag(L"ADX_HighPassFrequency", L"%hu", common.highpass_frequency);

    set_tag(L"ADX_TotalSamples", L"%u", static_cast<uint32_t>(total_samples_));

    pTagInfo->wSetValueW(L"ADX_IsLooped", -1, loop_enabled_ ? L"True" : L"False", -1);
    set_tag(L"ADX_LoopStartSample", L"%u", static_cast<uint32_t>(loop_start_sample_));
    set_tag(L"ADX_LoopEndSample", L"%u", static_cast<uint32_t>(loop_end_sample_));

    if (common.flags == 0x08)
    {
        pTagInfo->wSetValueW(L"ADX_Flags", -1, L"Encrypted (may not play correctly)", -1);
    }

    if (song_is_aax_)
    {
        set_tag(L"AAX_Segments", L"%u", static_cast<uint32_t>(segments_.size()));
    }
}

void KpiADXDecoder::fillMediaInfo(DWORD dwNumber)
{
    const auto sample_rate = segments_.front().header.common.sample_rate;

    mediainfo_.dwNumber = dwNumber;
    mediainfo_.dwFormatType = KPI_MEDIAINFO::FORMAT_PCM;
    mediainfo_.dwSampleRate = sample_rate;
    mediainfo_.nBitsPerSample = 16;
    mediainfo_.dwChannels = channels_;
    mediainfo_.qwFadeOut = -1;
    mediainfo_.dwSeekableFlags = KPI_MEDIAINFO::SEEK_FLAGS_SAMPLE;
    mediainfo_.dwUnitSample = 0;

    if (loop_enabled_)
    {
        mediainfo_.qwLength = kpi_SampleTo100ns(loop_end_sample_, sample_rate);
        mediainfo_.qwLoop = kpi_SampleTo100ns(loop_end_sample_ - loop_start_sample_, sample_rate);
    }
    else
    {
        mediainfo_.qwLength = kpi_SampleTo100ns(total_samples_, sample_rate);
        mediainfo_.qwLoop = 0;
    }
}

DWORD KpiADXDecoder::Select(DWORD dwNumber, const KPI_MEDIAINFO** ppMediaInfo, IKpiTagInfo* pTagInfo, DWORD dwTagGetFlags)
{
    if (dwNumber == 0)
        return 0;

    if (dwNumber > mediainfo_.dwCount)
        return 0;

    try
    {
        IKpiFile* song;
        if (source_type_ == SourceType::SOURCE_AFS)
        {
            const auto& item = afs_items_[dwNumber - 1];
            song = new KpiPartialFile(kpi_file_, item.offset, static_cast<UINT64>(item.offset) + item.length);
        }
        else
        {
            song = kpi_file_;
            song->AddRef();
        }
        const bool ok = loadSong(song);
        song->Release();     // セグメントが参照を持っている
        if (!ok)
        {
            clearSong();
            return 0;
        }

        fillMediaInfo(dwNumber);
        positionAt(0);

        if (ppMediaInfo != nullptr)
            *ppMediaInfo = &mediainfo_;
        if (pTagInfo != nullptr && ppMediaInfo != nullptr)
        {
            extract_tags(pTagInfo);
        }
    }
    catch (...)
    {
        clearSong();
        return 0;
    }

    return dwNumber;
}

UINT64 KpiADXDecoder::Seek(UINT64 qwPosSample, DWORD dwFlag)
{
    if (segments_.empty())
        return 0;

    if (loop_enabled_)
    {
        if (qwPosSample >= loop_end_sample_)
        {
            const UINT64 loop_length = loop_end_sample_ - loop_start_sample_;
            const UINT64 pos = loop_start_sample_ + (qwPosSample - loop_start_sample_) % loop_length;
            if (pos == loop_start_sample_ && loop_state_valid_)
                restoreLoopState();
            else
                positionAt(pos);
        }
        else
        {
            positionAt(qwPosSample);
        }
        return qwPosSample;
    }

    // 曲の終わりより先なら終わった位置
    qwPosSample = std::min(qwPosSample, total_samples_);
    positionAt(qwPosSample);
    return qwPosSample;
}

// 曲中の位置に移動する(瞬時シーク)。
// 先頭以外では ADX の履歴が分からないので 0 から始める
void KpiADXDecoder::positionAt(UINT64 qwPosSample)
{
    size_t index = segments_.size() - 1;
    for (size_t i = 0; i < segments_.size(); i++)
    {
        if (qwPosSample < segments_[i].start_sample + segments_[i].samples)
        {
            index = i;
            break;
        }
    }
    const auto& seg = segments_[index];
    enterSegment(index, std::min(qwPosSample - std::min(qwPosSample, seg.start_sample), seg.samples), false);
    history_exact_ = (qwPosSample == 0);
}

// セグメント内の位置に移動する。
// carry_history: 前のセグメントから続けて再生する。AAX のセグメントは前のセグメントの
// 最後の 2 サンプルを履歴として続けてエンコードされているので、それを引き継ぐ
void KpiADXDecoder::enterSegment(size_t index, UINT64 sample_in_segment, bool carry_history)
{
    const auto& seg = segments_[index];
    state_.segment = index;
    state_.position = seg.start_sample + sample_in_segment;
    state_.decoded_read = state_.decoded_count = 0;
    if (carry_history)
    {
        state_.ctx = state_.tail;
    }
    else
    {
        std::fill(state_.ctx.begin(), state_.ctx.end(), ADXContext{});
        std::fill(state_.tail.begin(), state_.tail.end(), ADXContext{});
    }

    state_.next_block = sample_in_segment / seg.samples_per_block;
    seg.file->Seek(static_cast<INT64>(seg.data_offset + state_.next_block * block_size_ * channels_), FILE_BEGIN);

    const auto samples_to_skip = static_cast<UINT32>(sample_in_segment % seg.samples_per_block);
    if (samples_to_skip > 0)
    {
        decodeNextBlock();
        state_.decoded_read = samples_to_skip;
    }
}

// ループ開始位置の状態に戻す
void KpiADXDecoder::restoreLoopState()
{
    const auto& seg = segments_[loop_state_.segment];
    state_ = loop_state_;
    seg.file->Seek(static_cast<INT64>(seg.data_offset + state_.next_block * block_size_ * channels_), FILE_BEGIN);
    history_exact_ = true;
}

// 現在のセグメントの次のブロックを state_.decoded にデコードする
// 読めなかった場合は無音で埋めて false を返す
bool KpiADXDecoder::decodeNextBlock()
{
    const auto& seg = segments_[state_.segment];
    state_.next_block++;
    state_.decoded_read = 0;
    state_.decoded_count = seg.samples_per_block;

    const DWORD bytes_to_read = static_cast<DWORD>(source_buffer_.size());
    if (seg.file->Read(source_buffer_.data(), bytes_to_read) < bytes_to_read)
    {
        std::fill(state_.decoded.begin(), state_.decoded.end(), static_cast<int16_t>(0));
        return false;
    }

    // ADX では各チャンネルのサンプルは 32 samples ごとに
    // LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR ...
    // と並んでいるので，これを WaveAudio の LRLRLRLR ... 形式に直す
    for (int ch = 0; ch < channels_; ch++)
        decodeBlock(seg, source_buffer_.data() + static_cast<size_t>(ch) * block_size_, state_.decoded.data() + ch, state_.ctx[ch]);
    return true;
}

// 1 チャンネル分の 1 ブロックをデコードして pDest にチャンネル数おきに書き込む
// https://github.com/Thealexbarney/VGAudio/tree/master/src/VGAudio/Codecs/CriAdx
void KpiADXDecoder::decodeBlock(const Segment& seg, BYTE* pSource, int16_t* pDest, ADXContext& context) const
{
    const auto& common = seg.header.common;
    int r_scale = get_be<int16_t>(pSource);
    int scale = r_scale & 0x1fff;

    const int16_t* coeff_p = seg.coeff;
    if(common.encoding_type == ADXEncodingType::ADX_FIXED_COEF)
    {
        const int coeff_index = (r_scale >> 13) & 0x03;
        coeff_p = ADX_TYPE2_COEFF[coeff_index];
    }
    if(common.encoding_type == ADXEncodingType::ADX_EXP_SCALE)
        scale = 1 << (12 - scale);

    pSource += 2;
    bitstream b(pSource, common.block_size - sizeof(int16_t));
    const int sign_decr = (1 << common.sample_bitdepth);
    const int sign_mask = sign_decr >> 1;

    for (int i = 0; i < seg.samples_per_block; i++)
    {
        int d = b.get(common.sample_bitdepth);
        if (d & sign_mask) d -= sign_decr;
        *pDest = predictor(coeff_p, d, scale, context.s);
        pDest += channels_;
    }
}

DWORD KpiADXDecoder::Render(BYTE* pBuffer, DWORD dwSizeSample)
{
    if (segments_.empty())
        return 0;
    return render(reinterpret_cast<int16_t*>(pBuffer), dwSizeSample);
}

// pOutput == nullptr なら読み捨てる
DWORD KpiADXDecoder::render(int16_t* pOutput, DWORD dwSizeSample)
{
    DWORD dwSamplesRendered = 0;

    while (dwSamplesRendered < dwSizeSample)
    {
        const UINT64 end = loop_enabled_ ? loop_end_sample_ : total_samples_;
        if (state_.position >= end)
        {
            if (!loop_enabled_)
                break;
            if (!loop_state_valid_)
            {
                // シークでループ開始位置を飛ばした場合は、先頭からデコードし直して求める
                positionAt(0);
                render(nullptr, static_cast<DWORD>(std::min<UINT64>(loop_start_sample_, MAXDWORD)));
                loop_state_ = state_;
                loop_state_valid_ = (state_.position == loop_start_sample_);
                if (!loop_state_valid_)
                    positionAt(loop_start_sample_);
            }
            else
            {
                restoreLoopState();
            }
            continue;
        }

        const auto& seg = segments_[state_.segment];
        const UINT64 segment_end = seg.start_sample + seg.samples;
        if (state_.position >= segment_end)
        {
            // position < end <= total_samples_ なので次のセグメントがある
            enterSegment(state_.segment + 1, 0, true);
            continue;
        }

        if (loop_enabled_ && !loop_state_valid_ && history_exact_ && state_.position == loop_start_sample_)
        {
            loop_state_ = state_;
            loop_state_valid_ = true;
        }

        if (state_.decoded_read >= state_.decoded_count)
        {
            // データが途中で切れている場合、ループしない曲の最後のセグメントならそこで終わる
            // それ以外は曲の長さを保つため無音で埋める
            if (!decodeNextBlock() && !loop_enabled_ && state_.segment + 1 == segments_.size())
                break;
        }

        // ループ開始位置ではいったん止めて状態を保存する
        const UINT64 stop = (loop_enabled_ && state_.position < loop_start_sample_) ? loop_start_sample_ : end;
        const UINT64 samples_to_copy = std::min({
            static_cast<UINT64>(dwSizeSample - dwSamplesRendered),
            static_cast<UINT64>(state_.decoded_count - state_.decoded_read),
            segment_end - state_.position,
            stop - state_.position });

        const int16_t* p_src = state_.decoded.data() + static_cast<size_t>(state_.decoded_read) * channels_;
        if (pOutput != nullptr)
        {
            memcpy(pOutput + static_cast<size_t>(dwSamplesRendered) * channels_, p_src,
                static_cast<size_t>(samples_to_copy) * channels_ * sizeof(int16_t));
        }

        // 次のセグメントに引き継ぐ履歴(最後の 2 サンプル)
        const size_t last = static_cast<size_t>(samples_to_copy) - 1;
        for (int ch = 0; ch < channels_; ch++)
        {
            auto& t = state_.tail[ch].s;
            const int16_t s0 = p_src[last * channels_ + ch];
            t[1] = (last >= 1 || state_.decoded_read >= 1) ? p_src[(static_cast<ptrdiff_t>(last) - 1) * channels_ + ch] : t[0];
            t[0] = s0;
        }

        dwSamplesRendered += static_cast<DWORD>(samples_to_copy);
        state_.decoded_read += static_cast<UINT32>(samples_to_copy);
        state_.position += samples_to_copy;
    }
    return dwSamplesRendered;
}
