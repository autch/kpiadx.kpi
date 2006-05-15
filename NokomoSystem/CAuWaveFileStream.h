
#pragma once

#include "NokomoSystem.h"
#include "FileDrivers.h"

namespace Nokomo
{
  namespace FileDrivers
  {
    class CAuWaveFileStream: public Stream::IAuReadWriteStream
    {
    private:
      WAVEFORMATEX m_Wfex;
      CAuRiffFileStream m_Riff;
    public:
      CAuWaveFileStream() {}
      CAuWaveFileStream(const string& sFileName, const DWORD dwMode) throw(...)
      {
        OpenFromFile(sFileName, dwMode);
      }
      BOOL OpenFromFile(const string& sFileName, const DWORD dwMode) throw(...)
      {
        return m_Riff.OpenFromFile(sFileName, dwMode);
      }
      virtual ~CAuWaveFileStream(){ Close(); }
      VOID Close()
      {
        m_Riff.Close();
      }
      QWORD GetPosition()
      {
        return m_Riff.GetPosition();
      }
      VOID Ascend() { m_Riff.Ascend(); }
      VOID AscendAll() { m_Riff.AscendAll(); }
      HMMIO GetHandle() const { return m_Riff.GetHandle(); }
      MMCKINFO GetCurrentChunkInfo() { return m_Riff.GetCurrentChunkInfo(); }

      BOOL Write(const VOID* pBuffer, const DWORD dwBytesToWrite, DWORD& dwBytesWritten)
      {
        return m_Riff.Write(pBuffer, dwBytesToWrite, dwBytesWritten);
      }
      BOOL Seek(const QWORD qwPosition)
      {
        return m_Riff.Seek(qwPosition);
      }
      BOOL Seek(const DWORD dwPosition)
      {
        return m_Riff.Seek(dwPosition);
      }
      BOOL Read(VOID* pBuffer, const DWORD dwBytesToRead, DWORD& dwBytesRead)
      {
        return m_Riff.Read(pBuffer, dwBytesToRead, dwBytesRead);
      }

      BOOL CreateChunk(const string& fccId, const DWORD dwSize = 0, const string& fccType = "")
      {
        return m_Riff.CreateChunk(fccId, dwSize, fccType);
      }
      BOOL CreateRiffChunk(const string& fccType, const DWORD dwSize = 0)
      {
        return m_Riff.CreateRiffChunk(fccType, dwSize);
      }
      BOOL CreateListChunk(const string& fccId, const DWORD dwSize = 0, const string& fccType = "")
      {
        return m_Riff.CreateListChunk(fccId, dwSize, fccType);
      }
      BOOL FindChunk(const string& fccId, const string& fccType = "")
      {
        return m_Riff.FindChunk(fccId, fccType);
      }
      BOOL FindRiffChunk(const string& fccType)
      {
        return m_Riff.FindRiffChunk(fccType);
      }
      BOOL FindListChunk(const string& fccId, const string& fccType = "")
      {
        return m_Riff.FindListChunk(fccId, fccType);
      }
      BOOL SetWaveFormatEx(const WAVEFORMATEX& wfex)
      {
        DWORD dwBytesWritten;
        m_Riff.CreateRiffChunk("WAVE");
        m_Riff.CreateChunk("fmt ", sizeof wfex);
        m_Riff.Write(&wfex, sizeof wfex, dwBytesWritten);
        m_Riff.Ascend();
        m_Riff.CreateChunk("data");
        return TRUE;
      }
      BOOL SetInformationList(const string& sTitle, const string& sArtist)
      {
        DWORD dwBytesWritten;
        m_Riff.CreateListChunk("LIST", 0, "INFO");
        {
          m_Riff.CreateChunk("INAM");
            m_Riff.Write(sTitle.c_str(), sTitle.size(), dwBytesWritten);
          m_Riff.Ascend();
          m_Riff.CreateChunk("IART");
            m_Riff.Write(sArtist.c_str(), sArtist.size(), dwBytesWritten);
          m_Riff.Ascend();
        }
        m_Riff.Ascend();
        return TRUE;
      }
    };
  };
};
