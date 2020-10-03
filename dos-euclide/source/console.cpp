#include "console.h"
#include "console-none.h"

#ifdef EUCLIDE_WINDOWS
	#include "console-win.h"
	typedef WinConsole EuclideConsole;
#else
	#include "console-linux.h"
	typedef LinuxConsole EuclideConsole;
#endif

/* -------------------------------------------------------------------------- */

Console *Console::create(const Strings& strings, bool quiet)
{
	if (quiet)
		return new NoConsole(strings);

	Console *console = new EuclideConsole(strings);
	if (console->m_valid)
		return console;

	delete console;
	return nullptr;
}

/* -------------------------------------------------------------------------- */

Console::Console(const Strings& strings)
	: m_stdout(strings), m_output(strings), m_strings(strings), m_width(0), m_height(0), m_valid(false), m_abort(false)
{
	/* -- Initialize callbacks -- */

	memset(&m_callbacks, 0, sizeof(m_callbacks));

	m_callbacks.displayCopyright = displayCopyrightCallback;
	m_callbacks.displayProblem = displayProblemCallback;
	m_callbacks.displayMessage = displayMessageCallback;
	m_callbacks.displayProgress = displayProgressCallback;
	m_callbacks.displayDeductions = displayDeductionsCallback;
	m_callbacks.displayThinking = displayThinkingCallback;
	m_callbacks.displaySolution = displaySolutionCallback;
	m_callbacks.abort = abortCallback;
	m_callbacks.handle = this;
}

/* -------------------------------------------------------------------------- */

Console::~Console()
{
}

/* -------------------------------------------------------------------------- */

void Console::reset(std::chrono::seconds timeout)
{
	m_stdout.reset();
	m_output.reset();

	m_timer = Timer();

	m_solutions = 0;
	m_timeout = timeout;

	clear();
}

/* -------------------------------------------------------------------------- */

void Console::clear()
{
}

/* -------------------------------------------------------------------------- */

void Console::done(EUCLIDE_Status status)
{
	m_stdout.done(status);
	m_output.done(status);

	displayMessage(L"");

	if (m_solutions <= 1)
	{
		wchar_t string[32];
		swprintf(string, countof(string), L"%24ls", m_strings[m_solutions ? Strings::UniqueSolution : Strings::NoSolution]);
		write(string, m_width - 25, 5, Colors::Verdict);
	}
}

/* -------------------------------------------------------------------------- */

bool Console::wait()
{
	write(m_strings[Strings::PressAnyKey], m_width - 1, true, 0, m_height - 1, Colors::Question);
	return !m_abort;
}

/* -------------------------------------------------------------------------- */

void Console::displayTimer() const
{
	wchar_t string[32];
	swprintf(string, countof(string), L"%16ls", m_timer.elapsed());
	write(string, m_width - 17, 7, Colors::Timer);
}

/* -------------------------------------------------------------------------- */

void Console::displayError(const wchar_t *string) const
{
	write(string, m_width - 10, true, 9, 4, Colors::Error);
}

/* -------------------------------------------------------------------------- */

void Console::displayMessage(const wchar_t *string) const
{
	write(string, m_width - 10, true, 9, 1, Colors::Standard);
}

/* -------------------------------------------------------------------------- */

void Console::displayCopyright(const wchar_t *copyright) const
{
	m_stdout.displayCopyright(copyright);
	m_output.displayCopyright(copyright);

	const int length = wcslen(copyright);
	const int x = m_width - length - 1;
	if (x < 0)
		return;

	write(copyright, x, 0, Colors::Copyright);
	displayTimer();
}

/* -------------------------------------------------------------------------- */

void Console::displayMessage(EUCLIDE_Message message) const
{
	m_stdout.displayMessage(message);
	m_output.displayMessage(message);

	displayMessage(m_strings[message]);
	displayTimer();
}

/* -------------------------------------------------------------------------- */

void Console::displayProblem(const EUCLIDE_Problem& problem) const
{
	m_stdout.displayProblem(problem);
	m_output.displayProblem(problem);

	/* -- Count number of pieces, by color -- */

	int numWhitePieces = 0;
	int numBlackPieces = 0;

	for (int square = 0; square < 64; square++)
	{
		EUCLIDE_Glyph glyph = problem.diagram[square];

		const bool isWhiteGlyph = ((glyph == EUCLIDE_GLYPH_WHITE_KING) || (glyph == EUCLIDE_GLYPH_WHITE_QUEEN) || (glyph == EUCLIDE_GLYPH_WHITE_ROOK) || (glyph == EUCLIDE_GLYPH_WHITE_BISHOP) || (glyph == EUCLIDE_GLYPH_WHITE_KNIGHT) || (glyph == EUCLIDE_GLYPH_WHITE_PAWN));
		const bool isBlackGlyph = ((glyph == EUCLIDE_GLYPH_BLACK_KING) || (glyph == EUCLIDE_GLYPH_BLACK_QUEEN) || (glyph == EUCLIDE_GLYPH_BLACK_ROOK) || (glyph == EUCLIDE_GLYPH_BLACK_BISHOP) || (glyph == EUCLIDE_GLYPH_BLACK_KNIGHT) || (glyph == EUCLIDE_GLYPH_BLACK_PAWN));

		if (isWhiteGlyph)
			numWhitePieces++;
		if (isBlackGlyph)
			numBlackPieces++;
	}

	/* -- Show number of half moves and number of pieces -- */

	wchar_t string[32];

	swprintf(string, countof(string), L"%d%ls%d %ls", problem.numHalfMoves / 2, m_strings[Strings::Dot], 5 * (problem.numHalfMoves % 2), m_strings[Strings::Moves]);
	write(string, 32, true, 9, 7, Colors::Standard);

	swprintf(string, countof(string), L"(%d+%d)", numWhitePieces, numBlackPieces);
	write(string, 16, true, 9, 6, Colors::Standard);

	/* -- Show timer -- */

	displayTimer();
}

