
#pragma once

#include "NokomoSystem.h"

namespace Nokomo
{
  namespace StlExtensions
  {
    template<class T>
    struct ptr_less : binary_function<T*, T*, bool>
    {
      bool operator()(const T* x, const T* y) const
      {
        return *x < *y;
      }
    };
    template<class T>
    struct ptr_greater : binary_function<T*, T*, bool>
    {
      bool operator()(const T* x, const T* y) const
      {
        return *x > *y;
      }
    };
  };
};
