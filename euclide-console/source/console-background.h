#ifndef __CONSOLE_BACKGROUND_H
#define __CONSOLE_BACKGROUND_H

#include "includes.h"
#include "console.h"

/* -------------------------------------------------------------------------- */

class BackgroundConsole : public Console
{
	public:
		BackgroundConsole(Console& console, bool active = false);
		virtual ~BackgroundConsole();

		virtual void reset(std::chrono::seconds timeout, const Timer& timer = Timer()) override;
		virtual void clear() override;
		virtual void done(EUCLIDE_Status status) override;
		virtual bool wait() override;

		virtual void displayTimer() const override { assert(false); }
		virtual void displayError(const wchar_t *) const override { assert(false); }
		virtual void displayMessage(const wchar_t *) const override { assert(false); }

		virtual void displayCopyright(const wchar_t *copyright) const override;
		virtual void displayOptions(const EUCLIDE_Options& options) const override;
		virtual void displayMessage(EUCLIDE_Message message) const override;
		virtual void displayProblem(const EUCLIDE_Problem& problem) const override;
		virtual void displayDeductions(const EUCLIDE_Deductions& deductions) const override;
		virtual void displayThinking(const EUCLIDE_Thinking& thinking) const override;
		virtual void displaySolution(const EUCLIDE_Solution& solution) const override;

		virtual bool abort() const;

		bool foreground(bool wait);

	private:
		Console& m_console;                                   /**< Foreground console. */
		bool m_active;                                        /**< Set when background console should forward display to foreground console. */
		int m_id;                                             /**< Console id, used for debugging. */

		mutable std::condition_variable m_condition;          /**< Condition variable used to wait for done() to be called. */
		mutable std::mutex m_lock;                            /**< Mutex to protect against concurrent access of method foreground(). */

		mutable bool m_displayCopyright;                      /**< Set if displayCopyright callback has been called. */
		mutable std::wstring m_copyright;                     /**< Last parameters received through aforementioned callback. */

		mutable bool m_displayOptions;                        /**< Set if displayOptions callback has been called. */
		mutable EUCLIDE_Options m_options;                    /**< Last parameters received through aforementioned callback. */

		mutable bool m_displayMessage;                        /**< Set if displayMessage callback has been called. */
		mutable EUCLIDE_Message m_message;                    /**< Last parameters received through aforementioned callback. */

		mutable bool m_displayProblem;                        /**< Set if displayProblem callback has been called. */
		mutable EUCLIDE_Problem m_problem;                    /**< Last parameters received through aforementioned callback. */

		mutable bool m_displayDeductions;                     /**< Set if displayDeductions callback has been called. */
		mutable EUCLIDE_Deductions m_deductions;              /**< Last parameters received through aforementioned callback. */

		mutable bool m_displayThinking;                       /**< Set if displayThinking callback has been called. */
		mutable EUCLIDE_Thinking m_thinking;                  /**< Last parameters received through aforementioned callback. */

		mutable bool m_displaySolution;                       /**< Set if displaySolution callback has been called. */
		mutable std::vector<EUCLIDE_Solution> m_solutions;    /**< List of solutions received through aforementioned callback. */

		bool m_done;                                          /**< Set when done() function has been called. */
		EUCLIDE_Status m_status;                              /**< Status forwarded to the done() function. */
};

/* -------------------------------------------------------------------------- */

#endif
