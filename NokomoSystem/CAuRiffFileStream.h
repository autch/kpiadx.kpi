
#pragma once

#include "NokomoSystem.h"
#include "FileDrivers.h"

namespace Nokomo
{
  namespace FileDrivers
  {
    class CAuRiffFileStream: public Stream::IAuReadWriteStream
    {
    protected:
      typedef stack<MMCKINFO> sMMCKINFO;
      HMMIO m_hMmio;
      sMMCKINFO m_sChunks;

      MMRESULT CreateChunkSub(const string& fccId, const string& fccType = "", const DWORD dwSize = 0, const DWORD dwFlags = 0)
      {
        MMRESULT r;
        MMCKINFO info;
        ::ZeroMemory(&info, sizeof info);
        if(fccId != "")   info.ckid = ::mmioStringToFOURCC(fccId.c_str(), 0);
        if(fccType != "") info.fccType = ::mmioStringToFOURCC(fccType.c_str(), 0);
        info.cksize = dwSize;
        r = ::mmioCreateChunk(m_hMmio, &info, dwFlags);
        if(r == MMSYSERR_NOERROR) m_sChunks.push(info);
        return r;
      }
      MMRESULT DescendChunkSub(const string& fccId, const string& fccType = "", const DWORD dwFlags = 0)
      {
        MMRESULT r;
        MMCKINFO info;
        ::ZeroMemory(&info, sizeof info);
        if(fccId != "")   info.ckid = ::mmioStringToFOURCC(fccId.c_str(), 0);
        if(fccType != "") info.fccType = ::mmioStringToFOURCC(fccType.c_str(), 0);
        r = ::mmioDescend(m_hMmio, &info, NULL, dwFlags);
        if(r == MMSYSERR_NOERROR) m_sChunks.push(info);
        return r;
      }
    public:
      CAuRiffFileStream() {}
      CAuRiffFileStream(const string& sFileName, const DWORD dwMode) throw(...)
      {
        if(!OpenFromFile(sFileName, dwMode))
          throw new Exceptions::CAuException("Cannot mmioOpen");
      }
      BOOL OpenFromFile(const string& sFileName, const DWORD dwMode)
      {
        m_hMmio = ::mmioOpen(const_cast<LPSTR>(sFileName.c_str()), NULL, dwMode);
        if(m_hMmio) return TRUE;
        return FALSE;
      }
      virtual ~CAuRiffFileStream() { Close(); }
      VOID Close()
      {
        if(m_hMmio)
        {
          while(!m_sChunks.empty())
            Ascend();
          ::mmioFlush(m_hMmio, 0);
          ::mmioClose(m_hMmio, 0);
          m_hMmio = NULL;
        }
      }
      VOID Ascend()
      {
        if(!m_sChunks.empty())
        {
          MMCKINFO info;
          info = m_sChunks.top();
          ::mmioAscend(m_hMmio, &info, 0);
          m_sChunks.pop();
        }
      }
      VOID AscendAll() { while(!m_sChunks.empty()) Ascend(); }
      HMMIO GetHandle() const { return m_hMmio; }
      MMCKINFO GetCurrentChunkInfo()
      {
        return m_sChunks.top();
      }
      BOOL CreateChunk(const string& fccId, const DWORD dwSize = 0, const string& fccType = "")
      {
        return CreateChunkSub(fccId, fccType, dwSize, 0) == MMSYSERR_NOERROR;
      }
      BOOL CreateRiffChunk(const string& fccType, const DWORD dwSize = 0)
      {
        return CreateChunkSub("", fccType, dwSize, MMIO_CREATERIFF) == MMSYSERR_NOERROR;
      }
      BOOL CreateListChunk(const string& fccId, const DWORD dwSize = 0, const string& fccType = "")
      {
        return CreateChunkSub(fccId, fccType, dwSize, MMIO_CREATELIST) == MMSYSERR_NOERROR;
      }
      BOOL FindChunk(const string& fccId, const string& fccType = "")
      {
        return DescendChunkSub(fccId, fccType, MMIO_FINDCHUNK) == MMSYSERR_NOERROR;
      }
      BOOL FindRiffChunk(const string& fccType)
      {
        return DescendChunkSub("", fccType, MMIO_FINDRIFF) == MMSYSERR_NOERROR;
      }
      BOOL FindListChunk(const string& fccId, const string& fccType = "")
      {
        return DescendChunkSub(fccId, fccType, MMIO_FINDLIST) == MMSYSERR_NOERROR;
      }
      BOOL Write(const VOID* pBuffer, const DWORD dwBytesToWrite, DWORD& dwBytesWritten)
      {
        dwBytesWritten = ::mmioWrite(m_hMmio, reinterpret_cast<const char*>(pBuffer), dwBytesToWrite);
        return dwBytesWritten != -1;
      }
      BOOL Seek(const QWORD qwPosition)
      {
        return ::mmioSeek(m_hMmio, static_cast<DWORD>(qwPosition), SEEK_SET) != -1;
      }
      BOOL Seek(const DWORD dwPosition)
      {
        return ::mmioSeek(m_hMmio, dwPosition, SEEK_SET) != -1;
      }
      QWORD GetPosition()
      {
        return ::mmioSeek(m_hMmio, 0, SEEK_CUR);
      }
      BOOL Read(VOID* pBuffer, const DWORD dwBytesToRead, DWORD& dwBytesRead)
      {
        dwBytesRead = ::mmioRead(m_hMmio, reinterpret_cast<char*>(pBuffer), dwBytesToRead);
        return dwBytesRead != 0xffffffff;
      }
    };
  }
 };
