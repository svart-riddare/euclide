#ifndef __OUTPUT_H
#define __OUTPUT_H

#include "includes.h"
#include "timer.h"

/* -------------------------------------------------------------------------- */

class Output
{
	public:
		Output(const Strings& strings, const char *inputFileName = nullptr);
		~Output();

		void open(const char *inputFileName);

		void reset();
		void done(EUCLIDE_Status status);

		void displayCopyright(const wchar_t *copyright) const;
		void displayMessage(EUCLIDE_Message message) const;
		void displayProblem(const EUCLIDE_Problem& problem) const;
		void displayProgress(int whiteFreeMoves, int blackFreeMoves, double complexity) const;
		void displayDeductions(const EUCLIDE_Deductions& deductions) const;
		void displayThinking(const EUCLIDE_Thinking& thinking) const;
		void displaySolution(const EUCLIDE_Solution& solution) const;

	public:
		inline operator const EUCLIDE_Callbacks *() const
			{ return &m_callbacks; }
		inline bool operator!() const
			{ return !m_file; }

	protected:
		static void displayCopyrightCallback(EUCLIDE_UserHandle handle, const wchar_t *copyright)
			{ return reinterpret_cast<Output *>(handle)->displayCopyright(copyright); }
		static void displayMessageCallback(EUCLIDE_UserHandle handle, EUCLIDE_Message message)
			{ return reinterpret_cast<Output *>(handle)->displayMessage(message); }
		static void displayProblemCallback(EUCLIDE_UserHandle handle, const EUCLIDE_Problem *problem)
			{ return reinterpret_cast<Output *>(handle)->displayProblem(*problem); }
		static void displayProgressCallback(EUCLIDE_UserHandle handle, int whiteFreeMoves, int blackFreeMoves, double complexity)
			{ return reinterpret_cast<Output *>(handle)->displayProgress(whiteFreeMoves, blackFreeMoves, complexity); }
		static void displayDeductionsCallback(EUCLIDE_UserHandle handle, const EUCLIDE_Deductions *deductions)
			{ return reinterpret_cast<Output *>(handle)->displayDeductions(*deductions); }
		static void displayThinkingCallback(EUCLIDE_UserHandle handle, const EUCLIDE_Thinking *thinking)
			{ return reinterpret_cast<Output *>(handle)->displayThinking(*thinking); }
		static void displaySolutionCallback(EUCLIDE_UserHandle handle, const EUCLIDE_Solution *solution)
			{ return reinterpret_cast<Output *>(handle)->displaySolution(*solution); }

	private:
		const Strings& m_strings;         /**< Text strings. */

		EUCLIDE_Callbacks m_callbacks;    /**< Euclide callbacks. */
		FILE *m_file;                     /**< Output file. */

		Timer m_timer;                    /**< Timer used to output total solving time. */
		mutable double m_complexity;      /**< Solving complexity, only the last value is written to file. */
		mutable int64_t m_positions;      /**< Number of positions examined. */
		mutable int m_solutions;          /**< Number of solutions found. */
};

/* -------------------------------------------------------------------------- */

#endif
