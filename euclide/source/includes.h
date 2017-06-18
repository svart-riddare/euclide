#ifndef __EUCLIDE_INCLUDE_H
#define __EUCLIDE_INCLUDE_H

/* -------------------------------------------------------------------------- */

#include <climits>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <cstring>

/* -------------------------------------------------------------------------- */

#include <array>
#include <algorithm>
#include <bitset>
#include <functional>
#include <new>
#include <numeric>
#include <list>
#include <queue>
#include <tuple>
#include <vector>
#include <utility>

using std::array;
using std::bitset;
using std::list;
using std::queue;
using std::pair;
using std::priority_queue;
using std::tuple;
using std::vector;

using std::get;
using std::make_pair;
using std::make_tuple;

/* -------------------------------------------------------------------------- */

#include "../interface/euclide.h"
#include "enumerations/enumerations.h"

/* -------------------------------------------------------------------------- */

#include "utilities/array.h"
#include "utilities/algorithm.h"
#include "utilities/bitset.h"
#include "utilities/matrix.h"
#include "utilities/pointer.h"
#include "utilities/predicates.h"
#include "utilities/vector.h"
#include "utilities/tag.h"

#include "utilities/boost/tribool.hpp"

using boost::logic::tribool;
using boost::logic::indeterminate;

/* -------------------------------------------------------------------------- */

#include "constants/constants.h"
#include "tables/tables.h"

using euclide::constants::infinity;

/* -------------------------------------------------------------------------- */

namespace euclide
{
	class Castling;
	class Problem;
	class Board;
}

/* -------------------------------------------------------------------------- */

#include "problem.h"
#include "board.h"
#include "debug.h"

/* -------------------------------------------------------------------------- */

#endif
