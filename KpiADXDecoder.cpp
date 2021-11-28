#include "stdafx.h"
#include "KpiADXDecoder.h"
#include "bitstream.h"
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
}


DWORD KpiADXDecoder::isThisADX(IKpiFile* pFile)
{
    uint8_t signature[6];

    pFile->Seek(0, FILE_BEGIN);
    if (pFile->Read(&adx_header_, sizeof adx_header_) < sizeof adx_header_)
    {
        ::OutputDebugStringW(L"isADX: cannot read whole common\n");
        return 0;
    }

    adx_header_.prepare();

    if (adx_header_.common.sig_8000 != ADX_SIGNATURE)
    {
        ::OutputDebugStringW(L"isADX: cannot read leading 0x8000\n");
        return 0;
    }

    pFile->Seek(static_cast<INT64>(adx_header_.common.copyright_offset) - 2, FILE_BEGIN);
    if (pFile->Read(signature, 6) != 6)
    {
        ::OutputDebugStringW(L"isADX: cannot read at signature offset\n");
        return 0;
    }

    if (memcmp(signature, ADX_COPYRIGHT_SIGNATURE, 6) != 0)
    {
        ::OutputDebugStringW(L"isADX: Signature is not (c)CRI\n");
        return 0;
    }

    return 1;
}

DWORD KpiADXDecoder::isThisAFS(IKpiFile* pFile)
{
    pFile->Seek(0, FILE_BEGIN);
    if (pFile->Read(&afs_header_, sizeof afs_header_) < sizeof afs_header_)
    {
        ::OutputDebugStringW(L"isAFS: cannot read afs header\n");
        return 0;
    }

    if (memcmp(afs_header_.signature, AFS_SIGNATURE, 4) != 0)
    {
        ::OutputDebugStringW(L"isAFS: signature is not AFS\\0\n");
        return 0;
    }

    afs_items_.resize(afs_header_.num_of_files);

    const auto bytes_to_read = sizeof(AFSitem) * afs_header_.num_of_files;
    if (pFile->Read(afs_items_.data(), static_cast<DWORD>(bytes_to_read)) < bytes_to_read)
    {
        ::OutputDebugStringW(L"isAFS: file is small to hold items\n");
        return 0;
    }

    return afs_header_.num_of_files;
}


DWORD KpiADXDecoder::Open(const KPI_MEDIAINFO* cpRequest, IKpiFile* pFile)
{
    pFile->AddRef();
    this->kpi_file_org_ = pFile;

    kpi_InitMediaInfo(&mediainfo_);

    DWORD dwSongs;
    if ((dwSongs = isThisADX(kpi_file_org_)) > 0)
    {
        mediainfo_.dwCount = mediainfo_.dwNumber = dwSongs;
        source_type = SourceType::SOURCE_ADX;
        this->kpi_file_ = this->kpi_file_org_;
        this->kpi_file_->AddRef();
    }
    else if ((dwSongs = isThisAFS(kpi_file_org_)) > 0)
    {
        mediainfo_.dwCount = dwSongs;
        source_type = SourceType::SOURCE_AFS;
    }
    else
    {
        pFile->Release();
        this->kpi_file_org_ = nullptr;
        return 0;
    }

    return mediainfo_.dwCount;
}

void KpiADXDecoder::reset()
{
    for (int ch = 0; ch < adx_header_.common.channel_count; ch++)
    {
        memset(adx_ctx_[ch].s, 0, sizeof adx_ctx_[ch].s);
        decoded_block_[ch].s.resize(samples_per_block_);
        for (int i = 0; i < samples_per_block_; i++)
            memset(decoded_block_[ch].s.data(), 0, sizeof(int16_t) * decoded_block_[ch].s.size());
    }
    memset(loop_buffer_.data(), 0, loop_buffer_.size());
    samples_to_skip_ = 0;
    samples_decoded_ = 0;
    kpi_file_->Seek(static_cast<INT64>(adx_header_.common.copyright_offset) + 4, FILE_BEGIN);
}

void KpiADXDecoder::close()
{
    if (kpi_file_ != nullptr)
    {
        kpi_file_->Release();
        kpi_file_ = nullptr;
    }
    if (kpi_file_org_ != nullptr)
    {
        kpi_file_org_->Release();
        kpi_file_org_ = nullptr;
    }
}

IKpiFile* KpiADXDecoder::prepareAFSMember(DWORD dwNumber)
{
    return new KpiPartialFile(kpi_file_org_, afs_items_[dwNumber - 1].offset,
        afs_items_[dwNumber - 1].offset + afs_items_[dwNumber - 1].length);
}

