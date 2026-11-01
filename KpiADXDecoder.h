#pragma once

#include <vector>

#include "kpi_decoder.h"
#include "kpi_impl.h"
#include "adx.h"

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
    enum class SourceType
    {
        SOURCE_ADX,     // 1 ADX stream
        SOURCE_AFS      // archive of ADX, 1 song per member
    };

    static constexpr int16_t ADX_TYPE2_COEFF[4][2] =
    {
        {0x0000, 0x0000},
        {0x0f00, 0x0000},
        {0x1cc0, static_cast<int16_t>(0xf300u)},
        {0x1880, static_cast<int16_t>(0xf240u)}
    };

    // 1 ADX stream.  a song is a sequence of segments
    struct Segment
    {
        IKpiFile* file{ nullptr };          // owns a reference
        ADXheader header{};
        int16_t coeff[2]{ 0, 0 };           // ADX filter coefficients
        int samples_per_block{ 0 };         // # of samples stored in an ADX block, for typical 18 bytes block, this is 32
        UINT64 data_offset{ 0 };            // offset of the first block in file
        UINT64 start_sample{ 0 };           // position of the first sample in the song
        UINT64 samples{ 0 };
    };

    struct ADXContext
    {
        int16_t s[2];
    };

    IKpiFile* kpi_file_{ nullptr };         // file passed to Open
    SourceType source_type_{ SourceType::SOURCE_ADX };
    std::vector<AFSitem> afs_items_;
    KPI_MEDIAINFO mediainfo_{};

    // the selected song
    std::vector<Segment> segments_;
    int channels_{ 0 };
    int block_size_{ 0 };
    UINT64 total_samples_{ 0 };
    bool loop_enabled_{ false };
    UINT64 loop_start_sample_{ 0 };
    UINT64 loop_end_sample_{ 0 };

    // playback state
    struct PlayState
    {
        size_t segment{ 0 };
        UINT64 next_block{ 0 };             // index of the next block to decode in the segment
        UINT64 position{ 0 };               // position in the song of the next sample to render
        std::vector<ADXContext> ctx;        // ADX history samples per channel, size = # of channels
        std::vector<int16_t> decoded;       // interleaved PCM of a block, size = (samples per block) * (# of channels)
        UINT32 decoded_read{ 0 };           // # of samples in decoded already rendered
        UINT32 decoded_count{ 0 };          // # of samples in decoded
    };
    PlayState state_;
    bool history_exact_{ false };           // state_.ctx is what decoding from the beginning would give
    PlayState loop_state_;                  // state_ at loop_start_sample_, to repeat the first pass exactly
    bool loop_state_valid_{ false };
    std::vector<BYTE> source_buffer_;       // holds ADX blocks of all channels, size = (ADX block size) * (# of channels)

    void close();
    void clearSong();
    bool loadSong(IKpiFile* pSong);
    bool addSegment(IKpiFile* pFile);
    void fillMediaInfo(DWORD dwNumber);
    void extract_tags(IKpiTagInfo* pTagInfo) const;

    void positionAt(UINT64 qwPosSample);
    void enterSegment(size_t index, UINT64 sample_in_segment);
    void restoreLoopState();
    DWORD render(int16_t* pOutput, DWORD dwSizeSample);
    bool decodeNextBlock();
    void decodeBlock(const Segment& seg, BYTE* pSource, int16_t* pDest, ADXContext& context) const;

    static bool readADXHeader(IKpiFile* pFile, ADXheader& header);
    DWORD readAFS(IKpiFile* pFile);
};
