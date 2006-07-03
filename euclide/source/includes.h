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

#include <boost/array.hpp>
#include <boost/logic/tribool.hpp>

using boost::array;
using boost::logic::tribool;

/* -------------------------------------------------------------------------- */

#include "euclide.h"

/* -------------------------------------------------------------------------- */

#include "enumerations.h"
#include "constants.h"
#include "tables.h"

/* -------------------------------------------------------------------------- */

#ifndef __EUCLIDE_PROBLEM_H
	#include "problem.h"
#ifndef __EUCLIDE_FATE_H
	#include "fate.h"
#endif
#endif

/* -------------------------------------------------------------------------- */

#endif
