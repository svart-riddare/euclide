#ifndef __TIMER_H
#define __TIMER_H

#include "includes.h"

/* -------------------------------------------------------------------------- */

class Timer
{
	public:
		Timer();

		std::chrono::seconds seconds() const;
		const wchar_t *elapsed() const;

	protected:
		static std::chrono::steady_clock::time_point now();

	private:
		std::chrono::steady_clock::time_point m_start;    /**< Starting time. */
		mutable wchar_t m_elapsed[64];                    /**< Temporary buffer used to store elapsed time as string. */
};

/* -------------------------------------------------------------------------- */

#endif
