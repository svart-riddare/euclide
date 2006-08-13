#ifndef __EUCLIDE_PREDICATES_H
#define __EUCLIDE_PREDICATES_H

#include "includes.h"

/* -------------------------------------------------------------------------- */

template <typename T, int N>
class _greater
{
	public :
		_greater(const array<T, N>& values) : values(&values) {};

		bool operator()(T m, T n) const
		{
			return (*values)[m] > (*values)[n];
		}

	private :
		const array<T, N> *values;
};

/* -------------------------------------------------------------------------- */

#endif
