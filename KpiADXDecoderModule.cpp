#include "stdafx.h"
#include "KpiADXDecoderModule.h"
#include "KpiADXDecoder.h"

#include "kpiadx.h"
#include "adx.h"

// {DBCA462E-44CF-4085-ABCB-418FDDFA2B6F}
static constexpr GUID KPIADX_GUID =
{ 0xdbca462e, 0x44cf, 0x4085, { 0xab, 0xcb, 0x41, 0x8f, 0xdd, 0xfa, 0x2b, 0x6f } };

static constexpr KPI_DECODER_MODULEINFO decoder_info =
{
    .cb = sizeof(KPI_DECODER_MODULEINFO),
    .dwModuleVersion = KPI_DECODER_MODULE_VERSION,
    .dwPluginVersion = KPIADX_VERSION,
    .dwMultipleInstance = KPI_MULTINST_INFINITE,
    .guid = KPIADX_GUID,
    .cszDescription = KPIADX_DESC,
    .cszCopyright = KPIADX_COPYRIGHT,
    .cszSupportExts = L".adx/.afs",
    .cszMultiSongExts = L".afs",
    .pvReserved1 = nullptr,
    .pvReserved2 = nullptr,
    .dwSupportTagInfo = 0,
    .dwSupportConfig = 0,
    .dwReserved = { 0, 0, 0, 0 }
};

void KpiADXDecoderModule::GetModuleInfo(const KPI_DECODER_MODULEINFO** ppInfo)
{
    *ppInfo = &decoder_info;
}

DWORD KpiADXDecoderModule::Open(const KPI_MEDIAINFO* cpRequest, IKpiFile* pFile, IKpiFolder* pFolder, IKpiDecoder** ppDecoder)
{
    auto* decoder = new KpiADXDecoder();
    const DWORD dwCount = decoder->Open(cpRequest, pFile);
    if (dwCount == 0)
    {
        ::OutputDebugStringW(L"decoder returned no songs");
        decoder->Release();
        *ppDecoder = nullptr;
        return 0;
    }
    *ppDecoder = static_cast<IKpiDecoder*>(decoder);
    return dwCount;
}
