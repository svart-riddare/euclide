#ifndef __EUCLIDE_ARRAY_H
#define __EUCLIDE_ARRAY_H

#include "../includes.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

template<class T, std::size_t N>
class array : public std::array<T, N>
{
	public :
		array() {}
	
		array(const T& value)
			{ std::array<T, N>::assign(value); }
};

/* -------------------------------------------------------------------------- */

}

#endif
