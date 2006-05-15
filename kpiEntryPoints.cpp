
#include "stdafx.h"
#include "kpiEntryPoints.h"
#include "CAuADXDecoder.h"
#include "CAuLoopedDecoder.h"

#include <string>

typedef CAuLoopedDecoder<CAuADXDecoder> CAuLoopedADXDecoder;

extern HMODULE g_hModule; // kpiadx.cpp

namespace
{
  CHAR s_szExeName[1024];
}

void WINAPI kpiInit()
{
  GetModuleFileName(g_hModule, s_szExeName, 1024);
  strcpy(strrchr(s_szExeName, '.'), ".ini");
}

void WINAPI kpiDeinit()
{
}

HKMP WINAPI kpiOpen(const char* cszFileName, SOUNDINFO* pInfo)
{
  CAuLoopedADXDecoder* pDecoder = new CAuLoopedADXDecoder();

  pDecoder->SetConfigPath(s_szExeName);

  if(pDecoder->Open((LPSTR)cszFileName, pInfo))
    return (HKMP)pDecoder;
  else
    delete pDecoder;

  return NULL;
}

void WINAPI kpiClose(HKMP hKMP)
{
  CAuLoopedADXDecoder* pDecoder = (CAuLoopedADXDecoder*)hKMP;
  if(pDecoder)
    delete pDecoder;
}

DWORD WINAPI kpiRender(HKMP hKMP, BYTE* Buffer, DWORD dwSize)
{
  CAuLoopedADXDecoder* pDecoder = (CAuLoopedADXDecoder*)hKMP;
  if(pDecoder)
    return pDecoder->Render(Buffer, dwSize);
  return 0;
}

DWORD WINAPI kpiSetPosition(HKMP hKMP, DWORD dwPos)
{
  CAuLoopedADXDecoder* pDecoder = (CAuLoopedADXDecoder*)hKMP;
  if(pDecoder)
    return pDecoder->SetPosition(dwPos);
  return 0;
}
