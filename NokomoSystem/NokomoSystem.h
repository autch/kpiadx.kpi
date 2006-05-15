
#pragma once

/************************************************************************
 * NokomoSystem.h -- NokomoSystem basic services / system services
 *
 * Copyright(c) 2004, Autch.net.  All rights reserved.
 *
 ************************************************************************/

// vital Win32 SDK
#include <windows.h>

// Standard C++ libraries
//   STL
#include <set>
#include <string>
#include <stack>
#include <vector>
#include <algorithm>
#include <functional>
//   C++ libs.
#include <iostream>

namespace Nokomo
{
  // NokomoSystem global declarations
  using namespace std;
  typedef unsigned __int64 QWORD, *PQWORD;
};

//// NokomoSystem basic / system services ////////////////////////////////

// exceptions
#include "Exceptions.h"

// basic arithmetic functions / classes
#include "BasicArithmetics.h"

// synchronization objects
#include "SyncObjects.h"

// unit conversion objects
#include "UnitConversions.h"

// add-on for Standard Template Libraries
#include "StlExtensions.h"

// streamer interface
#include "Stream.h"

// file drivers
#include "FileDrivers.h"

//////////////////////////////////////////////////////////////////////////
