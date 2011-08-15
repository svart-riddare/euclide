#include "console.h"
#include "colors.h"
#include "strings.h"

/* -------------------------------------------------------------------------- */

Console::Console()
{
	valid = false;
	abort = false;

	/* -- Initialize EUCLIDE_Callbacks structure -- */

	memset(&callbacks, 0, sizeof(callbacks));

	callbacks.displayCopyright = _displayCopyright;
	callbacks.displayProblem = _displayProblem;
	callbacks.displayMessage = _displayMessage;
	callbacks.displayFreeMoves = _displayFreeMoves;
	callbacks.displayDeductions = _displayDeductions;

	callbacks.handle = static_cast<EUCLIDE_Handle>(this);
}

/* -------------------------------------------------------------------------- */

Console::~Console()
{
}

/* -------------------------------------------------------------------------- */

void Console::reset()
{
	timer = Timer();
	clear();
}

/* -------------------------------------------------------------------------- */

void Console::clear()
{
}

/* -------------------------------------------------------------------------- */

bool Console::wait()
{
	write(strings::load(strings::PressAnyKey), width - 1, true, 0, height - 1, colors::question);
	return !abort;
}

/* -------------------------------------------------------------------------- */

void Console::displayTimer()
{
	wchar_t string[32];
	swprintf(string, sizeof(string) / sizeof(string[0]), L"%16ls", (const wchar_t *)timer);
	write(string, width - 17, 7, colors::timer);
}

/* -------------------------------------------------------------------------- */

void Console::displayError(const wchar_t *string)
{
	write(string, width - 10, true, 9, 4, colors::error);
}

/* -------------------------------------------------------------------------- */

void Console::displayMessage(const wchar_t *string)
{
	write(string, width - 10, true, 9, 1, colors::standard);
}

/* -------------------------------------------------------------------------- */

void Console::displayCopyright(const wchar_t *copyright)
{
	int length = (int)wcslen(copyright);
	int x = width - length - 1;
	if (x < 0)
		return;

	write(copyright, x, 0, colors::copyright);
	displayTimer();
}

/* -------------------------------------------------------------------------- */

void Console::displayMessage(EUCLIDE_Message message)
{
	displayMessage(strings::load(message));
	displayTimer();
}

/* -------------------------------------------------------------------------- */

void Console::displayProblem(const EUCLIDE_Problem *problem)
{
	int numWhitePieces = 0;
	int numBlackPieces = 0;

	for (int square = 0; square < 64; square++)
	{
		EUCLIDE_Glyph glyph = problem->glyphs[square];

		bool isWhiteGlyph = ((glyph == EUCLIDE_GLYPH_WHITE_KING) || (glyph == EUCLIDE_GLYPH_WHITE_QUEEN) || (glyph == EUCLIDE_GLYPH_WHITE_ROOK) || (glyph == EUCLIDE_GLYPH_WHITE_BISHOP) || (glyph == EUCLIDE_GLYPH_WHITE_KNIGHT) || (glyph == EUCLIDE_GLYPH_WHITE_PAWN));
		bool isBlackGlyph = ((glyph == EUCLIDE_GLYPH_BLACK_KING) || (glyph == EUCLIDE_GLYPH_BLACK_QUEEN) || (glyph == EUCLIDE_GLYPH_BLACK_ROOK) || (glyph == EUCLIDE_GLYPH_BLACK_BISHOP) || (glyph == EUCLIDE_GLYPH_BLACK_KNIGHT) || (glyph == EUCLIDE_GLYPH_BLACK_PAWN));

		if (isWhiteGlyph)
			numWhitePieces++;
		if (isBlackGlyph)
			numBlackPieces++;
	}

	const wchar_t *half = strings::load((problem->numHalfMoves & 1) ? strings::HalfMove : strings::NoHalfMove);
	const wchar_t *moves = strings::load(strings::Moves);

	size_t length = 16 + wcslen(half) + wcslen(moves);
	wchar_t *string = new wchar_t[length];
	
	swprintf(string, length, L"%d%ls %ls", problem->numHalfMoves / 2, half, moves);
	write(string, 32, true, 9, 7, colors::standard);
	
	swprintf(string, length, L"(%d+%d)", numWhitePieces, numBlackPieces);
	write(string, 16, true, 9, 6, colors::standard);
	displayTimer();

	delete[] string;
}

/* -------------------------------------------------------------------------- */

void Console::displayFreeMoves(int whiteFreeMoves, int blackFreeMoves)
{
	wchar_t string[32];
	swprintf(string, sizeof(string) / sizeof(string[0]), L"%d - %d", whiteFreeMoves, blackFreeMoves);

	write(string, 32, true, 11, 2, colors::freeMoves);
	displayTimer();
}

/* -------------------------------------------------------------------------- */

void Console::displayDeductions(const EUCLIDE_Deductions *deductions)
{
	displayFreeMoves(deductions->freeWhiteMoves, deductions->freeBlackMoves);
	displayTimer();
}

/* -------------------------------------------------------------------------- */

Console::operator const EUCLIDE_Callbacks *() const
{
	return &callbacks;
}

/* -------------------------------------------------------------------------- */

bool Console::operator!() const
{
	return !valid || abort;
}

/* -------------------------------------------------------------------------- */

void Console::write(const wchar_t * /*string*/, int /*x*/, int /*y*/, unsigned /*color*/)
{
}

/* -------------------------------------------------------------------------- */

void Console::write(const wchar_t *string, int maxLength, bool fillWithBlanks, int x, int y, unsigned color)
{
	if (maxLength <= 0)
		maxLength = (int)wcslen(string);

	int length = std::min((int)wcslen(string), maxLength);

	wchar_t *text = new wchar_t[maxLength + 1];
	wcsncpy(text, string, length);
	text[maxLength] = '\0';
	text[length] = '\0';

	if (fillWithBlanks)
		for (int k = length; k < maxLength; k++)
			text[k] = ' ';

	write(text, x, y, color);
	delete[] text;
}

/* -------------------------------------------------------------------------- */
