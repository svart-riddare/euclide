#ifndef __CONSOLE_H
#define __CONSOLE_H

#include "includes.h"
#include "output.h"
#include "timer.h"

/* -------------------------------------------------------------------------- */

class Console
{
	public :
		Console();
		virtual ~Console();

		virtual void reset();
		virtual void clear();
		virtual void done();
		virtual bool wait();

		virtual void displayTimer();
		virtual void displayError(const wchar_t *string);
		virtual void displayMessage(const wchar_t *string);

		virtual void displayCopyright(const wchar_t *copyright);
		virtual void displayMessage(EUCLIDE_Message message);
		virtual void displayProblem(const EUCLIDE_Problem *problem);
		virtual void displayProgress(int whiteFreeMoves, int blackFreeMoves, double complexity);
		virtual void displayDeductions(const EUCLIDE_Deductions *deductions);

		operator const EUCLIDE_Callbacks *() const;
		bool operator!() const;

		void operator<<(const char *inputName);

	protected :
		static void _displayCopyright(EUCLIDE_Handle handle, const wchar_t *copyright)                                   { reinterpret_cast<Console *>(handle)->displayCopyright(copyright); }
		static void _displayMessage(EUCLIDE_Handle handle, EUCLIDE_Message message)                                      { reinterpret_cast<Console *>(handle)->displayMessage(message); }
		static void _displayProblem(EUCLIDE_Handle handle, const EUCLIDE_Problem *problem)                               { reinterpret_cast<Console *>(handle)->displayProblem(problem); }
		static void _displayProgress(EUCLIDE_Handle handle, int whiteFreeMoves, int blackFreeMoves, double complexity)   { reinterpret_cast<Console *>(handle)->displayProgress(whiteFreeMoves, blackFreeMoves, complexity); }
		static void _displayDeductions(EUCLIDE_Handle handle, const EUCLIDE_Deductions *deductions)                      { reinterpret_cast<Console *>(handle)->displayDeductions(deductions); }

		virtual void write(const wchar_t *string, int x, int y, unsigned color);
		virtual void write(const wchar_t *string, int maxLength, bool fillWithBlanks, int x, int y, unsigned color);

	private :
		EUCLIDE_Callbacks callbacks;
		Output output;
		Timer timer;

	protected :
		int width;
		int height;

		bool valid;
		bool abort;
};

/* -------------------------------------------------------------------------- */

#endif
