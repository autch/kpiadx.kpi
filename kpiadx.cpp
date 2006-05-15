// kpiadx.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "kpiEntryPoints.h"

#define KPIADX_VERSION   ((1 << 8) | 2)      // 1.2

#ifdef _DEBUG
#define KPIADX_DESC      "CRI ADX decoder plugin for KbMedia Player [DEBUG]"
#else
#define KPIADX_DESC      "CRI ADX decoder plugin for KbMedia Player"
#endif
#define KPIADX_COPYRIGHT "Copyright (c) 2006, Autch."

HMODULE g_hModule;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReasonForCall, LPVOID lpReserved)
{
  if(dwReasonForCall == DLL_PROCESS_ATTACH)
  {
    g_hModule = (HMODULE)hModule;
    DisableThreadLibraryCalls(g_hModule);
  }
  return TRUE;
}

extern "C" KMPMODULE* WINAPI kmp_GetTestModule()
{
  static const char* pszExts[] = { ".adx", NULL };

  static KMPMODULE kpiModule =
  {
		KMPMODULE_VERSION,				// DWORD dwVersion;
		KPIADX_VERSION,    		    // DWORD dwPluginVersion;
		KPIADX_COPYRIGHT, 		    // const char	*pszCopyright;
		KPIADX_DESC,						  // const char	*pszDescription;
		pszExts,									// const char	**ppszSupportExts;
		1,												// DWORD dwReentrant;
		kpiInit,								  // void (WINAPI *Init)(void);
		kpiDeinit,							  // void (WINAPI *Deinit)(void);
		kpiOpen,									// HKMP (WINAPI *Open)(const char *cszFileName, SOUNDINFO *pInfo);
		NULL,	            				// HKMP (WINAPI *OpenFromBuffer)(const BYTE *Buffer, DWORD dwSize, SOUNDINFO *pInfo);
		kpiClose,									// void (WINAPI *Close)(HKMP hKMP);
		kpiRender,								// DWORD (WINAPI *Render)(HKMP hKMP, BYTE* Buffer, DWORD dwSize);
		kpiSetPosition						// DWORD (WINAPI *SetPosition)(HKMP hKMP, DWORD dwPos);
  };
  return &kpiModule;
}
