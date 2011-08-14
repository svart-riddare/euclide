#ifndef __EUCLIDE_ERRORS_H
#define __EUCLIDE_ERRORS_H

#include <cassert>
#ifndef NDEBUG
	#undef assert
#endif

#include "../../interface/euclide.h"

namespace euclide 
{

/* -------------------------------------------------------------------------- */

typedef enum
{
	NoError,
	NoSolution,

	IncorrectInputError,
	InternalLogicError,
	OutOfMemoryError,

	NumErrors

} error_t;

/* -------------------------------------------------------------------------- */

void abort(error_t error);

#ifndef NDEBUG
	void assert(bool expression);
#endif

/* -------------------------------------------------------------------------- */

static inline
EUCLIDE_Status getStatus(error_t error)
{
	if (error == NoError)
		return EUCLIDE_STATUS_OK;

	if (error == IncorrectInputError)
		return EUCLIDE_STATUS_INCORRECT_INPUT_ERROR;

	if (error == InternalLogicError)
		return EUCLIDE_STATUS_INTERNAL_LOGIC_ERROR;

	if (error == OutOfMemoryError)
		return EUCLIDE_STATUS_OUT_OF_MEMORY_ERROR;
	
	if (error == NoSolution)
		return EUCLIDE_STATUS_KO;

	return EUCLIDE_STATUS_UNKNOWN_ERROR;
}

/* -------------------------------------------------------------------------- */

}

#endif
