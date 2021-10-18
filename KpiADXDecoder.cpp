#include "stdafx.h"
#include "KpiADXDecoder.h"
#include <cmath>
#include <memory>
#include <numeric>

DWORD KpiADXDecoder::IsThisADX(IKpiFile* pFile)
{
    uint8_t signature[6];

    pFile->Seek(0, FILE_BEGIN);
    if (pFile->Read(&header, sizeof header) < sizeof header)
    {
        ::OutputDebugStringW(L"isADX: cannot read whole common\n");
        return 0;
    }

    header.prepare();

    if (header.common.sig_8000 != ADX_SIGNATURE)
    {
        ::OutputDebugStringW(L"isADX: cannot read leading 0x8000\n");
        return 0;
    }

    pFile->Seek(header.common.copyright_offset - 2, FILE_BEGIN);
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

DWORD KpiADXDecoder::IsThisAFS(IKpiFile* pFile)
{
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

    afs_items.resize(afs_header.num_of_files);

    const auto bytes_to_read = sizeof(AFSitem) * afs_header.num_of_files;
    if (pFile->Read(afs_items.data(), bytes_to_read) < bytes_to_read)
    {
        ::OutputDebugStringW(L"isAFS: file is small to hold items\n");
        return 0;
    }

    return afs_header.num_of_files;
}


DWORD KpiADXDecoder::Open(const KPI_MEDIAINFO* cpRequest, IKpiFile* pFile)
{
    pFile->AddRef();
    this->pFile_org = pFile;

    kpi_InitMediaInfo(&mInfo);

    DWORD dwSongs;
    if ((dwSongs = IsThisADX(pFile_org)) > 0)
    {
        mInfo.dwCount = mInfo.dwNumber = dwSongs;
        source_type = SourceType::SOURCE_ADX;
        this->pFile = this->pFile_org;
        this->pFile->AddRef();
    }
    else if ((dwSongs = IsThisAFS(pFile_org)) > 0)
    {
        mInfo.dwCount = dwSongs;
        source_type = SourceType::SOURCE_AFS;
    }
    else
    {
        pFile->Release();
        this->pFile_org = nullptr;
        return 0;
    }

    return mInfo.dwCount;
}

void KpiADXDecoder::Reset()
{
    for (int ch = 0; ch < header.common.channel_count; ch++)
    {
        memset(ctx[ch].s, 0, sizeof ctx[ch].s);
        src[ch].s.resize(m_samples_per_block);
        for (int i = 0; i < m_samples_per_block; i++)
            memset(src[ch].s.data(), 0, sizeof(int16_t) * src[ch].s.size());
    }
    memset(loopBuffer.data(), 0, loopBuffer.size());
    m_samplesToSkip = 0;
    m_dwSamplesDecoded = 0;
    pFile->Seek(header.common.copyright_offset + 4, FILE_BEGIN);
}

void KpiADXDecoder::Close()
{
    if (pFile != nullptr)
    {
        pFile->Release();
        pFile = nullptr;
    }
    if (pFile_org != nullptr)
    {
        pFile_org->Release();
        pFile_org = nullptr;
    }
}

IKpiFile* KpiADXDecoder::prepareAFSMember(DWORD dwNumber)
{
    auto* partial_file = new KpiPartialFile(pFile_org, afs_items[dwNumber - 1].offset,
        afs_items[dwNumber - 1].offset + afs_items[dwNumber - 1].length);
    return partial_file;
}

DWORD KpiADXDecoder::Select(DWORD dwNumber, const KPI_MEDIAINFO** ppMediaInfo, IKpiTagInfo* pTagInfo, DWORD dwTagGetFlags)
{
    uint8_t signature[6];

    if (dwNumber == 0)
        return 0;

    if (dwNumber > mInfo.dwCount)
        return 0;

    if (source_type == SourceType::SOURCE_AFS)
    {
        if (pFile != nullptr)
        {
            pFile->Release();
        }

        this->pFile = prepareAFSMember(dwNumber);
    }

    pFile->Seek(0, FILE_BEGIN);
    if (pFile->Read(&header, sizeof header) < sizeof header)
    {
        ::OutputDebugStringW(L"select: cannot read whole common\n");
        return 0;
    }

    header.prepare();

    if (header.common.sig_8000 != ADX_SIGNATURE)
    {
        ::OutputDebugStringW(L"select: cannot read leading 0x8000\n");
        return 0;
    }

    pFile->Seek(header.common.copyright_offset - 2, FILE_BEGIN);
    if (pFile->Read(signature, 6) != 6)
    {
        ::OutputDebugStringW(L"select: cannot read at signature offset\n");
        return 0;
    }

    if (memcmp(signature, ADX_COPYRIGHT_SIGNATURE, 6) != 0)
    {
        ::OutputDebugStringW(L"select: Signature is not (c)CRI\n");
        return 0;
    }

    {
        constexpr auto pi = 3.14159265358979;
        const double a = std::sqrt(2.0) - std::cos(
            2.0 * pi * (static_cast<double>(header.common.highpass_frequency) / static_cast<double>(header.common.sample_rate)));
        const double b = std::sqrt(2.0) - 1.0;
        const double c = (a - std::sqrt((a + b) * (a - b))) / b; //(a+b)*(a-b) = a*a-b*b, however the simpler formula loses accuracy in floating point

        coeff[0] = c * 2.0;
        coeff[1] = -(c * c);
    }

    mInfo.dwNumber = dwNumber;
    mInfo.dwFormatType = KPI_MEDIAINFO::FORMAT_PCM;
    mInfo.dwSampleRate = header.common.sample_rate;
    mInfo.nBitsPerSample = 16;
    mInfo.dwChannels = header.common.channel_count;
    mInfo.qwFadeOut = -1;
    mInfo.dwSeekableFlags = KPI_MEDIAINFO::SEEK_FLAGS_SAMPLE;

    m_samples_per_block = ((header.common.block_size - sizeof(uint16_t)) * 8) / header.common.sample_bitdepth;

    mInfo.dwUnitSample = m_samples_per_block;

    switch (header.common.version)
    {
    case ADXVersion::ADX_VERSION_3:
        m_LoopEnabled = (header.common.copyright_offset + 4) > ADX_V3_HEADER_SIZE
            && (header.v3.loop_enabled_1 != 0 || header.v3.loop_enabled_2 != 0);
        m_dwLoopStartSample = header.v3.loop_begin_sample_index;
        m_dwLoopEndSample = header.v3.loop_end_sample_index;
        break;
    case ADXVersion::ADX_VERSION_4:
        m_LoopEnabled = (header.common.copyright_offset + 4) > ADX_V4_HEADER_SIZE
            && header.v4.loop_enabled != 0;
        m_dwLoopStartSample = header.v4.loop_begin_sample_index;
        m_dwLoopEndSample = header.v4.loop_end_sample_index;
        break;
    case ADXVersion::ADX_VERSION_5:
        m_LoopEnabled = false;
        break;
    }

    if (m_LoopEnabled)
    {
        mInfo.qwLength = kpi_SampleTo100ns(m_dwLoopEndSample, header.common.sample_rate);
        mInfo.qwLoop = kpi_SampleTo100ns(m_dwLoopEndSample - m_dwLoopStartSample, header.common.sample_rate);
    }
    else
    {
        mInfo.qwLength = kpi_SampleTo100ns(header.common.total_samples, header.common.sample_rate);
        mInfo.qwLoop = 0;
        m_dwLoopStartSample = m_dwLoopEndSample = 0;
    }

    abySource.resize(header.common.block_size * header.common.channel_count);
    decodedBuffer.resize(m_samples_per_block * header.common.channel_count * 2 * 1);
    loopBuffer.resize(decodedBuffer.size() * 2);

    ctx.resize(header.common.channel_count);
    src.resize(header.common.channel_count);

    m_currentPosition = 0;

    Reset();

    if (ppMediaInfo != nullptr)
        *ppMediaInfo = &mInfo;
    if (pTagInfo != nullptr && ppMediaInfo != nullptr)
    {
        auto buf = std::make_shared<wchar_t[]>(128);
        auto set_tag = [buf, pTagInfo](auto name, auto value)
        {
            wsprintfW(buf.get(), L"%d", value);
            pTagInfo->wSetValueW(name, -1, buf.get(), -1);
        };

        const auto bps = (header.common.sample_bitdepth * header.common.sample_rate);
        wsprintf(buf.get(), L"%d.%03dkbps", bps / 1000, bps % 1000);
        pTagInfo->wSetValueW(SZ_KMP_NAME_BITRATE_W, -1, buf.get(), -1);

        set_tag(L"ADX_EncodingType", header.common.encoding_type);
        set_tag(L"ADX_Version", header.common.version);
        set_tag(L"ADX_BlockSize", header.common.block_size);
        set_tag(L"ADX_SampleBitDepth", header.common.sample_bitdepth);
        set_tag(L"ADX_HighPassFrequency", header.common.highpass_frequency);

        set_tag(L"ADX_LoopStartSample", m_dwLoopStartSample);
        set_tag(L"ADX_LoopEndSample", m_dwLoopEndSample);

        if (header.common.flags == 0x08)
        {
            pTagInfo->wSetValueW(L"ADX_Flags", -1, L"Encrypted (may not play correctly)", -1);
        }
    }

    return dwNumber;
}

UINT64 KpiADXDecoder::Seek(UINT64 qwPosSample, DWORD dwFlag)
{
    const UINT64 qwPosSample_save = qwPosSample;

    Reset();
    m_currentPosition = 0;

    if (!m_LoopEnabled && qwPosSample > header.common.total_samples)
        return KPI_FILE_EOF;

    if (qwPosSample > m_dwLoopEndSample)
    {
        const UINT64 loop_length = (m_dwLoopEndSample - m_dwLoopStartSample);
        while (qwPosSample > m_dwLoopEndSample)
        {
            qwPosSample -= loop_length;
        }
    }

    const uint64_t block_index = qwPosSample / m_samples_per_block;
    pFile->Seek(header.common.copyright_offset + 4 + (block_index * header.common.block_size * header.common.channel_count), FILE_BEGIN);

    m_samplesToSkip = qwPosSample % m_samples_per_block;
    m_dwSamplesDecoded = qwPosSample;
    return qwPosSample_save;
}

// 1w + 32h 分デコードする
void KpiADXDecoder::DecodeBlock(BYTE* pSource, int16_t* pDest, ADXContext& context)
{
    const int16_t nScale = get_be<int16_t>(pSource) & 0x1fff;
    int s1 = context.s[0], s2 = context.s[1];

    pSource += 2;
    for (int i = 0; i < header.common.block_size - sizeof(int16_t); i++)
    {
        double predict = coeff[0] * s1 + coeff[1] * s2;
        int d = *pSource >> 4;
        if (d & 0x08)  d -= 0x10;
        int s0 = d * nScale + static_cast<int>(predict);
        if (s0 > 0x7fff)  s0 = 0x7fff;
        if (s0 < -0x7fff)  s0 = -0x7fff;
        *pDest++ = s0 & 0xffff;  s2 = s1;  s1 = s0;

        predict = coeff[0] * s1 + coeff[1] * s2;
        d = *pSource & 0x0f;
        if (d & 0x08)  d -= 0x10;
        s0 = d * nScale + static_cast<int>(predict);
        if (s0 > 0x7fff)  s0 = 0x7fff;
        if (s0 < -0x7fff)  s0 = -0x7fff;
        *pDest++ = s0 & 0xffff;  s2 = s1;  s1 = s0;

        pSource++;
    }
    context.s[0] = s1;
    context.s[1] = s2;
}

// dwSize (% dwUnitRender == 0) の分だけデコードする
DWORD KpiADXDecoder::DecodeBuffer(BYTE* pBuffer, DWORD dwSizeSamples)
{
    DWORD dwSamplesDecoded = 0;
    const int channels = header.common.channel_count;

    while (dwSamplesDecoded < dwSizeSamples)
    {
        const DWORD dwBytesRead = pFile->Read(abySource.data(), abySource.size());
        if (dwBytesRead < header.common.block_size * channels) break;	// 1ブロックも読めなければそこで中断
        BYTE* pSource = abySource.data();
        BYTE* pSrcEnd = pSource + dwBytesRead;
        while (pSource < pSrcEnd && dwSamplesDecoded < dwSizeSamples)
        {
            for (int ch = 0; ch < channels; ch++)
            {
                DecodeBlock(pSource, src[ch].s.data(), ctx[ch]);
                pSource += header.common.block_size;
            }

            // ADX では各チャンネルのサンプルは 32 samples ごとに
            // LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR ...
            // と並んでいるので，これを WaveAudio の LRLRLRLR ... 形式に直す
            if (pBuffer != nullptr)
            {
                auto* pOutput = reinterpret_cast<int16_t*>(pBuffer);
                for (DWORD i = 0; i < m_samples_per_block; i++)
                {
                    for (int ch = 0; ch < channels; ch++)
                        *pOutput++ = src[ch].s[i];
                }
                pBuffer += m_samples_per_block * channels * 2;
            }

            dwSamplesDecoded += m_samples_per_block;
        }
        if (dwBytesRead < abySource.size()) break;
    }
    return dwSamplesDecoded;
}

DWORD KpiADXDecoder::Render(BYTE* pBuffer, DWORD dwSizeSample)
{
    if (!m_LoopEnabled)
        return DecodeBuffer(pBuffer, dwSizeSample);

    const int channels = header.common.channel_count;
    const int bytes_per_sample = 2 * channels;
    DWORD dwSamplesRendered = 0;

    while (dwSizeSample > 0)
    {
        if (m_currentPosition > 0)
        {
            const DWORD samplesToCopy = (m_currentPosition > dwSizeSample) ? dwSizeSample : m_currentPosition;

            memcpy(pBuffer, loopBuffer.data(), samplesToCopy * bytes_per_sample);
            dwSizeSample -= samplesToCopy;
            pBuffer += samplesToCopy * bytes_per_sample;
            dwSamplesRendered += samplesToCopy;
            memmove(loopBuffer.data(), loopBuffer.data() + samplesToCopy * bytes_per_sample, loopBuffer.size() - samplesToCopy * bytes_per_sample);
            m_currentPosition -= samplesToCopy;
        }
        if (dwSizeSample == 0) break;

        DWORD samplesToCut = 0;
        const DWORD dwDecodedSamples = DecodeBuffer(decodedBuffer.data(), m_samples_per_block);
        memcpy(loopBuffer.data() + (m_currentPosition * bytes_per_sample), decodedBuffer.data() + (m_samplesToSkip * bytes_per_sample),
            (dwDecodedSamples - m_samplesToSkip) * bytes_per_sample);
        m_samplesToSkip = 0;

        if (m_dwSamplesDecoded > m_dwLoopEndSample)
        {
            samplesToCut = m_dwSamplesDecoded - m_dwLoopEndSample;
        }

        m_dwSamplesDecoded += dwDecodedSamples - m_samplesToSkip - samplesToCut;
        m_currentPosition += dwDecodedSamples - m_samplesToSkip - samplesToCut;

        if (m_dwSamplesDecoded >= m_dwLoopEndSample)
        {
            Reset();

            const uint64_t block_index = m_dwLoopStartSample / m_samples_per_block;
            pFile->Seek(header.common.copyright_offset + 4 + (block_index * header.common.block_size * header.common.channel_count), FILE_BEGIN);

            m_samplesToSkip = m_dwLoopStartSample % m_samples_per_block;
            m_dwSamplesDecoded = m_dwLoopStartSample;
        }
    }
    return dwSamplesRendered;
}