void KpiADXDecoder::extract_tags(IKpiTagInfo* pTagInfo) const
{
    auto buf = std::make_shared<wchar_t[]>(128);
    auto set_tag = [buf, pTagInfo](auto name, auto format, auto value)
    {
        wsprintfW(buf.get(), format, value);
        pTagInfo->wSetValueW(name, -1, buf.get(), -1);
    };

    const auto bps = (adx_header_.common.sample_bitdepth * adx_header_.common.sample_rate);
    wsprintf(buf.get(), L"%d.%03dkbps", bps / 1000, bps % 1000);
    pTagInfo->wSetValueW(SZ_KMP_NAME_BITRATE_W, -1, buf.get(), -1);

    switch(adx_header_.common.encoding_type)
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
        wsprintf(buf.get(), L"%d (unsupported)", adx_header_.common.encoding_type);
        pTagInfo->wSetValueW(L"ADX_EncodingType", -1, buf.get(), -1);
    }
    set_tag(L"ADX_Version", L"%u", adx_header_.common.version);
    set_tag(L"ADX_BlockSize", L"%u", adx_header_.common.block_size);
    set_tag(L"ADX_SampleBitDepth", L"%u", adx_header_.common.sample_bitdepth);
    set_tag(L"ADX_HighPassFrequency", L"%hu", adx_header_.common.highpass_frequency);

    set_tag(L"ADX_TotalSamples", L"%u", adx_header_.common.total_samples);

    pTagInfo->wSetValueW(L"ADX_IsLooped", -1, loop_enabled_ ? L"True" : L"False", -1);
    set_tag(L"ADX_LoopStartSample", L"%u", loop_start_sample_);
    set_tag(L"ADX_LoopEndSample", L"%u", loop_end_sample_);

    if (adx_header_.common.flags == 0x08)
    {
        pTagInfo->wSetValueW(L"ADX_Flags", -1, L"Encrypted (may not play correctly)", -1);
    }
}

void KpiADXDecoder::fillMediaInfo(DWORD dwNumber)
{
    mediainfo_.dwNumber = dwNumber;
    mediainfo_.dwFormatType = KPI_MEDIAINFO::FORMAT_PCM;
    mediainfo_.dwSampleRate = adx_header_.common.sample_rate;
    mediainfo_.nBitsPerSample = 16;
    mediainfo_.dwChannels = adx_header_.common.channel_count;
    mediainfo_.qwFadeOut = -1;
    mediainfo_.dwSeekableFlags = KPI_MEDIAINFO::SEEK_FLAGS_SAMPLE;

    samples_per_block_ = (adx_header_.common.block_size - sizeof(uint16_t)) * 8 / adx_header_.common.sample_bitdepth;

    mediainfo_.dwUnitSample = samples_per_block_;

    switch (adx_header_.common.version)
    {
    case ADXVersion::ADX_VERSION_3:
        loop_enabled_ = (adx_header_.common.copyright_offset + 4) > ADX_V3_HEADER_SIZE
            && (adx_header_.v3.loop_enabled_1 != 0 || adx_header_.v3.loop_enabled_2 != 0);
        loop_start_sample_ = adx_header_.v3.loop_begin_sample_index;
        loop_end_sample_ = adx_header_.v3.loop_end_sample_index;
        break;
    case ADXVersion::ADX_VERSION_4:
        loop_enabled_ = (adx_header_.common.copyright_offset + 4) > ADX_V4_HEADER_SIZE
            && adx_header_.v4.loop_enabled != 0;
        loop_start_sample_ = adx_header_.v4.loop_begin_sample_index;
        loop_end_sample_ = adx_header_.v4.loop_end_sample_index;
        break;
    case ADXVersion::ADX_VERSION_5:
        loop_enabled_ = false;
        break;
    }

    if (loop_enabled_)
    {
        mediainfo_.qwLength = kpi_SampleTo100ns(loop_end_sample_, adx_header_.common.sample_rate);
        mediainfo_.qwLoop = kpi_SampleTo100ns(static_cast<UINT64>(loop_end_sample_) - loop_start_sample_, adx_header_.common.sample_rate);
    }
    else
    {
        mediainfo_.qwLength = kpi_SampleTo100ns(adx_header_.common.total_samples, adx_header_.common.sample_rate);
        mediainfo_.qwLoop = 0;
        loop_start_sample_ = loop_end_sample_ = 0;
    }
}

