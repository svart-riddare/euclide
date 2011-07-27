#ifndef __EUCLIDE_TAG_H
#define __EUCLIDE_TAG_H

#include "includes.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

template <class T>
class taggable
{
	public :
		taggable() {};
		taggable(const T& tag)
			{ _tag = tag; }

	public :
		void tag(const T& tag) const
			{ _tag = tag; }		
		const T& tag(void) const
			{ return _tag; }

	private :
		mutable T _tag;
};

/* -------------------------------------------------------------------------- */

}

#endif