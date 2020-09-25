#ifndef __EUCLIDE_MATRIX_H
#define __EUCLIDE_MATRIX_H

#include "../includes.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */

template<class T, std::size_t M, std::size_t N>
class matrix : public array<array<T, N>, M>
{
	public:
		matrix() {};

		matrix(const T& value)
			{ fill(value); }

		void fill(const T& value)
			{ for (std::size_t n = 0; n < N; n++)  (*this)[n].fill(value); }

	public:
		typedef T value_type;
		typedef T& reference;
		typedef const T& const_reference;
};

/* -------------------------------------------------------------------------- */

}

#endif
