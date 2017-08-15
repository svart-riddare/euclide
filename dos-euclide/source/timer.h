#ifndef __TIMER_H
#define __TIMER_H

#include "includes.h"

/* -------------------------------------------------------------------------- */

class Timer
{
	public :
		Timer();

		const wchar_t *elapsed() const;

	protected :
		static steady_clock::time_point now();

	private :
		steady_clock::time_point _start;    /**< Starting time. */
		mutable wchar_t _elapsed[64];       /**< Temporary buffer used to store elasped time as string. */
};

/* -------------------------------------------------------------------------- */

#endif
