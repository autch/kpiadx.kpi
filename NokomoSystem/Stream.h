
#pragma once

#include "NokomoSystem.h"

namespace Nokomo
{
  namespace Stream
  {
    class IAuStream
    {
    public:
      virtual ~IAuStream() {}

      virtual BOOL Seek(const QWORD qwPosition) = 0;
      virtual QWORD GetPosition() = 0;
    };
    
    class IAuReadOnlyStream: public IAuStream
    {
    public:
      virtual ~IAuReadOnlyStream() {}
      virtual BOOL Read(VOID* pBuffer, const DWORD dwBytesToRead, DWORD& dwBytesRead) = 0;
    };

    class IAuWriteOnlyStream: public IAuStream
    {
    public:
      virtual ~IAuWriteOnlyStream() {}
      virtual BOOL Write(const VOID* pBuffer, const DWORD dwBytesToWrite, DWORD& dwBytesWritten) = 0;
    };

    class IAuReadWriteStream: public IAuStream
    {
    public:
      virtual ~IAuReadWriteStream() {}
      virtual BOOL Read(VOID* pBuffer, const DWORD dwBytesToRead, DWORD& dwBytesRead) = 0;
      virtual BOOL Write(const VOID* pBuffer, const DWORD dwBytesToWrite, DWORD& dwBytesWritten) = 0;
    };
  };
};
