
#pragma once

#include "stdafx.h"

// PCM レベルでループを実現する
template<class CSrcDecoder>
class CAuLoopedDecoder
{
private:
  // 本当に使いたいデコーダ
  CSrcDecoder m_Decoder;

  SOUNDINFO m_Info;
  DWORD m_dwLoopStart;
  DWORD m_dwLoopEnd;
  DWORD m_dwSamplesRendered; // ファイル先頭からの通算サンプル数
  DWORD m_dwBufferLeft;      // m_pbyBuffer に実際に入っている PCM のバイト数
  DWORD m_dwBufferCapacity;  // m_pbyBuffer のサイズ
  PBYTE m_pbyBuffer;         // ブロックサイズ以下の中途半端なサイズで転送するためのバッファ

  void Reset()
  {
    m_dwBufferLeft = 0;
    m_dwSamplesRendered = 0;
  }
public:
  CAuLoopedDecoder()
  {
    m_pbyBuffer = NULL;
  }
  ~CAuLoopedDecoder()
  {
    Close();
  }
  VOID Close()
  {
    m_Decoder.Close();
    delete[] m_pbyBuffer; m_pbyBuffer = NULL;
  }
  BOOL Open(LPSTR szFileName, SOUNDINFO* pInfo)
  {
    BOOL r = m_Decoder.Open(szFileName, pInfo);
    if(r)
    {
      m_Info = *pInfo;
      m_dwLoopStart = m_Decoder.GetLoopStart();
      m_dwLoopEnd = m_Decoder.GetLoopEnd();
      m_dwBufferCapacity = m_Info.dwUnitRender * 2;
      m_pbyBuffer = new BYTE[m_dwBufferCapacity];
      Reset();
    }
    return r;
  }
  DWORD SetPosition(DWORD dwPos)
  {
    Reset();
    DWORD r = m_Decoder.SetPosition(dwPos);
    m_dwSamplesRendered = m_Decoder.GetCurrentSamplesRendered();
    return r;
  }
  DWORD BytesToSamples(DWORD dwBytes)
  {
    return dwBytes / (m_Info.dwBitsPerSample >> 3) / m_Info.dwChannels;
  }
  DWORD SamplesToBytes(DWORD dwSamples)
  {
    return dwSamples * (m_Info.dwBitsPerSample >> 3) * m_Info.dwChannels;
  }
  VOID SeekToLoopStart()
  {
    m_Decoder.SeekToLoopStartBlock();
  }
  DWORD Render(BYTE* pBuffer, DWORD dwSize)
  {
    if(m_Info.dwLoopFlag)
    {
      DWORD dwBytesToRender = dwSize;
      while(dwBytesToRender > 0)
      {
        if(m_dwBufferLeft > 0)
        {
          // バッファから吐き出し専門
          DWORD dwBytesToCopy = m_dwBufferLeft > dwBytesToRender ? dwBytesToRender : m_dwBufferLeft;
          CopyMemory(pBuffer, m_pbyBuffer, dwBytesToCopy);
          MoveMemory(m_pbyBuffer, m_pbyBuffer + dwBytesToCopy, m_dwBufferCapacity - dwBytesToCopy);
          pBuffer += dwBytesToCopy;
          m_dwBufferLeft -= dwBytesToCopy;
          dwBytesToRender -= dwBytesToCopy;
        }
        if(dwBytesToRender > 0)
        {
          // バッファへ書き込み専門
          DWORD dwBytesRendered = m_Decoder.Render(m_pbyBuffer + m_dwBufferLeft, dwSize);

          if(m_dwSamplesRendered + BytesToSamples(dwBytesRendered) >= m_dwLoopEnd)
          {
            DWORD dwBytesToSave = SamplesToBytes(m_dwLoopEnd - m_dwSamplesRendered);
            SeekToLoopStart();
            m_dwSamplesRendered = m_dwLoopStart;
            m_dwBufferLeft += dwBytesToSave;
          }
          else
          {
            m_dwSamplesRendered += BytesToSamples(dwBytesRendered);
            m_dwBufferLeft += dwBytesRendered;
          }
        }
      }
      return dwSize;
    }
    else
      return m_Decoder.Render(pBuffer, dwSize);
  }
  VOID SetConfigPath(LPSTR szPath) { m_Decoder.SetConfigPath(szPath); }
};
