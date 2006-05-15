
#pragma once

#include "NokomoSystem.h"

namespace Nokomo
{
  namespace UnitConvs
  {
    class CAuPcmUnitConverter
    {
    private:
      DWORD m_dwSamplesPerSec, m_dwBitsPerSample, m_dwChannels;
    public:
      CAuPcmUnitConverter() { SetPcmParameters(0, 0, 0); }
      CAuPcmUnitConverter(const DWORD dwSamplesPerSec, const DWORD dwBitsPerSample, const DWORD dwChannels)
      {
        SetPcmParameters(dwSamplesPerSec, dwBitsPerSample, dwChannels);
      }
      VOID SetPcmParameters(const DWORD dwSamplesPerSec, const DWORD dwBitsPerSample, const DWORD dwChannels)
      {
        m_dwSamplesPerSec = dwSamplesPerSec;
        m_dwBitsPerSample = dwBitsPerSample;
        m_dwChannels = dwChannels;
      }
      DWORD BytesPerSecond() const { return m_dwSamplesPerSec * (m_dwBitsPerSample >> 3) * m_dwChannels; }
      DWORD BytesToMSecs(const DWORD dwBytes) const { return MulDiv(dwBytes, 1000, BytesPerSecond()); }
      DWORD MSecsToBytes(const DWORD dwMSecs) const { return MulDiv(dwMSecs, BytesPerSecond(), 1000); }
    };
    class CAuTimeUnitConverter
    {
    public:
      static VOID MSecsToMSMSecs(const DWORD dwMSecs, DWORD& dwMinutes, DWORD& dwSeconds, DWORD& dwMilliSeconds)
      {
        dwMinutes = dwMSecs / 60000;
        dwSeconds = (dwMSecs / 1000) % 60;
        dwMilliSeconds = dwMSecs % 1000;
      }
      static DWORD MSMSecsToMSecs(const DWORD dwMinutes, const DWORD dwSeconds, const DWORD dwMilliSeconds)
      {
        return dwMinutes * 60000 + dwSeconds * 60 + dwMilliSeconds;
      }
    };
  };
};
