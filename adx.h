#pragma once

// reference: https://en.wikipedia.org/wiki/ADX_(file_format)

#include <cstdint>
#include <winsock.h>

// note: BIG ENDIAN

constexpr auto ADX_SIGNATURE = 0x8000;
constexpr auto ADX_COPYRIGHT_SIGNATURE = "(c)CRI";

enum class ADXEncodingType : uint8_t
{
    ADX_FIXED_COEF = 0x02,
    ADX_LINEAR_SCALE = 0x03,
    ADX_EXP_SCALE = 0x04,
    ADX_AHX_10 = 0x10,
    ADX_AHX_11 = 0x11,
};

enum class ADXVersion : uint8_t
{
    ADX_VERSION_3 = 0x03,
    ADX_VERSION_4 = 0x04,
    ADX_VERSION_5 = 0x05,
};

constexpr auto ADX_HEADER_SIZE = 0x14;	// 20
constexpr auto ADX_V3_LOOP_OFFSET = 0x14;	// v3 has the loop information just after the common part
constexpr auto ADX_V4_HISTORY_OFFSET = 0x18;	// v4 has the history samples and then the loop information
constexpr auto ADX_V4_MIN_HISTORY_SIZE = 0x08;	// 2 x int16_t per channel, at least 2 channels

struct ADXheaderCommon
{
    uint16_t sig_8000;	// 0x8000
    uint16_t copyright_offset;	// if == 0x14, there's no loop info
    ADXEncodingType encoding_type;
    uint8_t block_size;			// typ. 18
    uint8_t sample_bitdepth;	// typ. 4
    uint8_t channel_count;
    uint32_t sample_rate;
    uint32_t total_samples;
    // 0x10
    uint16_t highpass_frequency;
    ADXVersion version;
    uint8_t flags;							// 0x08: encrypted in XOR

    void prepare()
    {
        sig_8000 = ntohs(sig_8000);
        copyright_offset = ntohs(copyright_offset);
        sample_rate = ntohl(sample_rate);
        total_samples = ntohl(total_samples);
        highpass_frequency = ntohs(highpass_frequency);
    }
};

static_assert(sizeof(ADXheaderCommon) == ADX_HEADER_SIZE, "sizeof ADX common common is not 20 bytes.  maybe padding?");

// loop information of v3 and v4
// reference: VGAudio AdxReader, vgmstream meta/adx.c
struct ADXloop
{
    uint16_t inserted_samples;	// silent samples inserted at the beginning, to align the loop start to a block
    uint16_t loop_count;		// 0 if not looped
    uint32_t loop_flag;			// non-zero if looped
    uint32_t loop_begin_sample_index;
    uint32_t loop_begin_byte_index;
    uint32_t loop_end_sample_index;
    uint32_t loop_end_byte_index;

    inline void prepare()
    {
        inserted_samples = ntohs(inserted_samples);
        loop_count = ntohs(loop_count);
        loop_flag = ntohl(loop_flag);
        loop_begin_sample_index = ntohl(loop_begin_sample_index);
        loop_begin_byte_index = ntohl(loop_begin_byte_index);
        loop_end_sample_index = ntohl(loop_end_sample_index);
        loop_end_byte_index = ntohl(loop_end_byte_index);
    }
};

static_assert(sizeof(ADXloop) == 0x18, "sizeof ADXloop is not 0x18 bytes.  maybe padding?");

// offset of ADXloop in the header, or 0 if the version has no loop information
// v4 places it after the history samples, so it moves when there are 3 or more channels
inline uint32_t adx_loop_offset(const ADXheaderCommon& common)
{
    switch (common.version)
    {
    case ADXVersion::ADX_VERSION_3:
        return ADX_V3_LOOP_OFFSET;
    case ADXVersion::ADX_VERSION_4:
    {
        const uint32_t history_size = 4u * common.channel_count;
        return ADX_V4_HISTORY_OFFSET + (history_size > ADX_V4_MIN_HISTORY_SIZE ? history_size : ADX_V4_MIN_HISTORY_SIZE);
    }
    default:
        // v5 has no loop
        return 0;
    }
}

struct ADXheader
{
    ADXheaderCommon common;
    ADXloop loop;               // all zero if !has_loop_info
    bool has_loop_info;         // the header is long enough to hold ADXloop
};

// https://azuco.sakura.ne.jp/column/ag_adx/cri.html
// note: AFS is in LITTLE ENDIAN

constexpr auto AFS_SIGNATURE = "AFS\0";

struct AFSheader
{
    char signature[4];	// "AFS\0"
    uint32_t num_of_files;
};

struct AFSitem
{
    uint32_t offset;
    uint32_t length;
};

// file name table of AFS (optional)
// its offset and size are stored as an AFSitem just after the item table,
// or (in some archives) just before the first item
struct AFSname
{
    char name[32];          // not always NUL terminated
    uint16_t year;
    uint16_t month;
    uint16_t day;
    uint16_t hour;
    uint16_t minute;
    uint16_t second;
    uint32_t unreliable;    // often a copy of the item table shifted by one, not the file size
};

static_assert(sizeof(AFSname) == 0x30, "sizeof AFSname is not 0x30 bytes.  maybe padding?");
