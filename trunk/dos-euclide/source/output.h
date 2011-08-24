#ifndef __OUTPUT_H
#define __OUTPUT_H

#include "includes.h"
#include "timer.h"

/* -------------------------------------------------------------------------- */

class Output
{
	public :
		Output();
		Output(const char *inputName);
		~Output();

		void open(const char *inputName);

		void reset();
		void done();

		void displayCopyright(const wchar_t *copyright);
		void displayMessage(EUCLIDE_Message message);
		void displayProblem(const EUCLIDE_Problem *problem);
		void displayProgress(int whiteFreeMoves, int blackFreeMoves, double complexity);
		void displayDeductions(const EUCLIDE_Deductions *deductions);

		operator const EUCLIDE_Callbacks *() const;
		bool operator!() const;

	protected :
		static void _displayCopyright(EUCLIDE_Handle handle, const wchar_t *copyright)                                   { return reinterpret_cast<Output *>(handle)->displayCopyright(copyright); }
		static void _displayMessage(EUCLIDE_Handle handle, EUCLIDE_Message message)                                      { return reinterpret_cast<Output *>(handle)->displayMessage(message); }
		static void _displayProblem(EUCLIDE_Handle handle, const EUCLIDE_Problem *problem)                               { return reinterpret_cast<Output *>(handle)->displayProblem(problem); }
		static void _displayProgress(EUCLIDE_Handle handle, int whiteFreeMoves, int blackFreeMoves, double complexity)   { return reinterpret_cast<Output *>(handle)->displayProgress(whiteFreeMoves, blackFreeMoves, complexity); }
		static void _displayDeductions(EUCLIDE_Handle handle, const EUCLIDE_Deductions *deductions)                      { return reinterpret_cast<Output *>(handle)->displayDeductions(deductions); }

	private :
		EUCLIDE_Callbacks callbacks;
		Timer timer;

	private :
		FILE *file;

	private :
		double complexity;


};

/* -------------------------------------------------------------------------- */

#endif
