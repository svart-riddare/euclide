#include "errors.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

void abort(error_t error)
{
	throw error;
}

/* -------------------------------------------------------------------------- */

#ifndef NDEBUG

void assert(bool expression)
{
	if (!expression)
		abort(InternalLogicError);
}

#endif

/* -------------------------------------------------------------------------- */

}

/* -------------------------------------------------------------------------- */

namespace boost
{

void assertion_failed(char const *, char const *, char const *, long)
{
#ifndef NDEBUG
	euclide::assert(false);
#endif
}

}

/* -------------------------------------------------------------------------- */
