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
	if (!expression)
		abort(InternalLogicError);
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
