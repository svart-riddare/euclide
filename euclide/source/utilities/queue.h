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
		Queue() : _out(0), _in(0) {}

		inline void push(const T& value)
			{ assert(_in < MaxSize); _queue[_in++] = value; }
		inline void pass(const T& value, int skip = 0)
			{ assert(_in < MaxSize); assert(skip <= size()); _in++; std::copy_backward(&_queue[_out + skip], &_queue[_in - 1], &_queue[_in]); _queue[_out + skip] = value; }
		inline void pop()
			{ assert(_out < _in); _out += 1; }

		inline const T& front() const
			{ assert(size()); return _queue[_out]; }
		inline const T& back() const
			{ assert(size()); return _queue[_in - 1]; }

		inline bool empty() const
			{ return _in == _out; }
		inline bool full() const
			{ return _in >= MaxSize; }
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
