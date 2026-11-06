#pragma once

#include <string>
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
        SOURCE_AAX,     // 1 song made of ADX segments
        SOURCE_AFS      // archive of ADX/AAX, 1 song per member
    };

    static constexpr int16_t ADX_TYPE2_COEFF[4][2] =
    {
        {0x0000, 0x0000},
        {0x0f00, 0x0000},
        {0x1cc0, static_cast<int16_t>(0xf300u)},
        {0x1880, static_cast<int16_t>(0xf240u)}
    };

    // 1 ADX stream in song_file_.  a song is a sequence of segments (only AAX has 2 or more)
    struct Segment
    {
        uint64_t offset{ 0 };               // offset of the ADX stream in song_file_
        uint64_t end_offset{ 0 };           // end of the ADX stream in song_file_
        ADXheader header{};
        int16_t coeff[2]{ 0, 0 };           // ADX filter coefficients
        int samples_per_block{ 0 };         // # of samples stored in an ADX block, for typical 18 bytes block, this is 32
        uint64_t data_offset{ 0 };          // offset of the first block from the beginning of the ADX stream
        uint32_t inserted_samples{ 0 };     // silent samples at the beginning of the stream, not played
        uint64_t start_sample{ 0 };         // position of the first sample in the song
        uint64_t samples{ 0 };              // # of samples played, excluding inserted_samples
        bool aax_loop{ false };             // AAX lpflg
    };

    struct ADXContext
    {
        int16_t s[2];
    };

    IKpiFile* kpi_file_{ nullptr };         // file passed to Open
    SourceType source_type_{ SourceType::SOURCE_ADX };
    std::vector<AFSitem> afs_items_;
    std::vector<std::wstring> afs_titles_;  // from the file name table, empty if none
    std::vector<std::wstring> afs_dates_;   // from the file name table, "YYYY-MM-DD hh:mm:ss" or empty
    KPI_MEDIAINFO mediainfo_{};

    // the selected song
    IKpiFile* song_file_{ nullptr };        // kpi_file_ itself, or a member of AFS
    std::vector<Segment> segments_;
    int channels_{ 0 };
    int block_size_{ 0 };
    uint64_t total_samples_{ 0 };
    bool loop_enabled_{ false };
    uint64_t loop_start_sample_{ 0 };
    uint64_t loop_end_sample_{ 0 };
    bool song_is_aax_{ false };

    // playback state
    struct PlayState
    {
        size_t segment{ 0 };
        uint64_t next_block{ 0 };           // index of the next block to decode in the segment
        uint64_t position{ 0 };             // position in the song of the next sample to render
        std::vector<ADXContext> ctx;        // ADX history samples per channel, size = # of channels
        std::vector<ADXContext> tail;       // last 2 rendered samples per channel, history for the next segment
        std::vector<int16_t> decoded;       // interleaved PCM of a block, size = (samples per block) * (# of channels)
        uint32_t decoded_read{ 0 };         // # of samples in decoded already rendered
        uint32_t decoded_count{ 0 };        // # of samples in decoded
    };
    PlayState state_;
    bool history_exact_{ false };           // state_.ctx is what decoding from the beginning would give
    PlayState loop_state_;                  // state_ at loop_start_sample_, to repeat the first pass exactly
    bool loop_state_valid_{ false };
    std::vector<BYTE> source_buffer_;       // holds ADX blocks of all channels, size = (ADX block size) * (# of channels)

    void close();
    void clearSong();
    bool loadSong();
    bool addSegment(uint64_t offset, uint64_t size, bool aax_loop);
    void fillMediaInfo(DWORD dwNumber);
    void extract_tags(IKpiTagInfo* pTagInfo) const;
    void setTitle(IKpiTagInfo* pTagInfo, DWORD dwNumber, DWORD dwTagGetFlags) const;

    void positionAt(UINT64 qwPosSample);
    void enterSegment(size_t index, uint64_t sample_in_segment, bool carry_history);
    void restoreLoopState();
    uint64_t blockOffset(const Segment& seg, uint64_t block) const;
    DWORD render(int16_t* pOutput, DWORD dwSizeSample);
    bool decodeNextBlock();
    void decodeBlock(const Segment& seg, BYTE* pSource, int16_t* pDest, ADXContext& context) const;

    static bool readADXHeader(IKpiFile* pFile, UINT64 qwOffset, ADXheader& header);
    DWORD readAFS(IKpiFile* pFile);
    void readAFSNames(IKpiFile* pFile);
};
