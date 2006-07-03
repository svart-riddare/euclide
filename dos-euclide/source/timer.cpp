#include "timer.h"

/* -------------------------------------------------------------------------- */

Timer::Timer()
{
	start = clock();
}

/* -------------------------------------------------------------------------- */

Timer::operator double() const
{
	return (double)(clock() - start) / CLOCKS_PER_SEC;
}

/* -------------------------------------------------------------------------- */

Timer::operator LPCTSTR() const
{
	double elapsed = (double)(*this);

	unsigned int hundreths = (unsigned int)floor(100.0 * modf(elapsed, &elapsed));
	unsigned int seconds = (unsigned int)elapsed % 60;
	unsigned int minutes = (unsigned int)floor(elapsed / 60.0) % 60;
	unsigned int hours = (unsigned int)floor(elapsed / 3600.0);

	if (hours > 0)
		_sntprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), _T("%u:%02u:%02u.%02u"), hours, minutes, seconds, hundreths);
	else
	if (minutes > 0)
		_sntprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), _T("%u:%02u.%02u"), minutes, seconds, hundreths);
	else
		_sntprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), _T("%u.%02u"), seconds, hundreths);

	return buffer;
}

/* -------------------------------------------------------------------------- */
