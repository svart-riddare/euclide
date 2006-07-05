#ifndef __EUCLIDE_INCLUDE_H
#define __EUCLIDE_INCLUDE_H

/* -------------------------------------------------------------------------- */

#include <cstdlib>

/* -------------------------------------------------------------------------- */

#include <algorithm>
#include <functional>
#include <new>
#include <vector>

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

/* -------------------------------------------------------------------------- */

#include "problem.h"

/* -------------------------------------------------------------------------- */

#endif
