#ifndef __CONSOLE_LINUX_H
#define __CONSOLE_LINUX_H

#include "includes.h"
#include "console.h"

/* -------------------------------------------------------------------------- */

class LinuxConsole : public Console
{
	public:
		LinuxConsole(const Strings& strings);
		virtual ~LinuxConsole();

		virtual void clear();
		virtual bool wait();

		virtual void displayProblem(const EUCLIDE_Problem& problem) const;
		virtual void displayDeductions(const EUCLIDE_Deductions& deductions) const;

	protected:
		virtual void write(const wchar_t *string, int x, int y, Color color) const;
		virtual void write(const wchar_t *string, int maxLength, bool fillWithBlanks, int x, int y, Color color) const;
};

/* -------------------------------------------------------------------------- */

#endif
