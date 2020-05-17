#ifndef __CONSOLE_WIN_H
#define __CONSOLE_WIN_H

#include "includes.h"
#include "console.h"

/* -------------------------------------------------------------------------- */

class WinConsole : public Console
{
	public:
		WinConsole(const Strings& strings);
		virtual ~WinConsole();

		virtual void clear() override;
		virtual bool wait() override;

		virtual void displayProblem(const EUCLIDE_Problem& problem) const override;
		virtual void displayDeductions(const EUCLIDE_Deductions& deductions) const override;

	protected:
		virtual void write(const wchar_t *string, int x, int y, Color color) const override;
		virtual void write(const wchar_t *string, int maxLength, bool fillWithBlanks, int x, int y, Color color) const override;

	private:
		HANDLE m_output;                              /**< Console output handle. */
		HANDLE m_input;                               /**< Console input handle. */

		CONSOLE_SCREEN_BUFFER_INFO m_initialState;    /**< Initial console characteristics, restored at exit. */
		DWORD m_initialOutputMode;                    /**< Initial console output mode, restored at exit. */
		DWORD m_initialInputMode;                     /**< Initial console input mode, restored at exit. */
		BOOL m_initialCursorVisibility;               /**< Initial console cursor visibility, restored at exit. */

		mutable CHAR_INFO *m_characters;              /**< Temporary buffer used for console display. */
};

/* -------------------------------------------------------------------------- */

#endif
