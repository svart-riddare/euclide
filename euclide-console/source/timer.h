#ifndef __TIMER_H
#define __TIMER_H

#include "includes.h"

/* -------------------------------------------------------------------------- */

class Timer
{
	public:
		Timer();

		void stop();

		std::chrono::seconds seconds() const;
		const wchar_t *elapsed() const;

	protected:
		std::chrono::steady_clock::time_point now() const;

	private:
		std::chrono::steady_clock::time_point m_start;    /**< Starting time. */
		std::chrono::steady_clock::time_point m_stop;     /**< Stop time, if timer was stopped. */
		mutable wchar_t m_elapsed[64];                    /**< Temporary buffer used to store elapsed time as string. */
};

/* -------------------------------------------------------------------------- */

#endif
