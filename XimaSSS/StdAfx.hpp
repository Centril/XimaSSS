#pragma once

// Standard C++ Includes
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cmath>
#include <fstream>
#include <cassert>
using namespace std;

// Boost C++ Includes
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/regex.hpp>
using namespace boost;

// Win32 Includes
#include <windows.h>
#include <windowsx.h>

// GFX Includes
#include <gdiplus.h>
using namespace Gdiplus;

// Global Variables

// Global Messages
#include "WinMessages.hpp"

// Own Includes
#include "resource.h"
#include "globals.func.hpp"
#include "Win/Control.hpp"
using namespace Win;


// Testing Purposes
#include "Win/testing.hpp"