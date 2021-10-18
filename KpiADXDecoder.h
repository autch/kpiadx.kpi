#pragma once

#include <vector>

#include "kpi_decoder.h"
#include "kpi_impl.h"
#include "adx.h"
#include "KpiPartialFile.h"

class KpiADXDecoder : public KbKpiDecoderImpl
{
public:
	~KpiADXDecoder() override { Close(); }

	DWORD WINAPI Open(const KPI_MEDIAINFO* cpRequest, IKpiFile* pFile);
	VOID Close();

	DWORD  WINAPI Select(DWORD dwNumber,                    //曲番号
		const KPI_MEDIAINFO** ppMediaInfo, //may be NULL
		IKpiTagInfo* pTagInfo,             //may be NULL
		DWORD dwTagGetFlags) override;//どのフィールドのタグを取得するかのフラグ
	DWORD  WINAPI Render(BYTE* pBuffer, DWORD dwSizeSample) override;
	UINT64 WINAPI Seek(UINT64 qwPosSample, DWORD dwFlag) override;
	DWORD  WINAPI UpdateConfig(void* pvReserved) override { return 0; }

	VOID WINAPI Reset();

private:
	IKpiFile* pFile_org{nullptr};
	IKpiFile* pFile{nullptr};
	ADXheader header{};
	KPI_MEDIAINFO mInfo{};

	double coeff[2]{ 0, 0 };                // フィルタ係数

	static constexpr uint16_t ADX_TYPE2_COEFF[4][2] =
	{
		{0x0000, 0x0000},
		{0x0f00, 0x0000},
		{0x1cc0, 0xf300},
		{0x1880, 0xf240}
	};

	struct ADXContext
	{
		int32_t s[2];
	};
	std::vector<ADXContext> ctx;            // チャンネルごとの ADX 履歴値
	struct ADXblockSamples
	{
		std::vector<int16_t> s;
	};
	std::vector<ADXblockSamples> src;       // DecodeBlock の結果を受け取る

	template<typename T>
	T get_be(BYTE* p);

	template<>
	int16_t get_be(BYTE* p)
	{
		const int16_t i = *reinterpret_cast<int16_t*>(p);
		return ntohs(i);
	}

	// static constexpr int BLOCKS_PER_ONCE = (1<<7);
	std::vector<BYTE> abySource;

	bool m_LoopEnabled{ false };
	int m_samples_per_block{0};
	uint32_t m_dwSamplesDecoded{0};
	uint32_t m_dwLoopStartSample{0};
	uint32_t m_dwLoopEndSample{0};

	uint32_t m_samplesToSkip{0};
	uint32_t m_currentPosition{0};
	std::vector<BYTE> loopBuffer;           // ループをつなげるための decodedBuffer * 2
	std::vector<BYTE> decodedBuffer;        // デコード一回分のPCM

	void DecodeBlock(BYTE* pSource, int16_t* pDest, ADXContext& context);
	DWORD DecodeBuffer(BYTE* pBuffer, DWORD dwSize);

	DWORD IsThisADX(IKpiFile* pFile);
	DWORD IsThisAFS(IKpiFile* pFile);

	IKpiFile* prepareAFSMember(DWORD dwNumber);

	AFSheader afs_header{};
	std::vector<AFSitem> afs_items;

	enum class SourceType
	{
		SOURCE_ADX,
		SOURCE_AFS
	};
	SourceType source_type{SourceType::SOURCE_ADX};
};
