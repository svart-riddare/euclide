#include "errors.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

void abort(error_t error)
{
	throw error;
}

/* -------------------------------------------------------------------------- */

void assert(bool expression)
{
#ifndef NDEBUG
	if (!expression)
		abort(InternalLogicError);
#else
	expression;
#endif
}

/* -------------------------------------------------------------------------- */

}

/* -------------------------------------------------------------------------- */

namespace boost
{

void assertion_failed(char const *, char const *, char const *, long)
{
	euclide::assert(false);
}

}

/* -------------------------------------------------------------------------- */
