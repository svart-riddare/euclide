#ifndef __EUCLIDE_INCLUDES_H
#define __EUCLIDE_INCLUDES_H

/* -------------------------------------------------------------------------- */

#if defined(_MSC_VER) && defined(_M_X64)
	#define EUCLIDE_WIN_IMPLEMENTATION
#endif

#if defined(__GNUC__) && defined(__x86_64__)
	#define EUCLIDE_LINUX_IMPLEMENTATION
#endif

/* -------------------------------------------------------------------------- */

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

/* -------------------------------------------------------------------------- */

#include <array>
#include <algorithm>
#include <bitset>
#include <functional>
#include <iterator>
#include <map>
#include <new>
#include <numeric>
#include <queue>
#include <vector>

using std::array;
using std::tuple;

/* -------------------------------------------------------------------------- */

#ifdef EUCLIDE_WIN_IMPLEMENTATION
	#include <intrin.h>
#endif

/* -------------------------------------------------------------------------- */

#define countof(array) std::extent<decltype(array)>::value

#define static_assert(expression) \
	static_assert((expression), #expression)

#if defined(_MSC_VER) && (_MSC_VER < 1900)
	#define constexpr inline
#endif

/* -------------------------------------------------------------------------- */

#include "utilities/boost/tribool.hpp"
BOOST_TRIBOOL_THIRD_STATE(unknown);
using namespace boost::logic;

static inline bool is(tribool value) { return bool(value); }
static inline bool maybe(tribool value) { return bool(value) || unknown(value); }

#include "utilities/algorithm.h"
#include "utilities/bitset.h"
#include "utilities/iterator.h"
#include "utilities/queue.h"
#include "utilities/matrix.h"

/* -------------------------------------------------------------------------- */

#include "../interface/euclide.h"
#include "definitions.h"

/* -------------------------------------------------------------------------- */

namespace Euclide
{
	extern const wchar_t *Copyright;

	static const int Infinity = 7777;
}

/* -------------------------------------------------------------------------- */

#endif
