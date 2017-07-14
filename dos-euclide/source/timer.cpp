#include "timer.h"

/* -------------------------------------------------------------------------- */

Timer::Timer()
{
	_start = steady_clock::now();
}

/* -------------------------------------------------------------------------- */

const wchar_t *Timer::elapsed() const
{
	const auto elapsed = duration_cast<milliseconds>(steady_clock::now() - _start);

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
