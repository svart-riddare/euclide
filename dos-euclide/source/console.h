#ifndef __CONSOLE_H
#define __CONSOLE_H

#include "includes.h"
#include "timer.h"

/* -------------------------------------------------------------------------- */

class Console
{
	public :
		Console();
		virtual ~Console();

		virtual void reset();
		virtual void clear();
		virtual bool wait();

		virtual void displayTimer();
		virtual void displayError(const wchar_t *string);
		virtual void displayMessage(const wchar_t *string);

		virtual void displayCopyright(const wchar_t *copyright);
		virtual void displayMessage(EUCLIDE_Message message);
		virtual void displayProblem(const EUCLIDE_Problem *problem);
		virtual void displayFreeMoves(int whiteFreeMoves, int blackFreeMoves);
		virtual void displayDeductions(const EUCLIDE_Deductions *deductions);

		operator const EUCLIDE_Callbacks *() const;
		bool operator!() const;

	protected :
		static void _displayCopyright(EUCLIDE_Handle handle, const wchar_t *copyright)                 { return reinterpret_cast<Console *>(handle)->displayCopyright(copyright); }
		static void _displayMessage(EUCLIDE_Handle handle, EUCLIDE_Message message)                    { return reinterpret_cast<Console *>(handle)->displayMessage(message); }
		static void _displayProblem(EUCLIDE_Handle handle, const EUCLIDE_Problem *problem)             { return reinterpret_cast<Console *>(handle)->displayProblem(problem); }
		static void _displayFreeMoves(EUCLIDE_Handle handle, int whiteFreeMoves, int blackFreeMoves)   { return reinterpret_cast<Console *>(handle)->displayFreeMoves(whiteFreeMoves, blackFreeMoves); }
		static void _displayDeductions(EUCLIDE_Handle handle, const EUCLIDE_Deductions *deductions)    { return reinterpret_cast<Console *>(handle)->displayDeductions(deductions); }

		virtual void write(const wchar_t *string, int x, int y, unsigned color);
		virtual void write(const wchar_t *string, int maxLength, bool fillWithBlanks, int x, int y, unsigned color);

	private :
		EUCLIDE_Callbacks callbacks;
		Timer timer;

	protected :
		int width;
		int height;

		bool valid;
		bool abort;
};

/* -------------------------------------------------------------------------- */

#endif
