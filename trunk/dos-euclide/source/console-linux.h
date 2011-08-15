#ifndef __CONSOLE_LINUX_H
#define __CONSOLE_LINUX_H

#include "includes.h"
#include "console.h"

/* -------------------------------------------------------------------------- */

class LinuxConsole : public Console
{
	public :
		LinuxConsole();
		virtual ~LinuxConsole();

		virtual void clear();
		virtual bool wait();

		virtual void displayProblem(const EUCLIDE_Problem *problem);
		virtual void displayDeductions(const EUCLIDE_Deductions *deductions);

	protected :
		virtual void write(const wchar_t *string, int x, int y, unsigned color);
		virtual void write(const wchar_t *string, int maxLength, bool fillWithBlanks, int x, int y, unsigned color);

	private :
#if 0
		HANDLE output;
		HANDLE input;

		CONSOLE_SCREEN_BUFFER_INFO initialState;
		DWORD initialOutputMode;
		DWORD initialInputMode;
		BOOL initialCursorVisibility;

		CHAR_INFO *characters;
#endif
};

/* -------------------------------------------------------------------------- */

#endif
