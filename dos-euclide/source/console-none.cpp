#include "console-none.h"

/* -------------------------------------------------------------------------- */

NoConsole::NoConsole(const Strings& strings)
	: Console(strings)
{
#ifdef EUCLIDE_WINDOWS
	SetConsoleOutputCP(1252);
#else
	setlocale(LC_ALL, "");
#endif

	/* -- Use stdout -- */

	open(nullptr, true);

	/* -- Done -- */

	m_valid = true;
}

/* -------------------------------------------------------------------------- */

NoConsole::~NoConsole()
{
}

/* -------------------------------------------------------------------------- */

bool NoConsole::wait()
{
	/* -- Prevent stdin buffering -- */

#ifndef EUCLIDE_WINDOWS
	termios attributes, xattributes;
	tcgetattr(STDIN_FILENO, &attributes);
	xattributes = attributes;
	xattributes.c_lflag &= ~ICANON & ~ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &xattributes);
	setbuf(stdin, nullptr);
#endif

	/* -- Clear all input -- */

#ifndef EUCLIDE_WINDOWS
	int waiting = 0;
	while (!ioctl(STDIN_FILENO, FIONREAD, &waiting) && waiting)
		getchar();
#else
	while (_kbhit())
		_getch();
#endif

	/* -- Show "press any key" message -- */

	fprintf(stdout, "%ls\n", m_strings[Strings::PressAnyKey]);
	fflush(stdout);

	/* -- Wait for key press -- */

#ifndef EUCLIDE_WINDOWS
	while (!ioctl(STDIN_FILENO, FIONREAD, &waiting) && !waiting)
		usleep(10000);
#else
	while (!_kbhit())
		Sleep(10);
#endif

	/* -- Check ESCAPE key -- */

#ifndef EUCLIDE_WINDOWS
	if (getchar() == 0x1B)
		m_abort = true;
#else
	if (_getch() == VK_ESCAPE)
		m_abort = true;
#endif

	/* -- Restore stdin buffering -- */

#ifndef EUCLIDE_WINDOWS
	tcsetattr(STDIN_FILENO, TCSANOW, &attributes);
#endif

	/* -- Done -- */

	return !m_abort;
}

/* -------------------------------------------------------------------------- */
