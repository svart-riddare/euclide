#ifndef __EUCLIDE_INCLUDE_H
#define __EUCLIDE_INCLUDE_H

/* -------------------------------------------------------------------------- */

#include <cstdlib>

/* -------------------------------------------------------------------------- */

#include <algorithm>
#include <functional>
#include <new>
#include <list>
#include <queue>
#include <vector>

using std::list;
using std::queue;
using std::vector;

/* -------------------------------------------------------------------------- */

#define BOOST_ENABLE_ASSERT_HANDLER

#include <boost/array.hpp>
#include <boost/logic/tribool.hpp>

using boost::array;
using boost::logic::tribool;
using boost::logic::indeterminate;

/* -------------------------------------------------------------------------- */

#include "euclide.h"

/* -------------------------------------------------------------------------- */

#include "enumerations.h"
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

#include "castling.h"
#include "problem.h"
#include "board.h"

/* -------------------------------------------------------------------------- */

#endif
