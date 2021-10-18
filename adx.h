#pragma once

// reference: https://en.wikipedia.org/wiki/ADX_(file_format)

#include <cstdint>
#include <winsock.h>

// note: BIG ENDIAN

constexpr auto ADX_SIGNATURE = 0x8000;
constexpr auto ADX_COPYRIGHT_SIGNATURE = "(c)CRI";

enum class ADXEncodingType: uint8_t
{
	ADX_PRESET_COEF = 0x02,
	ADX_STANDARD	= 0x03,
	ADX_EXP_SCALE	= 0x04,
	ADX_AHX_10		= 0x10,
	ADX_AHX_11		= 0x11,
};

enum class ADXVersion: uint8_t
{
	ADX_VERSION_3	= 0x03,
	ADX_VERSION_4   = 0x04,
	ADX_VERSION_5	= 0x05,
};

constexpr auto ADX_HEADER_SIZE = 0x14;	// 20
constexpr auto ADX_V3_HEADER_SIZE = 0x2c;	// 44
constexpr auto ADX_V4_HEADER_SIZE = 0x38;	// 56

constexpr auto ADX_V3_PART_SIZE = (ADX_V3_HEADER_SIZE - ADX_HEADER_SIZE);
constexpr auto ADX_V4_PART_SIZE = (ADX_V4_HEADER_SIZE - ADX_HEADER_SIZE);


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

struct ADXheaderV3
{
	// 0x14
	uint16_t loop_alignment_samples;
	uint16_t loop_enabled_1;
	uint32_t loop_enabled_2;
	uint32_t loop_begin_sample_index;
	// 0x20
	uint32_t loop_begin_byte_index;
	uint32_t loop_end_sample_index;
	uint32_t loop_end_byte_index;

	inline void prepare()
	{
		loop_alignment_samples = ntohs(loop_alignment_samples);
		loop_enabled_1 = ntohs(loop_enabled_1);
		loop_enabled_2 = ntohl(loop_enabled_2);
		loop_begin_sample_index = ntohl(loop_begin_sample_index);

		loop_begin_byte_index = ntohl(loop_begin_byte_index);
		loop_end_sample_index = ntohl(loop_end_sample_index);
		loop_end_byte_index = ntohl(loop_end_byte_index);
	}
};

static_assert(sizeof(ADXheaderV3) == ADX_V3_PART_SIZE, "sizeof ADX V3 common is not 0x2c bytes.  maybe padding?");

struct ADXheaderV4
{
	// 0x14
	uint8_t reserved[16];
	// 0x24
	uint32_t loop_enabled;
	uint32_t loop_begin_sample_index;
	uint32_t loop_begin_byte_index;
	// 0x30
	uint32_t loop_end_sample_index;
	uint32_t loop_end_byte_index;

	inline void prepare()
	{
		loop_enabled = ntohl(loop_enabled);
		loop_begin_sample_index = ntohl(loop_begin_sample_index);
		loop_begin_byte_index = ntohl(loop_begin_byte_index);

		loop_end_sample_index = ntohl(loop_end_sample_index);
		loop_end_byte_index = ntohl(loop_end_byte_index);
	}
};

static_assert(sizeof(ADXheaderV4) == ADX_V4_PART_SIZE, "sizeof ADX V4 common is not 0x38 bytes.  maybe padding?");

struct ADXheader
{
	ADXheaderCommon common;
	union
	{
		ADXheaderV3 v3;
		ADXheaderV4 v4;
	};

	inline void prepare()
	{
		common.prepare();

		switch(common.version)
		{
		case ADXVersion::ADX_VERSION_3:
			v3.prepare();
			break;
		case ADXVersion::ADX_VERSION_4:
		case ADXVersion::ADX_VERSION_5:
			v4.prepare();
			break;
		}
	}
};

static_assert(sizeof(ADXheader) == ADX_V4_HEADER_SIZE, "sizeof ADXheader is not 0x38 bytes.  maybe padding?");

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