DWORD KpiADXDecoder::Select(DWORD dwNumber, const KPI_MEDIAINFO** ppMediaInfo, IKpiTagInfo* pTagInfo, DWORD dwTagGetFlags)
{
    if (dwNumber == 0)
        return 0;

    if (dwNumber > mediainfo_.dwCount)
        return 0;

    if (source_type == SourceType::SOURCE_AFS)
    {
        if (kpi_file_ != nullptr)
            kpi_file_->Release();
        this->kpi_file_ = prepareAFSMember(dwNumber);
    }

    if(isThisADX(kpi_file_) == 0)
        return 0;

    switch(adx_header_.common.encoding_type)
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
        return 0;
    }

    {
        const double a = std::numbers::sqrt2 - std::cos(
            2.0 * std::numbers::pi * (static_cast<double>(adx_header_.common.highpass_frequency) / static_cast<double>(adx_header_.common.sample_rate)));
        const double b = std::numbers::sqrt2 - 1.0;
        const double c = (a - std::sqrt((a + b) * (a - b))) / b; //(a+b)*(a-b) = a*a-b*b, however the simpler formula loses accuracy in floating point

        adx_coeff_[0] = static_cast<int16_t>((c * 2.0) * 4096.0);
        adx_coeff_[1] = static_cast<int16_t>(-(c * c) * 4096.0);
    }

    fillMediaInfo(dwNumber);

    source_buffer_.resize(static_cast<size_t>(adx_header_.common.block_size) * adx_header_.common.channel_count);
    decoded_buffer_.resize(static_cast<size_t>(samples_per_block_) * adx_header_.common.channel_count * 2 * 1);
    loop_buffer_.resize(decoded_buffer_.size() * 2);

    adx_ctx_.resize(adx_header_.common.channel_count);
    decoded_block_.resize(adx_header_.common.channel_count);

    samples_buffered_ = 0;

    reset();

    if (ppMediaInfo != nullptr)
        *ppMediaInfo = &mediainfo_;
    if (pTagInfo != nullptr && ppMediaInfo != nullptr)
    {
        extract_tags(pTagInfo);
    }

    return dwNumber;
}

UINT64 KpiADXDecoder::Seek(UINT64 qwPosSample, DWORD dwFlag)
{
    const UINT64 qwPosSample_save = qwPosSample;

    reset();
    samples_buffered_ = 0;

    if (!loop_enabled_ && qwPosSample > adx_header_.common.total_samples)
        return KPI_FILE_EOF;

    if (loop_enabled_ && qwPosSample > loop_end_sample_)
    {
        const UINT64 loop_length = static_cast<UINT64>(loop_end_sample_) - loop_start_sample_;
        while (qwPosSample > loop_end_sample_)
        {
            qwPosSample -= loop_length;
        }
    }

    seekToExactSample(qwPosSample);
    return qwPosSample_save;
}

UINT64 KpiADXDecoder::seekToBlock(INT64 block_index) const
{
    int64_t seek_to = static_cast<int64_t>(adx_header_.common.copyright_offset) + 4;
    seek_to += block_index * static_cast<int64_t>(adx_header_.common.block_size) * static_cast<int64_t>(adx_header_.common.channel_count);
    return kpi_file_->Seek(seek_to, FILE_BEGIN);
}

UINT64 KpiADXDecoder::seekToExactSample(UINT64 qwPosSample)
{
    const uint64_t block_index = qwPosSample / samples_per_block_;
    seekToBlock(block_index);

    samples_to_skip_ = qwPosSample % samples_per_block_;
    samples_decoded_ = static_cast<uint32_t>(qwPosSample);
    return qwPosSample;
}

int16_t KpiADXDecoder::predictor(const int16_t coeffs[2], int d, int16_t scale, int16_t s[2]) const
{
    const int predict = coeffs[0] * s[0] + coeffs[1] * s[1];
    const int s0 = d * scale + (predict >> 12);
    const auto r = clip<int16_t, int, int16_t>(s0);
    s[1] = s[0];  s[0] = r;
    return r;
}

// 1w + 32h 分デコードする
// https://github.com/Thealexbarney/VGAudio/tree/master/src/VGAudio/Codecs/CriAdx
void KpiADXDecoder::decodeBlock(BYTE* pSource, int16_t* pDest, ADXContext& context) const
{
    int r_scale = get_be<int16_t>(pSource);
    int scale = r_scale & 0x1fff;

    const int16_t* coeff_p = adx_coeff_;
    if(adx_header_.common.encoding_type == ADXEncodingType::ADX_FIXED_COEF)
    {
        const int coeff_index = (r_scale >> 13) & 0x03;
        coeff_p = ADX_TYPE2_COEFF[coeff_index];
    }
    if(adx_header_.common.encoding_type == ADXEncodingType::ADX_EXP_SCALE)
        scale = 1 << (12 - scale);

    pSource += 2;
    bitstream b(pSource, adx_header_.common.block_size - sizeof(int16_t));
    const int sign_decr = (1 << adx_header_.common.sample_bitdepth);
    const int sign_mask = sign_decr >> 1;

    for (int i = 0; i < samples_per_block_; i++)
    {
        int d = b.get(adx_header_.common.sample_bitdepth);
        if (d & sign_mask) d -= sign_decr;
        *pDest++ = predictor(coeff_p, d, scale, context.s);
    }
}

