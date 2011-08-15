#ifndef __INCLUDES_H
#define __INCLUDES_H

/* -------------------------------------------------------------------------- */

#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cwchar>

/* -------------------------------------------------------------------------- */

#include <algorithm>
#include <list>
#include <memory>

using std::auto_ptr;
using std::list;

/* -------------------------------------------------------------------------- */

#ifndef EUCLIDE_LINUX
	#include <windows.h>
	#include <crtdbg.h>

	#undef min
	#undef max
#else
	#include <ncursesw/curses.h>
#endif

/* -------------------------------------------------------------------------- */

#include "euclide.h"

/* -------------------------------------------------------------------------- */

#include "strings.h"

/* -------------------------------------------------------------------------- */

#endif
