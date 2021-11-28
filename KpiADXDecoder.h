#pragma once

#include <vector>

#include "kpi_decoder.h"
#include "kpi_impl.h"
#include "adx.h"
#include "KpiPartialFile.h"

class KpiADXDecoder : public KbKpiDecoderImpl
{
public:
    ~KpiADXDecoder() override { close(); }

    DWORD Open(const KPI_MEDIAINFO* cpRequest, IKpiFile* pFile);

    DWORD  WINAPI Select(DWORD dwNumber,    // 曲番号
        const KPI_MEDIAINFO** ppMediaInfo,  // may be NULL
        IKpiTagInfo* pTagInfo,              // may be NULL
        DWORD dwTagGetFlags) override;      // どのフィールドのタグを取得するかのフラグ
    DWORD  WINAPI Render(BYTE* pBuffer, DWORD dwSizeSample) override;
    UINT64 WINAPI Seek(UINT64 qwPosSample, DWORD dwFlag) override;
    DWORD  WINAPI UpdateConfig(void* pvReserved) override { return 0; }

private:
    IKpiFile* kpi_file_org_{ nullptr };
    IKpiFile* kpi_file_{ nullptr };
    ADXheader adx_header_{};
    KPI_MEDIAINFO mediainfo_{};

    int16_t adx_coeff_[2]{ 0, 0 };                // ADX filter coefficients
    static constexpr int16_t ADX_TYPE2_COEFF[4][2] =
    {
        {0x0000, 0x0000},
        {0x0f00, 0x0000},
        {0x1cc0, static_cast<int16_t>(0xf300u)},
        {0x1880, static_cast<int16_t>(0xf240u)}
    };

    struct ADXContext
    {
        int16_t s[2];
    };
    std::vector<ADXContext> adx_ctx_;       // ADX history samples per channel, size = # of channels
    struct ADXblockSamples
    {
        std::vector<int16_t> s;             // size = samples_per_block_
    };
    std::vector<ADXblockSamples> decoded_block_;
                                            // receives PCM from decodeBlock(), size = # of channels

    std::vector<BYTE> source_buffer_;       // holds ADX blocks from ADX file, size = (ADX block size) * (# of channels)
    std::vector<BYTE> decoded_buffer_;      // holds PCMs from decodeBuffer(), size = sizeof(int16_t) * (ADX block size) * (# of channels)

    bool loop_enabled_{ false };
    int samples_per_block_{ 0 };            // # of samples stored in an ADX block, for typical 18 bytes block, this is 32
    uint32_t samples_decoded_{ 0 };         // number of totally decoded samples, reset when seek()
    uint32_t loop_start_sample_{ 0 };       // they're stored in ADX header as uint32_t, so 32bit is enough
    uint32_t loop_end_sample_{ 0 };

    uint32_t samples_to_skip_{ 0 };         // # of samples, to skip samples from the beginning of decoded_buffer_
    uint32_t samples_buffered_{ 0 };        // # of samples stored in loop_buffer_
    std::vector<BYTE> loop_buffer_;         // holds two decoded_buffer_, to connect PCMs at loop

    // AFS support
    AFSheader afs_header_{};
    std::vector<AFSitem> afs_items_;

    enum class SourceType
    {
        SOURCE_ADX,
        SOURCE_AFS
    };
    SourceType source_type{ SourceType::SOURCE_ADX };

    void close();
    void reset();
    UINT64 seekToBlock(INT64 block_index) const;
    UINT64 seekToExactSample(UINT64 qwPosSample);

    int16_t predictor(const int16_t coeffs[2], int d, int16_t scale, int16_t s[2]) const;
    void decodeBlock(BYTE* pSource, int16_t* pDest, ADXContext& context) const;
    DWORD decodeBuffer(BYTE* pBuffer, DWORD dwSize);

    DWORD isThisADX(IKpiFile* pFile);
    DWORD isThisAFS(IKpiFile* pFile);

    IKpiFile* prepareAFSMember(DWORD dwNumber);
    void extract_tags(IKpiTagInfo* pTagInfo) const;
    void fillMediaInfo(DWORD dwNumber);
};
