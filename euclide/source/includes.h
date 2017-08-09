#ifndef __EUCLIDE_INCLUDES_H
#define __EUCLIDE_INCLUDES_H

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
#include <new>
#include <numeric>
#include <queue>
#include <vector>

using std::array;

/* -------------------------------------------------------------------------- */

#define countof(array) std::extent<decltype(array)>::value

#define static_assert(expression) \
	static_assert((expression), #expression)

/* -------------------------------------------------------------------------- */

#include "utilities/boost/tribool.hpp"
BOOST_TRIBOOL_THIRD_STATE(unknown);
using namespace boost::logic;

#include "utilities/algorithm.h"
#include "utilities/bitset.h"
#include "utilities/iterator.h"

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
