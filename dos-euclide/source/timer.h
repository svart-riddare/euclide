#ifndef __TIMER_H
#define __TIMER_H

#include "includes.h"

/* -------------------------------------------------------------------------- */

class Timer
{
	public :
		Timer();

		operator double() const;
		operator LPCTSTR() const;
		
	private :
		mutable TCHAR buffer[32];
		clock_t start;
};

/* -------------------------------------------------------------------------- */

#endif
