#include "timer.h"

/* -------------------------------------------------------------------------- */

Timer::Timer()
{
	m_start = now();
}

/* -------------------------------------------------------------------------- */

std::chrono::seconds Timer::seconds() const
{
	return std::chrono::duration_cast<std::chrono::seconds>(now() - m_start);
}

/* -------------------------------------------------------------------------- */

const wchar_t *Timer::elapsed() const
{
	const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now() - m_start);

	const int thousandths = elapsed.count() % 1000;
	const int seconds = (elapsed.count() / 1000) % 60;
	const int minutes = (elapsed.count() / 60000) % 60;
	const int hours = elapsed.count() / 3600000;

	if (hours > 0)
		swprintf(m_elapsed, countof(m_elapsed), L"%u:%02d:%02d.%03d", hours, minutes, seconds, thousandths);
	else
	if (minutes > 0)
		swprintf(m_elapsed, countof(m_elapsed), L"%d:%02d.%03d", minutes, seconds, thousandths);
	else
		swprintf(m_elapsed, countof(m_elapsed), L"%d.%03d", seconds, thousandths);

	return m_elapsed;
}

/* -------------------------------------------------------------------------- */

std::chrono::steady_clock::time_point Timer::now()
{
#if defined(EUCLIDE_WINDOWS) && (_MSC_VER < 1900)
	static LARGE_INTEGER frequency = (QueryPerformanceFrequency(&frequency), frequency);

	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	std::chrono::milliseconds ms(1000 * now.QuadPart / frequency.QuadPart);
	return std::chrono::steady_clock::time_point::time_point(std::chrono::duration_cast<std::chrono::steady_clock::duration>(ms));

#else
	return std::chrono::steady_clock::now();
#endif
}

/* -------------------------------------------------------------------------- */
