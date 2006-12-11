#ifndef __EUCLIDE_INCLUDE_H
#define __EUCLIDE_INCLUDE_H

/* -------------------------------------------------------------------------- */

#include <cstdarg>
#include <cstdlib>

/* -------------------------------------------------------------------------- */

#include <algorithm>
#include <bitset>
#include <functional>
#include <new>
#include <numeric>
#include <list>
#include <queue>
#include <vector>
#include <utility>

using std::bitset;
using std::list;
using std::queue;
using std::priority_queue;
using std::vector;
using std::pair;

/* -------------------------------------------------------------------------- */

#define BOOST_ENABLE_ASSERT_HANDLER

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/logic/tribool.hpp>

using boost::ref;
using boost::cref;

using boost::logic::tribool;
using boost::logic::indeterminate;

/* -------------------------------------------------------------------------- */

#include "euclide.h"
#include "enumerations.h"

/* -------------------------------------------------------------------------- */

#include "utilities/array.h"
#include "utilities/algorithm.h"
#include "utilities/bitset.h"
#include "utilities/matrix.h"
#include "utilities/predicates.h"
#include "utilities/vector.h"

/* -------------------------------------------------------------------------- */

#include "constants.h"
#include "tables.h"

using euclide::constants::infinity;

/* -------------------------------------------------------------------------- */

namespace euclide
{
	class Castling;
	class Problem;
	class Board;
}

/* -------------------------------------------------------------------------- */

#include "tables.h"
#include "problem.h"
#include "board.h"

/* -------------------------------------------------------------------------- */

#endif
