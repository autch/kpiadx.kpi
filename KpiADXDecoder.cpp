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


// pFile の qwOffset から ADX が始まっていればヘッダを読んで true を返す
bool KpiADXDecoder::readADXHeader(IKpiFile* pFile, UINT64 qwOffset, ADXheader& header)
{
    uint8_t signature[6];

    header = ADXheader{};
    pFile->Seek(static_cast<INT64>(qwOffset), FILE_BEGIN);
    if (pFile->Read(&header.common, sizeof header.common) < sizeof header.common)
    {
        ::OutputDebugStringW(L"isADX: cannot read whole common\n");
        return false;
    }
    header.common.prepare();

    if (header.common.sig_8000 != ADX_SIGNATURE)
    {
        ::OutputDebugStringW(L"isADX: cannot read leading 0x8000\n");
        return false;
    }

    // "(c)CRI" is at copyright_offset - 2
    if (header.common.copyright_offset < 2)
    {
        ::OutputDebugStringW(L"isADX: copyright offset is too small\n");
        return false;
    }
    pFile->Seek(static_cast<INT64>(qwOffset + header.common.copyright_offset - 2), FILE_BEGIN);
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

    const auto& common = header.common;
    if (common.channel_count == 0 || common.sample_rate == 0
        || common.sample_bitdepth == 0 || common.sample_bitdepth > 16
        || common.block_size <= sizeof(uint16_t)
        || (common.block_size - sizeof(uint16_t)) * 8 / common.sample_bitdepth == 0)
    {
        ::OutputDebugStringW(L"isADX: invalid header\n");
        return false;
    }

    // ループ情報はヘッダの長さ(データの開始位置まで)に収まっているときだけある
    const uint32_t loop_offset = adx_loop_offset(common);
    if (loop_offset != 0 && common.copyright_offset + 4u > loop_offset + sizeof(ADXloop))
    {
        pFile->Seek(static_cast<INT64>(qwOffset + loop_offset), FILE_BEGIN);
        if (pFile->Read(&header.loop, sizeof header.loop) < sizeof header.loop)
        {
            ::OutputDebugStringW(L"isADX: cannot read loop information\n");
            return false;
        }
        header.loop.prepare();
        header.has_loop_info = true;
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

    readAFSNames(pFile);
    return afs_header.num_of_files;
}

// AFS のファイル名表を読んで afs_titles_ に入れる
// 名前表はなくてもよいので、読めなくても失敗にはしない
void KpiADXDecoder::readAFSNames(IKpiFile* pFile)
{
    afs_titles_.clear();
    afs_dates_.clear();
    const size_t count = afs_items_.size();

    // 名前表の位置は項目表の直後にある。そこが 0 のアーカイブでは最初の項目の直前にある
    AFSitem table{};
    const UINT64 qwAfterItems = sizeof(AFSheader) + static_cast<UINT64>(sizeof(AFSitem)) * count;
    pFile->Seek(static_cast<INT64>(qwAfterItems), FILE_BEGIN);
    if (pFile->Read(&table, sizeof table) < sizeof table)
        table = AFSitem{};
    if (table.offset == 0)
    {
        uint32_t first_offset = std::numeric_limits<uint32_t>::max();
        for (const auto& item : afs_items_)
        {
            if (item.offset != 0)
                first_offset = std::min(first_offset, item.offset);
        }
        if (first_offset != std::numeric_limits<uint32_t>::max() && first_offset >= qwAfterItems + sizeof table)
        {
            pFile->Seek(static_cast<INT64>(first_offset - sizeof table), FILE_BEGIN);
            if (pFile->Read(&table, sizeof table) < sizeof table)
                table = AFSitem{};
        }
    }

    if (table.offset == 0 || table.length == 0)
    {
        ::OutputDebugStringW(L"isAFS: no file name table\n");
        return;
    }
    const uint64_t names_size = static_cast<uint64_t>(sizeof(AFSname)) * count;
    if (table.length < names_size)
    {
        ::OutputDebugStringW(L"isAFS: file name table is smaller than the items\n");
        return;
    }
    const UINT64 file_size = pFile->GetSize();
    if ((file_size != KPI_FILE_EOF && table.offset + names_size > file_size) || names_size > MAXDWORD)
    {
        ::OutputDebugStringW(L"isAFS: file name table is beyond the end of file\n");
        return;
    }

    std::vector<AFSname> names(count);
    const DWORD bytes_to_read = static_cast<DWORD>(names_size);
    pFile->Seek(static_cast<INT64>(table.offset), FILE_BEGIN);
    if (pFile->Read(names.data(), bytes_to_read) < bytes_to_read)
    {
        ::OutputDebugStringW(L"isAFS: cannot read the file name table\n");
        return;
    }

    // 名前は NUL 終端とは限らない。日本語の名前は Shift_JIS と見なす
    afs_titles_.reserve(count);
    afs_dates_.reserve(count);
    for (const auto& entry : names)
    {
        const int name_length = static_cast<int>(strnlen(entry.name, sizeof entry.name));
        std::wstring title;
        const int title_length = (name_length > 0) ? MultiByteToWideChar(932, 0, entry.name, name_length, nullptr, 0) : 0;
        if (title_length > 0)
        {
            title.resize(static_cast<size_t>(title_length));
            MultiByteToWideChar(932, 0, entry.name, name_length, title.data(), title_length);
        }
        afs_titles_.push_back(std::move(title));

        // 日時が入っていない(すべて 0 の)アーカイブや、ありえない値は出さない
        std::wstring date;
        if (entry.year != 0 && entry.month >= 1 && entry.month <= 12 && entry.day >= 1 && entry.day <= 31
            && entry.hour < 24 && entry.minute < 60 && entry.second < 60)
        {
            wchar_t buf[32];
            wsprintfW(buf, L"%04u-%02u-%02u %02u:%02u:%02u",
                entry.year, entry.month, entry.day, entry.hour, entry.minute, entry.second);
            date = buf;
        }
        afs_dates_.push_back(std::move(date));
    }
}


DWORD KpiADXDecoder::Open(const KPI_MEDIAINFO* cpRequest, IKpiFile* pFile)
{
    kpi_InitMediaInfo(&mediainfo_);

    ADXheader header{};
    std::vector<AAXsegment> aax_segments;
    DWORD dwSongs;
    if (readADXHeader(pFile, 0, header))
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
    if (song_file_ != nullptr)
    {
        song_file_->Release();
        song_file_ = nullptr;
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

// song_file_ の offset から size バイトの ADX を曲の末尾にセグメントとして加える
// size が KPI_FILE_EOF(大きさ不明)ならファイルの終わりまで
bool KpiADXDecoder::addSegment(uint64_t offset, uint64_t size, bool aax_loop)
{
    Segment seg;
    if (!readADXHeader(song_file_, offset, seg.header))
        return false;

    const auto& common = seg.header.common;
    switch (common.encoding_type)
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
        channels_ = common.channel_count;
        block_size_ = common.block_size;
    }
    else
    {
        const auto& first = segments_.front().header.common;
        if (common.channel_count != first.channel_count || common.sample_rate != first.sample_rate
            || common.block_size != first.block_size || common.sample_bitdepth != first.sample_bitdepth)
        {
            ::OutputDebugStringW(L"select: segments have different formats\n");
            return false;
        }
    }

    {
        const double a = std::numbers::sqrt2 - std::cos(
            2.0 * std::numbers::pi * (static_cast<double>(common.highpass_frequency) / static_cast<double>(common.sample_rate)));
        const double b = std::numbers::sqrt2 - 1.0;
        const double c = (a - std::sqrt((a + b) * (a - b))) / b; //(a+b)*(a-b) = a*a-b*b, however the simpler formula loses accuracy in floating point

        seg.coeff[0] = static_cast<int16_t>((c * 2.0) * 4096.0);
        seg.coeff[1] = static_cast<int16_t>(-(c * c) * 4096.0);
    }

    seg.samples_per_block = (common.block_size - sizeof(uint16_t)) * 8 / common.sample_bitdepth;
    seg.data_offset = static_cast<uint64_t>(common.copyright_offset) + 4;
    if (seg.data_offset > size)
    {
        ::OutputDebugStringW(L"select: ADX header is larger than the segment\n");
        return false;
    }
    seg.offset = offset;
    seg.end_offset = (size > std::numeric_limits<uint64_t>::max() - offset) ? std::numeric_limits<uint64_t>::max() : offset + size;
    seg.start_sample = total_samples_;
    seg.samples = common.total_samples;
    seg.aax_loop = aax_loop;

    segments_.push_back(seg);
    total_samples_ += seg.samples;
    return true;
}

// song_file_ の曲を読み込む
bool KpiADXDecoder::loadSong()
{
    ADXheader header{};
    std::vector<AAXsegment> aax_segments;
    if (readADXHeader(song_file_, 0, header))
    {
        if (!addSegment(0, song_file_->GetSize(), false))
            return false;

        // ループ情報は ADX のヘッダにある
        // v3 ではループ回数かフラグ、v4 ではフラグが 0 でなければループする
        const auto& loop = header.loop;
        switch (header.common.version)
        {
        case ADXVersion::ADX_VERSION_3:
            loop_enabled_ = header.has_loop_info && (loop.loop_count != 0 || loop.loop_flag != 0);
            break;
        case ADXVersion::ADX_VERSION_4:
            loop_enabled_ = header.has_loop_info && loop.loop_flag != 0;
            break;
        default:
            loop_enabled_ = false;
            break;
        }
        loop_start_sample_ = loop.loop_begin_sample_index;
        loop_end_sample_ = loop.loop_end_sample_index;
    }
    else if (parseAAX(song_file_, aax_segments))
    {
        song_is_aax_ = true;
        for (const auto& s : aax_segments)
        {
            if (!addSegment(s.offset, s.size, s.loop))
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
        ::OutputDebugStringW(L"select: song is neither ADX nor AAX\n");
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
        [](const Segment& lhs, const Segment& rhs) { return lhs.samples_per_block < rhs.samples_per_block; })->samples_per_block;
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

    // AFS のファイル名表にある日時
    const DWORD number = mediainfo_.dwNumber;
    if (number >= 1 && number <= afs_dates_.size() && !afs_dates_[number - 1].empty())
    {
        pTagInfo->wSetValueW(L"AFS_FileDate", -1, afs_dates_[number - 1].c_str(), -1);
    }
}

// AFS のファイル名表の名前を曲名にする
void KpiADXDecoder::setTitle(IKpiTagInfo* pTagInfo, DWORD dwNumber, DWORD dwTagGetFlags) const
{
    // 本体が曲名を求めていない
    if ((dwTagGetFlags & KPI_TAGGET_FLAG_TITLE) == 0)
        return;
    // AFS ではない、または名前表がない
    if (dwNumber == 0 || dwNumber > afs_titles_.size())
        return;
    // 空の名前はタグとして設定しない
    const auto& title = afs_titles_[dwNumber - 1];
    if (title.empty())
        return;
    pTagInfo->wSetValueW(SZ_KMP_NAME_TITLE_W, -1, title.c_str(), -1);
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
    if (ppMediaInfo != nullptr)
        *ppMediaInfo = nullptr;

    // 0 は AFS 全体のタグ取得だが、AFS 全体の情報は持っていない
    if (dwNumber == 0)
        return 0;

    if (dwNumber > mediainfo_.dwCount)
        return 0;

    // タグ取得専用(Media エクスプローラでの表示用)。軽くするため曲は読み込まない
    if (ppMediaInfo == nullptr)
    {
        if (pTagInfo != nullptr)
            setTitle(pTagInfo, dwNumber, dwTagGetFlags);
        return dwNumber;
    }

    try
    {
        clearSong();
        if (source_type_ == SourceType::SOURCE_AFS)
        {
            const auto& item = afs_items_[dwNumber - 1];
            song_file_ = new KpiPartialFile(kpi_file_, item.offset, static_cast<UINT64>(item.offset) + item.length);
        }
        else
        {
            song_file_ = kpi_file_;
            song_file_->AddRef();
        }
        if (!loadSong())
        {
            clearSong();
            return 0;
        }

        fillMediaInfo(dwNumber);
        positionAt(0);

        *ppMediaInfo = &mediainfo_;
        if (pTagInfo != nullptr)
        {
            setTitle(pTagInfo, dwNumber, dwTagGetFlags);
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
    // Select() has not succeeded
    if (segments_.empty())
        return 0;

    if (loop_enabled_)
    {
        if (qwPosSample >= loop_end_sample_)
        {
            const uint64_t loop_length = loop_end_sample_ - loop_start_sample_;
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
void KpiADXDecoder::enterSegment(size_t index, uint64_t sample_in_segment, bool carry_history)
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
    song_file_->Seek(static_cast<INT64>(blockOffset(seg, state_.next_block)), FILE_BEGIN);

    const auto samples_to_skip = static_cast<uint32_t>(sample_in_segment % seg.samples_per_block);
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
    song_file_->Seek(static_cast<INT64>(blockOffset(seg, state_.next_block)), FILE_BEGIN);
    history_exact_ = true;
}

// セグメントの block 番目のブロックの song_file_ での位置
uint64_t KpiADXDecoder::blockOffset(const Segment& seg, uint64_t block) const
{
    return seg.offset + seg.data_offset + block * source_buffer_.size();
}

// 現在のセグメントの次のブロックを state_.decoded にデコードする
// 読めなかった場合は無音で埋めて false を返す
bool KpiADXDecoder::decodeNextBlock()
{
    const auto& seg = segments_[state_.segment];
    const uint64_t block_offset = blockOffset(seg, state_.next_block);
    state_.next_block++;
    state_.decoded_read = 0;
    state_.decoded_count = seg.samples_per_block;

    // ブロックがセグメントの終わりをはみ出す場合は読まない
    // (AAX では次のセグメントのデータになっている)
    const DWORD bytes_to_read = static_cast<DWORD>(source_buffer_.size());
    if (block_offset + bytes_to_read > seg.end_offset
        || song_file_->Read(source_buffer_.data(), bytes_to_read) < bytes_to_read)
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
    // Select() has not succeeded
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
        const uint64_t end = loop_enabled_ ? loop_end_sample_ : total_samples_;
        if (state_.position >= end)
        {
            if (!loop_enabled_)
                break;
            if (!loop_state_valid_)
            {
                // シークでループ開始位置を飛ばした場合は、先頭からデコードし直して求める
                positionAt(0);
                render(nullptr, static_cast<DWORD>(std::min<uint64_t>(loop_start_sample_, MAXDWORD)));
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
        const uint64_t segment_end = seg.start_sample + seg.samples;
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
        const uint64_t stop = (loop_enabled_ && state_.position < loop_start_sample_) ? loop_start_sample_ : end;
        const uint64_t samples_to_copy = std::min({
            static_cast<uint64_t>(dwSizeSample - dwSamplesRendered),
            static_cast<uint64_t>(state_.decoded_count - state_.decoded_read),
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
            auto& tail = state_.tail[ch].s;
            const int16_t last_sample = p_src[last * channels_ + ch];
            tail[1] = (last >= 1 || state_.decoded_read >= 1) ? p_src[(static_cast<ptrdiff_t>(last) - 1) * channels_ + ch] : tail[0];
            tail[0] = last_sample;
        }

        dwSamplesRendered += static_cast<DWORD>(samples_to_copy);
        state_.decoded_read += static_cast<uint32_t>(samples_to_copy);
        state_.position += samples_to_copy;
    }
    return dwSamplesRendered;
}
