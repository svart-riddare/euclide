#ifndef __TIMER_H
#define __TIMER_H

#include "includes.h"

/* -------------------------------------------------------------------------- */

class Timer
{
	public :
		Timer();

		operator double() const;
		operator const wchar_t *() const;
		
	private :
		mutable wchar_t buffer[64];
		clock_t start;
};

/* -------------------------------------------------------------------------- */

#endif