// dwSize (% dwUnitRender == 0) の分だけデコードする
DWORD KpiADXDecoder::decodeBuffer(BYTE* pBuffer, DWORD dwSizeSamples)
{
    DWORD samples_decoded = 0;
    const int channels = adx_header_.common.channel_count;

    while (samples_decoded < dwSizeSamples)
    {
        const DWORD bytes_read = kpi_file_->Read(source_buffer_.data(), static_cast<DWORD>(source_buffer_.size()));
        if (bytes_read < adx_header_.common.block_size * static_cast<unsigned>(channels)) break;	// 1ブロックも読めなければそこで中断
        BYTE* p_source = source_buffer_.data();
        BYTE* p_src_end = p_source + bytes_read;
        while (p_source < p_src_end && samples_decoded < dwSizeSamples)
        {
            for (int ch = 0; ch < channels; ch++)
            {
                decodeBlock(p_source, decoded_block_[ch].s.data(), adx_ctx_[ch]);
                p_source += adx_header_.common.block_size;
            }

            // ADX では各チャンネルのサンプルは 32 samples ごとに
            // LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR ...
            // と並んでいるので，これを WaveAudio の LRLRLRLR ... 形式に直す
            if (pBuffer != nullptr)
            {
                auto* p_output = reinterpret_cast<int16_t*>(pBuffer);
                for (int i = 0; i < samples_per_block_; i++)
                {
                    for (int ch = 0; ch < channels; ch++)
                        *p_output++ = decoded_block_[ch].s[i];
                }
                pBuffer += samples_per_block_ * channels * 2;
            }

            samples_decoded += samples_per_block_;
        }
        if (bytes_read < source_buffer_.size()) break;
    }
    return samples_decoded;
}

DWORD KpiADXDecoder::Render(BYTE* pBuffer, DWORD dwSizeSample)
{
    if (!loop_enabled_)
        return decodeBuffer(pBuffer, dwSizeSample);

    const int channels = adx_header_.common.channel_count;
    const int bytes_per_sample = 2 * channels;
    DWORD dwSamplesRendered = 0;

    while (dwSizeSample > 0)
    {
        if (samples_buffered_ > 0)
        {
            const auto samplesToCopy = (samples_buffered_ > dwSizeSample) ? dwSizeSample : samples_buffered_;

            memcpy(pBuffer, loop_buffer_.data(), static_cast<size_t>(samplesToCopy) * bytes_per_sample);
            dwSizeSample -= samplesToCopy;
            pBuffer += samplesToCopy * bytes_per_sample;
            dwSamplesRendered += samplesToCopy;
            memmove(loop_buffer_.data(), loop_buffer_.data() + samplesToCopy * bytes_per_sample, 
                loop_buffer_.size() - static_cast<size_t>(samplesToCopy) * bytes_per_sample);
            samples_buffered_ -= samplesToCopy;
        }
        if (dwSizeSample == 0) break;

        const DWORD dwDecodedSamples = decodeBuffer(decoded_buffer_.data(), samples_per_block_);
        memcpy(loop_buffer_.data() + (samples_buffered_ * bytes_per_sample), decoded_buffer_.data() + (samples_to_skip_ * bytes_per_sample),
            (static_cast<size_t>(dwDecodedSamples) - samples_to_skip_) * bytes_per_sample);

        samples_decoded_ += dwDecodedSamples - samples_to_skip_;
        samples_buffered_ += dwDecodedSamples - samples_to_skip_;
        samples_to_skip_ = 0;

        if (loop_enabled_ && samples_decoded_ >= loop_end_sample_)
        {
            const auto samples_to_cut = samples_decoded_ - loop_end_sample_;
            samples_decoded_ -= samples_to_cut;
            samples_buffered_ -= samples_to_cut;

            reset();
            seekToExactSample(loop_start_sample_);
        }
        if(!loop_enabled_ && samples_decoded_ >= adx_header_.common.total_samples)
        {
            const auto samples_to_cut = samples_decoded_ - adx_header_.common.total_samples;
            samples_decoded_ -= samples_to_cut;
            samples_buffered_ -= samples_to_cut;
        }
    }
    return dwSamplesRendered;
}
