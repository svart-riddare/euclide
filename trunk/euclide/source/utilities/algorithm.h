#ifndef __EUCLIDE_ALGORITHM_H
#define __EUCLIDE_ALGORITHM_H

#include "includes.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

template <class T>
T& maximize(T& variable, const T& value)
{
	return variable = std::max(variable, value);
}

template <class T>
T& minimize(T& variable, const T& value)
{
	return variable = std::min(variable, value);
}

/* -------------------------------------------------------------------------- */

}

#endif
