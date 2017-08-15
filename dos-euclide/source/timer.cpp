#include "timer.h"

/* -------------------------------------------------------------------------- */

Timer::Timer()
{
	_start = now();
}

/* -------------------------------------------------------------------------- */

const wchar_t *Timer::elapsed() const
{
	const auto elapsed = duration_cast<milliseconds>(now() - _start);

	const int thousandths = elapsed.count() % 1000;
	const int seconds = (elapsed.count() / 1000) % 60;
	const int minutes = (elapsed.count() / 60000) % 60;
	const int hours = elapsed.count() / 3600000;

	if (hours > 0)
		swprintf(_elapsed, countof(_elapsed), L"%u:%02d:%02d.%03d", hours, minutes, seconds, thousandths);
	else 
	if (minutes > 0)
		swprintf(_elapsed, countof(_elapsed), L"%d:%02d.%03d", minutes, seconds, thousandths);
	else
		swprintf(_elapsed, countof(_elapsed), L"%d.%03d", seconds, thousandths);

	return _elapsed;
}

/* -------------------------------------------------------------------------- */

steady_clock::time_point Timer::now()
{
#if defined(EUCLIDE_WINDOWS) && (_MSC_VER < 1900)
	static LARGE_INTEGER frequency = (QueryPerformanceFrequency(&frequency), frequency);

	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	milliseconds ms(1000 * now.QuadPart / frequency.QuadPart);
	return steady_clock::time_point::time_point(duration_cast<steady_clock::duration>(ms));

#else
	return steady_clock::now();
#endif
}

/* -------------------------------------------------------------------------- */