/* -------------------------------------------------------------------------- */

void Console::displayProgress(int whiteFreeMoves, int blackFreeMoves, double complexity) const
{
	m_stdout.displayProgress(whiteFreeMoves, blackFreeMoves, complexity);
	m_output.displayProgress(whiteFreeMoves, blackFreeMoves, complexity);

	wchar_t string[32];

	swprintf(string, countof(string), L"%d - %d", whiteFreeMoves, blackFreeMoves);
	write(string, 16, true, 11, 2, Colors::FreeMoves);

	swprintf(string, countof(string), L"%.2f", complexity);
	write(string, 16, true, 27, 2, Colors::Complexity);

	displayTimer();
}

/* -------------------------------------------------------------------------- */

void Console::displayDeductions(const EUCLIDE_Deductions& deductions) const
{
	m_stdout.displayDeductions(deductions);
	m_output.displayDeductions(deductions);

	displayProgress(deductions.freeWhiteMoves, deductions.freeBlackMoves, deductions.complexity);
	displayTimer();
}

/* -------------------------------------------------------------------------- */

void Console::displayThinking(const EUCLIDE_Thinking& thinking) const
{
	m_stdout.displayThinking(thinking);
	m_output.displayThinking(thinking);

	wchar_t string[6 * countof(thinking.moves) + 1];

	if (thinking.numHalfMoves)
	{
		const int black = (thinking.moves[0].glyph & 1) ^ 1;
		wcscpy(string, black ? L"1. ...  " : L"");

		wchar_t *s = string + wcslen(string);
		for (int m = 0; m < thinking.numHalfMoves - black; m++)
		{
			const EUCLIDE_Move& move = thinking.moves[m];

			if ((m & 1) == black)
			{
				*s++ = move.move + '0';
				*s++ = '.';
			}

			*s++ = toupper(m_strings[Strings::GlyphSymbols][move.glyph]);
			if (move.capture)
				*s++ = 'x';
			*s++ = move.to / 8 + 'a';
			*s++ = move.to % 8 + '1';
			if (!move.capture)
				*s++ = ' ';
			*s++ = ' ';
		}
		*s++ = '\0';

		write(string, m_width - (s - string) - 1, 3, Colors::Thinking);
	}
	else
	{
		write(L"", countof(string), true, m_width - countof(string) - 1, 3, Colors::Thinking);
	}

	swprintf(string, countof(string), L"%" PRId64, thinking.positions);
	write(string, 16, true, 43, 2, Colors::Complexity);

	displayTimer();
}

/* -------------------------------------------------------------------------- */

void Console::displaySolution(const EUCLIDE_Solution& solution) const
{
	m_solutions = solution.solution;

	m_stdout.displaySolution(solution);
	m_output.displaySolution(solution);

	wchar_t string[32];

	const Strings::String verdicts[] = { Strings::OneSolution, Strings::TwoSolutions, Strings::ThreeSolutions, Strings::FourSolutions, Strings::Cooked };
	swprintf(string, countof(string), L"%24ls", m_strings[verdicts[std::min<int>(countof(verdicts), solution.solution) - 1]]);
	write(string, m_width - 25, 5, Colors::Verdict);

	displayTimer();
}

/* -------------------------------------------------------------------------- */

bool Console::abort() const
{
	return (m_timeout > std::chrono::seconds::zero()) && (m_timer.seconds() >= m_timeout);
}

/* -------------------------------------------------------------------------- */

void Console::open(const char *inputFileName, bool print)
{
	if (inputFileName)
		m_output.open(inputFileName);

	if (print)
		m_stdout.bind(stdout);
}

/* -------------------------------------------------------------------------- */

void Console::write(const wchar_t * /*string*/, int /*x*/, int /*y*/, Color /*color*/) const
{
}

/* -------------------------------------------------------------------------- */

void Console::write(const wchar_t *string, int maxLength, bool fillWithBlanks, int x, int y, Color color) const
{
	std::wstring text(string);
	if (maxLength > 0)
		text.resize(maxLength, fillWithBlanks ? ' ' : '\0');

	write(text.c_str(), x, y, color);
}

/* -------------------------------------------------------------------------- */
