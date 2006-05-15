
#pragma once

#include "NokomoSystem.h"

namespace Nokomo
{
  namespace Exceptions
  {
    class CAuException
    {
    protected:
      DWORD m_dwErrorCode;
      string m_sErrorMsg;
    public:
      CAuException() { m_dwErrorCode = 0; m_sErrorMsg = ""; }
      CAuException(const DWORD dwErrorCode)
      {
        m_dwErrorCode = dwErrorCode;
      }
      CAuException(const string& sErrorMsg)
      {
        m_sErrorMsg = sErrorMsg;
      }
      CAuException(const DWORD dwErrorCode, const string& sErrorMsg)
      {
        m_dwErrorCode = dwErrorCode;
        m_sErrorMsg = sErrorMsg;
      }
      virtual ~CAuException() {}

      virtual string& GetMessage()       { return m_sErrorMsg; }
      virtual DWORD GetErrorCode() const { return m_dwErrorCode; }
    };
  };
};
