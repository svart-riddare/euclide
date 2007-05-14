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

template <class T>
void maximize(T *variables, const T *values, int size)
{
	for (int k = 0; k < size; k++)
		variables[k] = std::max(variables[k], values[k]);
}

template <class T>
void minimize(T *variables, const T *values, int size)
{
	for (int k = 0; k < size; k++)
		variables[k] = std::min(variables[k], values[k]);
}

/* -------------------------------------------------------------------------- */

}

#endif
