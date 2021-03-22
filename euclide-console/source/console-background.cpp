#include "console-background.h"

/* -------------------------------------------------------------------------- */

static int id = 0;

/* -------------------------------------------------------------------------- */

BackgroundConsole::BackgroundConsole(Console& console, bool active)
	: Console(console.strings()), m_console(console), m_active(active), m_id(++id)
{
	clear();

	/* -- Done -- */

	m_valid = !!console;
}

/* -------------------------------------------------------------------------- */

BackgroundConsole::~BackgroundConsole()
{
}

/* -------------------------------------------------------------------------- */

void BackgroundConsole::reset(std::chrono::seconds timeout, const Timer& timer)
{
	std::lock_guard<std::mutex> locker(m_lock);

	if (m_active)
		return m_console.reset(timeout, timer);

	Console::reset(timeout, timer);
}

/* -------------------------------------------------------------------------- */

void BackgroundConsole::clear()
{
	m_displayCopyright = false;
	m_displayOptions = false;
	m_displayMessage = false;
	m_displayProblem = false;
	m_displayDeductions = false;
	m_displayThinking = false;
	m_displaySolution = false;
	m_solutions.clear();
	m_done = false;
}

/* -------------------------------------------------------------------------- */

void BackgroundConsole::done(EUCLIDE_Status status)
{
	std::lock_guard<std::mutex> locker(m_lock);

	if (m_active)
		m_console.done(status);

	m_done = true;
	m_status = status;

	m_condition.notify_all();
}

/* -------------------------------------------------------------------------- */

bool BackgroundConsole::wait()
{
	assert(m_active);
	return m_console.wait();
}

/* -------------------------------------------------------------------------- */

void BackgroundConsole::displayCopyright(const wchar_t *copyright) const
{
	std::lock_guard<std::mutex> locker(m_lock);

	if (m_active)
		return m_console.displayCopyright(copyright);

	m_displayCopyright = true;
	m_copyright = copyright;
}

/* -------------------------------------------------------------------------- */

void BackgroundConsole::displayOptions(const EUCLIDE_Options& options) const
{
	std::lock_guard<std::mutex> locker(m_lock);

	if (m_active)
		return m_console.displayOptions(options);

	m_displayOptions = true;
	m_options = options;
}

/* -------------------------------------------------------------------------- */

void BackgroundConsole::displayMessage(EUCLIDE_Message message) const
{
	std::lock_guard<std::mutex> locker(m_lock);

	if (m_active)
		return m_console.displayMessage(message);

	m_displayMessage = true;
	m_message = message;
}

/* -------------------------------------------------------------------------- */

void BackgroundConsole::displayProblem(const EUCLIDE_Problem& problem) const
{
	std::lock_guard<std::mutex> locker(m_lock);

	if (m_active)
		return m_console.displayProblem(problem);

	m_displayProblem = true;
	m_problem = problem;
}

/* -------------------------------------------------------------------------- */

void BackgroundConsole::displayDeductions(const EUCLIDE_Deductions& deductions) const
{
	std::lock_guard<std::mutex> locker(m_lock);

	if (m_active)
		return m_console.displayDeductions(deductions);

	m_displayDeductions = true;
	m_deductions = deductions;
}

/* -------------------------------------------------------------------------- */

void BackgroundConsole::displayThinking(const EUCLIDE_Thinking& thinking) const
{
	std::lock_guard<std::mutex> locker(m_lock);

	if (m_active)
		return m_console.displayThinking(thinking);

	m_displayThinking = true;
	m_thinking = thinking;
}

/* -------------------------------------------------------------------------- */

void BackgroundConsole::displaySolution(const EUCLIDE_Solution& solution) const
{
	std::lock_guard<std::mutex> locker(m_lock);

	if (m_active)
		return m_console.displaySolution(solution);

	m_displaySolution = true;
	m_solutions.push_back(solution);
}

/* -------------------------------------------------------------------------- */

bool BackgroundConsole::abort() const
{
	std::lock_guard<std::mutex> locker(m_lock);

	if (m_active)
		if (m_console.abort())
			return true;

	return !m_console || Console::abort();
}

/* -------------------------------------------------------------------------- */

bool BackgroundConsole::foreground(bool wait)
{
	std::unique_lock<std::mutex> locker(m_lock);

	/* -- Forward callbacks to actual console -- */

	if (!m_active)
	{
		m_console.reset(std::chrono::seconds(0), timer());

		if (m_displayCopyright)
			m_console.displayCopyright(m_copyright.c_str());

		if (m_displayOptions)
			m_console.displayOptions(m_options);

		if (m_displayProblem)
			m_console.displayProblem(m_problem);

		if (m_displayMessage)
			m_console.displayMessage(m_message);

		if (m_displayDeductions)
			m_console.displayDeductions(m_deductions);

		if (m_displayThinking)
			m_console.displayThinking(m_thinking);

		if (m_displaySolution)
			for (const EUCLIDE_Solution& solution : m_solutions)
				m_console.displaySolution(solution);

		if (m_done)
			m_console.done(m_status);

		m_active = true;
	}

	/* -- Wait for done -- */

	m_condition.wait(locker, [&]() { return m_done; });

	/* -- Wait for user confirmation -- */

	return wait ? m_console.wait() : true;
}

/* -------------------------------------------------------------------------- */
