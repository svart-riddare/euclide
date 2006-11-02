#ifndef __EUCLIDE_VECTOR_H
#define __EUCLIDE_VECTOR_H

#include "includes.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

template <class T>
class vector_ptr_const_iterator : public vector<T *>::const_iterator
{
	public :
		vector_ptr_const_iterator(const typename vector<T *>::const_iterator& iterator) : vector<T *>::const_iterator(iterator) {}

	public :
		const T *operator->() const
			{ return NULL; }
};

template <class T>
class vector_ptr_iterator : public vector<T *>::iterator
{
	public :
		vector_ptr_iterator(const typename vector<T *>::iterator& iterator) : vector<T *>::iterator(iterator) {}

	public :
		T *operator->() const
			{ return **this; }
};

/* -------------------------------------------------------------------------- */

template <class T>
class vector_ptr : public vector<T *>
{
	public :
		typedef vector_ptr_const_iterator<T> const_iterator;
		typedef vector_ptr_iterator<T> iterator;

	public :
		vector_ptr() : vector<T *>() {}
		explicit vector_ptr(typename vector<T *>::size_type count) : vector<T *>(count) {}
		explicit vector_ptr(typename vector<T *>::size_type count, const T *& value) : vector<T *>(count, value) {}
};

/* -------------------------------------------------------------------------- */

}

#endif
