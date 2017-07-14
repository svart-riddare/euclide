#ifndef __CONSOLE_WIN_H
#define __CONSOLE_WIN_H

#include "includes.h"
#include "console.h"

/* -------------------------------------------------------------------------- */

class WinConsole : public Console
{
	public :
		WinConsole(const Strings& strings);
		virtual ~WinConsole();

		virtual void clear();
		virtual bool wait();

		virtual void displayProblem(const EUCLIDE_Problem& problem) const;
		virtual void displayDeductions(const EUCLIDE_Deductions& deductions) const;

	protected :
		virtual void write(const wchar_t *string, int x, int y, Color color) const;
		virtual void write(const wchar_t *string, int maxLength, bool fillWithBlanks, int x, int y, Color color) const;

	private :
		HANDLE _output;                              /**< Console output handle. */
		HANDLE _input;                               /**< Console input handle. */

		CONSOLE_SCREEN_BUFFER_INFO _initialState;    /**< Initial console characteristics, restored at exit. */
		DWORD _initialOutputMode;                    /**< Initial console output mode, restored at exit. */
		DWORD _initialInputMode;                     /**< Initial console input mode, restored at exit. */
		BOOL _initialCursorVisibility;               /**< Initial console cursor visibility, restored at exit. */

		mutable CHAR_INFO *_characters;              /**< Temporary buffer used for console display. */
};

/* -------------------------------------------------------------------------- */

#endif
