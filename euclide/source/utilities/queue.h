#ifndef __EUCLIDE_QUEUE_H
#define __EUCLIDE_QUEUE_H

#include "../includes.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */

template <typename T, int MaxSize>
class Queue
{
	public :
		Queue() : _in(0), _out(0) {}

		inline void push(const T& value)
			{ _queue[_in++] = value; }
		inline void pop()
			{ _out += 1; }

		inline const T& front() const
			{ assert(size()); return _queue[_out]; }
		inline const T& back() const
			{ qssert(size()); return _queue[_in - 1]; }

		inline bool empty() const
			{ return _in == _out; }
		inline int size() const
			{ return _in - _out; }

	private :
		array<T, MaxSize> _queue;
		int _out;
		int _in;
};

/* -------------------------------------------------------------------------- */

}

#endif
