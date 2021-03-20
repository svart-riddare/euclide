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
	public:
		static Console *create(const Strings& strings, bool quiet);

	public:
		Console(const Strings& strings);
		virtual ~Console();

		virtual void reset(std::chrono::seconds timeout, const Timer& timer = Timer());
		virtual void clear();
		virtual void done(EUCLIDE_Status status);
		virtual bool wait();

		virtual void displayTimer() const;
		virtual void displayError(const wchar_t *string) const;
		virtual void displayMessage(const wchar_t *string) const;

		virtual void displayCopyright(const wchar_t *copyright) const;
		virtual void displayMessage(EUCLIDE_Message message) const;
		virtual void displayProblem(const EUCLIDE_Problem& problem) const;
		virtual void displayDeductions(const EUCLIDE_Deductions& deductions) const;
		virtual void displayThinking(const EUCLIDE_Thinking& thinking) const;
		virtual void displaySolution(const EUCLIDE_Solution& solution) const;

		virtual bool abort() const;

	public:
		inline operator const EUCLIDE_Callbacks *() const
			{ return &m_callbacks; }
		inline operator const Strings&() const
			{ return m_strings; }
		inline bool operator!() const
			{ return !m_valid || m_abort; }
		inline explicit operator bool() const
			{ return m_valid && !m_abort; }

		inline const EUCLIDE_Callbacks *callbacks() const
			{ return &m_callbacks; }
		inline const Strings& strings() const
			{ return m_strings; }

		inline const Timer& timer() const
			{ return m_timer; }

	public:
		void open(const char *inputFileName, bool print = false);

	protected:
		static void displayCopyrightCallback(EUCLIDE_UserHandle handle, const wchar_t *copyright)
			{ reinterpret_cast<Console *>(handle)->displayCopyright(copyright); }
		static void displayMessageCallback(EUCLIDE_UserHandle handle, EUCLIDE_Message message)
			{ reinterpret_cast<Console *>(handle)->displayMessage(message); }
		static void displayProblemCallback(EUCLIDE_UserHandle handle, const EUCLIDE_Problem *problem)
			{ reinterpret_cast<Console *>(handle)->displayProblem(*problem); }
		static void displayDeductionsCallback(EUCLIDE_UserHandle handle, const EUCLIDE_Deductions *deductions)
			{ reinterpret_cast<Console *>(handle)->displayDeductions(*deductions); }
		static void displayThinkingCallback(EUCLIDE_UserHandle handle, const EUCLIDE_Thinking *thinking)
			{ reinterpret_cast<Console *>(handle)->displayThinking(*thinking); }
		static void displaySolutionCallback(EUCLIDE_UserHandle handle, const EUCLIDE_Solution *solution)
			{ reinterpret_cast<Console *>(handle)->displaySolution(*solution); }
		static bool abortCallback(EUCLIDE_UserHandle handle)
			{ return reinterpret_cast<Console *>(handle)->abort(); }

		virtual void write(const wchar_t *string, int x, int y, Color color) const;
		virtual void write(const wchar_t *string, int maxLength, bool fillWithBlanks, int x, int y, Color color) const;

	private:
		EUCLIDE_Callbacks m_callbacks;     /**< Euclide engine callbacks. */
		Output m_stdout;                   /**< Stdout output, when not using console mode. */
		Output m_output;                   /**< Output file for solving results. */
		Timer m_timer;                     /**< Timer use to output solving time. */

		mutable int m_solutions;           /**< Number of distinct solutions found. */
		std::chrono::seconds m_timeout;    /**< Timeout, in seconds. Zero if none. */

	protected:
		const Strings& m_strings;          /**< Constant strings. */

		int m_width;                       /**< Console width, in characters. */
		int m_height;                      /**< Console height, in characters. */

		bool m_valid;                      /**< Set unless console failed to initialize. */
		bool m_abort;                      /**< Set when the user hit ESC to abort solving. */
};

/* -------------------------------------------------------------------------- */

#endif
