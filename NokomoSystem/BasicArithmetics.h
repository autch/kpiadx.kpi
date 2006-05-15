#pragma once

#include "NokomoSystem.h"

namespace Nokomo
{
  namespace BasicArith
  {
    // absolute difference
    template<class T, class tmp>
    inline T DAbs(const T x, const T y)
    {
      tmp d = x - y;
      return (d >= 0) ? (T)d: (T)-d;
    }
  };
};
