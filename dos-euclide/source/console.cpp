#include "console.h"

/* -------------------------------------------------------------------------- */

Console::Console(const Strings& strings)
	: _output(strings), _strings(strings), _width(0), _height(0), _valid(false), _abort(false)
{
	/* -- Initialize callbacks -- */

	memset(&_callbacks, 0, sizeof(_callbacks));

	_callbacks.displayCopyright = displayCopyrightCallback;
	_callbacks.displayProblem = displayProblemCallback;
	_callbacks.displayMessage = displayMessageCallback;
	_callbacks.displayProgress = displayProgressCallback;
	_callbacks.displayDeductions = displayDeductionsCallback;
	_callbacks.handle = this;
}

/* -------------------------------------------------------------------------- */

Console::~Console()
{
}

/* -------------------------------------------------------------------------- */

void Console::reset()
{
	_output.reset();
	_timer = Timer();
	clear();
}

/* -------------------------------------------------------------------------- */

void Console::clear()
{
}

/* -------------------------------------------------------------------------- */

void Console::done(EUCLIDE_Status status)
{
	_output.done(status);
	wait();
}

/* -------------------------------------------------------------------------- */

bool Console::wait()
{
	write(_strings[Strings::PressAnyKey], _width - 1, true, 0, _height - 1, Colors::Question);
	return !_abort;
}

/* -------------------------------------------------------------------------- */

void Console::displayTimer() const
{
	wchar_t string[32];
	swprintf(string, countof(string), L"%16ls", _timer.elapsed());
	write(string, _width - 17, 7, Colors::Timer);
}

/* -------------------------------------------------------------------------- */

void Console::displayError(const wchar_t *string) const
{
	write(string, _width - 10, true, 9, 4, Colors::Error);
}

/* -------------------------------------------------------------------------- */

void Console::displayMessage(const wchar_t *string) const
{
	write(string, _width - 10, true, 9, 1, Colors::Standard);
}

/* -------------------------------------------------------------------------- */

void Console::displayCopyright(const wchar_t *copyright) const
{
	_output.displayCopyright(copyright);

	const int length = wcslen(copyright);
	const int x = _width - length - 1;
	if (x < 0)
		return;

	write(copyright, x, 0, Colors::Copyright);
	displayTimer();
}

/* -------------------------------------------------------------------------- */

void Console::displayMessage(EUCLIDE_Message message) const
{
	_output.displayMessage(message);

	displayMessage(_strings[message]);
	displayTimer();
}

/* -------------------------------------------------------------------------- */

void Console::displayProblem(const EUCLIDE_Problem& problem) const
{
	_output.displayProblem(problem);

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

	swprintf(string, countof(string), L"%d%ls%d %ls", problem.numHalfMoves / 2, _strings[Strings::Dot], 5 * (problem.numHalfMoves % 2), _strings[Strings::Moves]);
	write(string, 32, true, 9, 7, Colors::Standard);
	
	swprintf(string, countof(string), L"(%d+%d)", numWhitePieces, numBlackPieces);
	write(string, 16, true, 9, 6, Colors::Standard);

	/* -- Show timer -- */
	
	displayTimer();
}

/* -------------------------------------------------------------------------- */

void Console::displayProgress(int whiteFreeMoves, int blackFreeMoves, double complexity) const
{
	_output.displayProgress(whiteFreeMoves, blackFreeMoves, complexity);

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
	_output.displayDeductions(deductions);

	displayProgress(deductions.freeWhiteMoves, deductions.freeBlackMoves, deductions.complexity);
	displayTimer();
}

/* -------------------------------------------------------------------------- */

void Console::open(const char *inputFileName)
{
	_output.open(inputFileName);
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
