#ifndef __CONSOLE_H
#define __CONSOLE_H

#include "includes.h"
#include "strings.h"
#include "colors.h"
#include "output.h"
#include "timer.h"

/* -------------------------------------------------------------------------- */

class Console
{
	public :
		Console(const Strings& strings);
		virtual ~Console();

		virtual void reset();
		virtual void clear();
		virtual void done();
		virtual bool wait();

		virtual void displayTimer() const;
		virtual void displayError(const wchar_t *string) const;
		virtual void displayMessage(const wchar_t *string) const;

		virtual void displayCopyright(const wchar_t *copyright) const;
		virtual void displayMessage(EUCLIDE_Message message) const;
		virtual void displayProblem(const EUCLIDE_Problem& problem) const;
		virtual void displayProgress(int whiteFreeMoves, int blackFreeMoves, double complexity) const;
		virtual void displayDeductions(const EUCLIDE_Deductions& deductions) const;

	public :
		inline operator const EUCLIDE_Callbacks *() const
			{ return &_callbacks; }
		inline bool operator!() const
			{ return !_valid || _abort; }

	public :
		void open(const char *inputFileName);

	protected :
		static void displayCopyrightCallback(EUCLIDE_UserHandle handle, const wchar_t *copyright)
			{ reinterpret_cast<Console *>(handle)->displayCopyright(copyright); }
		static void displayMessageCallback(EUCLIDE_UserHandle handle, EUCLIDE_Message message)
			{ reinterpret_cast<Console *>(handle)->displayMessage(message); }
		static void displayProblemCallback(EUCLIDE_UserHandle handle, const EUCLIDE_Problem *problem)
			{ reinterpret_cast<Console *>(handle)->displayProblem(*problem); }
		static void displayProgressCallback(EUCLIDE_UserHandle handle, int whiteFreeMoves, int blackFreeMoves, double complexity)
			{ reinterpret_cast<Console *>(handle)->displayProgress(whiteFreeMoves, blackFreeMoves, complexity); }
		static void displayDeductionsCallback(EUCLIDE_UserHandle handle, const EUCLIDE_Deductions *deductions)
			{ reinterpret_cast<Console *>(handle)->displayDeductions(*deductions); }

		virtual void write(const wchar_t *string, int x, int y, Color color) const;
		virtual void write(const wchar_t *string, int maxLength, bool fillWithBlanks, int x, int y, Color color) const;

	private :
		EUCLIDE_Callbacks _callbacks;    /**< Euclide engine callbacks. */
		Output _output;                  /**< Output file for solving results. */
		Timer _timer;                    /**< Timer use to output solving time. */

	protected :
		const Strings& _strings;         /**< Constant strings. */

		int _width;                      /**< Console width, in characters. */
		int _height;                     /**< Console height, in characters. */

		bool _valid;                     /**< Set unless console failed to initialize. */
		bool _abort;                     /**< Set when the user hit ESC to abort solving. */
};

/* -------------------------------------------------------------------------- */

#endif
