#ifndef __EUCLIDE_QUEUE_H
#define __EUCLIDE_QUEUE_H

#include "../includes.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */

template <typename T, int MaxSize>
class Queue
{
	public:
		Queue() : m_out(0), m_in(0) {}

		inline void push(const T& value)
			{ assert(m_in < MaxSize); m_queue[m_in++] = value; }
		inline void pass(const T& value, int skip = 0)
			{ assert(m_in < MaxSize); assert(skip <= size()); m_in++; std::copy_backward(&m_queue[m_out + skip], &m_queue[m_in - 1], &m_queue[m_in]); m_queue[m_out + skip] = value; }
		inline void pop()
			{ assert(m_out < m_in); m_out += 1; }

		inline const T& front() const
			{ assert(size()); return m_queue[m_out]; }
		inline const T& back() const
			{ assert(size()); return m_queue[m_in - 1]; }

		inline bool empty() const
			{ return m_in == m_out; }
		inline bool full() const
			{ return m_in >= MaxSize; }
		inline int size() const
			{ return m_in - m_out; }

	private:
		array<T, MaxSize> m_queue;    /**< Queued elements. No more than MaxSize elements can be pushed in queue, ever. */
		int m_out;                    /**< Index in above array, of oldest element. */
		int m_in;                     /**< Index, in above array, or newest element. */
};

/* -------------------------------------------------------------------------- */

}

#endif
