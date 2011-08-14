#ifndef __EUCLIDE_PREDICATES_H
#define __EUCLIDE_PREDICATES_H

#include "../includes.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

template <typename T, int N>
class _greater
{
	public :
		_greater(const array<T, N>& values) : values(values.data()) {};
		_greater(const T values[N]) : values(values) {};

		bool operator()(T m, T n) const
		{
			assert((m >= 0) && (m < N));
			assert((n >= 0) && (n < N));

			return values[m] > values[n];
		}

	private :
		const T *values;
};

template <typename T, int N>
class _smaller
{
	public :
		_smaller(const array<T, N>& values) : values(values.data()) {};
		_smaller(const T values[N]) : values(values) {};

		bool operator()(T m, T n) const
		{
			assert((m >= 0) && (m < N));
			assert((n >= 0) && (n < N));

			return values[m] < values[n];
		}

	private :
		const T *values;
};

/* -------------------------------------------------------------------------- */

}

#endif
