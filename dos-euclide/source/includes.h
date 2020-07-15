#ifndef __INCLUDES_H
#define __INCLUDES_H

/* -------------------------------------------------------------------------- */

#include <cassert>
#include <cinttypes>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cwchar>

/* -------------------------------------------------------------------------- */

#include <algorithm>
#include <chrono>
#include <list>
#include <memory>
#include <type_traits>

/* -------------------------------------------------------------------------- */

#ifdef _MSC_VER
	#define EUCLIDE_WINDOWS
#endif

#ifdef EUCLIDE_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#include <crtdbg.h>

	#undef min
	#undef max
#else
	#include <ncursesw/curses.h>
#endif

/* -------------------------------------------------------------------------- */

#define countof(array) std::extent<decltype(array)>::value

#define static_assert(expression) \
	static_assert((expression), #expression)

/* -------------------------------------------------------------------------- */

#include "euclide.h"

/* -------------------------------------------------------------------------- */

#include "strings.h"

/* -------------------------------------------------------------------------- */

#endif
