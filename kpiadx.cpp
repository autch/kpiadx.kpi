// kpiadx.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include "kpi.h"
#include "kpi_decoder.h"
#include "kpiadx.h"

#include "KpiADXDecoderModule.h"

HMODULE g_hModule;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReasonForCall, LPVOID lpReserved)
{
	if (dwReasonForCall == DLL_PROCESS_ATTACH)
	{
		g_hModule = (HMODULE)hModule;
		DisableThreadLibraryCalls(g_hModule);
	}
	return TRUE;
}

HRESULT WINAPI kpi_CreateInstance(REFIID riid, void **ppvObject, IKpiUnknown *pUnknown)
{
	if(IsEqualIID(riid, IID_IKpiDecoderModule))
	{
		const auto pModule = new KpiADXDecoderModule();
		*ppvObject = static_cast<IKpiDecoderModule*>(pModule);
		return S_OK;
	}
	*ppvObject = nullptr;
	return E_NOINTERFACE;
}
