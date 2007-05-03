#ifndef __EUCLIDE_POINTER_H
#define __EUCLIDE_POINTER_H

#include "includes.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

template<typename T>
class Pointer
{
	public :
		Pointer(T *pointer) : pointer(pointer) {};

		operator const T *() const
			{ return pointer; }

		operator T *()
			{ return pointer; }

	private :
		T *pointer;
};

/* -------------------------------------------------------------------------- */

}

#endif
