#ifndef __CONSOLE_H
#define __CONSOLE_H

#include "includes.h"
#include "timer.h"

/* -------------------------------------------------------------------------- */

class Console
{
	public :
		Console();
		~Console();

		void reset();
		void clear();
		bool wait();

		void displayTimer();
		void displayError(LPCTSTR string);
		void displayMessage(LPCTSTR string);

		void displayCopyright(const wchar_t *copyright);
		void displayMessage(EUCLIDE_Message message);
		void displayProblem(const EUCLIDE_Problem *problem);
		void displayFreeMoves(int whiteFreeMoves, int blackFreeMoves);
		void displayDeductions(const EUCLIDE_Deductions *deductions);

		operator const EUCLIDE_Callbacks *() const;
		bool operator!() const;

	protected :
		static void _displayCopyright(EUCLIDE_Handle handle, const wchar_t *copyright)                 { return static_cast<Console *>(handle)->displayCopyright(copyright); }
		static void _displayMessage(EUCLIDE_Handle handle, EUCLIDE_Message message)                    { return static_cast<Console *>(handle)->displayMessage(message); }
		static void _displayProblem(EUCLIDE_Handle handle, const EUCLIDE_Problem *problem)             { return static_cast<Console *>(handle)->displayProblem(problem); }
		static void _displayFreeMoves(EUCLIDE_Handle handle, int whiteFreeMoves, int blackFreeMoves)   { return static_cast<Console *>(handle)->displayFreeMoves(whiteFreeMoves, blackFreeMoves); }
		static void _displayDeductions(EUCLIDE_Handle handle, const EUCLIDE_Deductions *deductions)    { return static_cast<Console *>(handle)->displayDeductions(deductions); }

		void write(LPCTSTR string, int x, int y, WORD color);
		void write(LPCTSTR string, int maxLength, bool fillWithBlanks, int x, int y, WORD color);

	private :
		HANDLE output;
		HANDLE input;
		bool valid;
		bool abort;

		int width;
		int height;

		CONSOLE_SCREEN_BUFFER_INFO initialState;
		DWORD initialOutputMode;
		DWORD initialInputMode;
		BOOL initialCursorVisibility;

		EUCLIDE_Callbacks callbacks;
		Timer timer;

		CHAR_INFO *characters;
};

/* -------------------------------------------------------------------------- */

#endif
