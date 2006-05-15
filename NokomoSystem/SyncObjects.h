
#pragma once

#include "NokomoSystem.h"

namespace Nokomo
{
  namespace SyncObjects
  {
    class CAuCriticalSection
    {
    protected:
      CRITICAL_SECTION m_cs;
    public:
      CAuCriticalSection() { ::InitializeCriticalSection(&m_cs); }
      virtual ~CAuCriticalSection() { ::DeleteCriticalSection(&m_cs); }

      void Enter() { ::EnterCriticalSection(&m_cs); }
      void Leave() { ::LeaveCriticalSection(&m_cs); }

      void Lock() { Enter(); }
      void Unlock() { Leave(); }
    };

    class CAuCriticalSectionEx
    {
    protected:
      CAuCriticalSection* m_cs;
    public:
      CAuCriticalSectionEx(CAuCriticalSection* cs) { m_cs = cs; m_cs->Enter(); }
      virtual ~CAuCriticalSectionEx() { m_cs->Leave(); }
    };

    class CAuInterlockedInt
    {
    private:
      LONG m_nValue;
    public:
      CAuInterlockedInt() { ::InterlockedExchange(&m_nValue, 0); }
      CAuInterlockedInt(LONG nValue) { Exchange(nValue); }

      LONG Exchange(LONG nValue) { return ::InterlockedExchange(&m_nValue, nValue); }
      LONG Get() const { return m_nValue; }

      LONG operator++(int n) { return ::InterlockedIncrement(&m_nValue); }
      LONG operator--(int n) { return ::InterlockedDecrement(&m_nValue); }
      LONG operator+=(LONG n) { return ::InterlockedExchangeAdd(&m_nValue, n); }
      LONG operator-=(LONG n) { return ::InterlockedExchangeAdd(&m_nValue, -n); }

      operator LONG() { return m_nValue; }

      LONG operator=(LONG nValue) { return Exchange(nValue); }
    };
  };
};
