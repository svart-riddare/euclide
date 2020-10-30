#ifndef __EUCLIDE_QUEUE_H
#define __EUCLIDE_QUEUE_H

#include "../includes.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */

template <typename T, size_t MaxSize>
class Queue
{
	public:
		constexpr Queue() : m_out(0), m_in(0) {}

		inline void clear()
			{ m_out = m_in = 0; }

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

		inline constexpr bool empty() const
			{ return m_in == m_out; }
		inline constexpr bool full() const
			{ return m_in >= MaxSize; }
		inline constexpr size_t size() const
			{ return m_in - m_out; }

		inline void sort()
			{ std::sort(m_queue.data() + m_out, m_queue.data() + m_in); }
		template<typename Comparator>
		inline void sort(const Comparator& comparator)
			{ std::sort(m_queue.data() + m_out, m_queue.data() + m_in, comparator); }

	private:
		array<T, MaxSize> m_queue;    /**< Queued elements. No more than MaxSize elements can be pushed in queue, ever. */
		size_t m_out;                 /**< Index in above array, of oldest element. */
		size_t m_in;                  /**< Index, in above array, or newest element. */
};

/* -------------------------------------------------------------------------- */

}

#endif
