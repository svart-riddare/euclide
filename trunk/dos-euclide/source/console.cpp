#include "console.h"
#include "colors.h"
#include "strings.h"

/* -------------------------------------------------------------------------- */

Console::Console()
{
	/* -- Initialize EUCLIDE_Callbacks structure -- */

	memset(&callbacks, 0, sizeof(callbacks));

	callbacks.displayCopyright = _displayCopyright;
	callbacks.displayProblem = _displayProblem;
	callbacks.displayMessage = _displayMessage;
	callbacks.displayFreeMoves = _displayFreeMoves;

	callbacks.handle = static_cast<EUCLIDE_Handle>(this);

	/* -- Initialize console handles -- */

	output = GetStdHandle(STD_OUTPUT_HANDLE);
	input = GetStdHandle(STD_INPUT_HANDLE);
	valid = false;

	if ((output == INVALID_HANDLE_VALUE) || (input == INVALID_HANDLE_VALUE))
		return;

	GetConsoleScreenBufferInfo(output, &initialState);
	width = std::min((int)initialState.dwSize.X, 80);
	height = std::min((int)initialState.dwSize.Y, 25);

	if ((width < 64) || (height < 25))
		return;

	GetConsoleMode(output, &initialOutputMode);
	GetConsoleMode(input, &initialInputMode);
	SetConsoleMode(input, ENABLE_PROCESSED_INPUT);
		
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(output, &cursorInfo);
	initialCursorVisibility = cursorInfo.bVisible;
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(output, &cursorInfo);

	SMALL_RECT window;
	window.Bottom = (SHORT)(height - 1);
	window.Right = (SHORT)(width - 1);
	window.Left = 0;
	window.Top = 0;
	SetConsoleWindowInfo(output, TRUE, &window);

	valid = true;
	clear();

	FlushConsoleInputBuffer(input);
}

/* -------------------------------------------------------------------------- */

Console::~Console()
{
	COORD cursor = { 0, 8 };
	SetConsoleCursorPosition(output, cursor);

	DWORD written;
	FillConsoleOutputAttribute(output, initialState.wAttributes, width * (height - 8), cursor, &written);
	FillConsoleOutputCharacter(output, ' ', width * (height - 8), cursor, &written);

	cursor.Y++;

	SetConsoleWindowInfo(output, TRUE, &initialState.srWindow);
	SetConsoleTextAttribute(output, initialState.wAttributes);
	SetConsoleCursorPosition(output, cursor);

	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(output, &cursorInfo);
	cursorInfo.bVisible = initialCursorVisibility;
	SetConsoleCursorInfo(output, &cursorInfo);

	SetConsoleMode(output, initialOutputMode);
	SetConsoleMode(input, initialInputMode);
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
	COORD cursor = { 0, 0 };
	SetConsoleCursorPosition(output, cursor);

	DWORD written;
	FillConsoleOutputAttribute(output, colors::standard, width * height, cursor, &written);
	FillConsoleOutputCharacter(output, ' ', width * height, cursor, &written);

	SetConsoleTextAttribute(output, initialState.wAttributes);
	SetConsoleCursorPosition(output, cursor);
}

/* -------------------------------------------------------------------------- */

bool Console::wait()
{
	write(strings::load(strings::PressAnyKey), width - 1, true, 0, height - 1, colors::question);

	DWORD read;
	TCHAR characters[2];
	FlushConsoleInputBuffer(input);
	WaitForSingleObject(input, INFINITE);
	ReadConsole(input, characters, 1, &read, NULL);

	write(_T(""), width - 1, true, 0, height - 1, colors::standard);
	return (*characters == VK_ESCAPE) || (*characters == 'x') || (*characters == 'X');
}

/* -------------------------------------------------------------------------- */

void Console::displayTimer()
{
	TCHAR string[32];
	_stprintf(string, _T("%16s"), (LPCTSTR)timer);
	write(string, width - 17, 7, colors::timer);
}

/* -------------------------------------------------------------------------- */

void Console::displayError(LPCTSTR string)
{
	write(string, width - 10, true, 9, 4, colors::error);
}

/* -------------------------------------------------------------------------- */

void Console::displayMessage(LPCTSTR string)
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

#ifdef _UNICODE
	write(copyright, x, 0, colors::copyright);
#else
	char *ansiCopyright = new char[2 * length + 1];
	CharToOemW(copyright, ansiCopyright);
	write(ansiCopyright, x, 0, Color::Copyright);
	delete[] ansiCopyright;
#endif

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
	CHAR_INFO chessboard[64];

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

		int column = square / 8;
		int row = square % 8;
		int index = 8 * (7 - row) + column;
		
		chessboard[index].Char.UnicodeChar = strings::load(strings::GlyphSymbols)[glyph];
		chessboard[index].Attributes = (((column & 1) ^ (row & 1)) ? colors::lightSquares : colors::darkSquares);
		chessboard[index].Attributes |= (isWhiteGlyph ? colors::whitePieces : colors::blackPieces);
	}

	COORD size = { 8, 8 };
	COORD corner = { 0, 0 };
	SMALL_RECT window = { 0, 0, 7, 7 };
	WriteConsoleOutput(output, chessboard, size, corner, &window);

	LPCTSTR half = strings::load((problem->numHalfMoves & 1) ? strings::HalfMove : strings::NoHalfMove);
	LPCTSTR moves = strings::load(strings::Moves);

	size_t length = 16 + _tcslen(half) + _tcslen(moves);
	TCHAR *string = new TCHAR[length];
	
	_stprintf(string, _T("%d%s %s"), problem->numHalfMoves / 2, half, moves);
	write(string, 32, true, 9, 7, colors::standard);
	
	_stprintf(string, _T("(%d+%d)"), numWhitePieces, numBlackPieces);
	write(string, 16, true, 9, 6, colors::standard);
	displayTimer();

	delete[] string;
}

/* -------------------------------------------------------------------------- */

void Console::displayFreeMoves(int whiteFreeMoves, int blackFreeMoves)
{
	TCHAR string[32];
	_stprintf(string, _T("%d - %d"), whiteFreeMoves, blackFreeMoves);

	write(string, 32, true, 11, 2, colors::freeMoves);
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
	return !valid;
}

/* -------------------------------------------------------------------------- */

void Console::write(LPCTSTR string, int x, int y, WORD color)
{
	DWORD written;
	DWORD length = (DWORD)_tcslen(string);

	COORD position = { (SHORT)x, (SHORT)y };

	SetConsoleTextAttribute(output, color);
	SetConsoleCursorPosition(output, position);
	WriteConsole(output, string, length, &written, NULL);
}

/* -------------------------------------------------------------------------- */

void Console::write(LPCTSTR string, int maxLength, bool fillWithBlanks, int x, int y, WORD color)
{
	if (maxLength <= 0)
		maxLength = (int)_tcslen(string);

	int length = std::min((int)_tcslen(string), maxLength);

	TCHAR *text = new TCHAR[maxLength + 1];
	_tcsncpy(text, string, length);
	text[maxLength] = '\0';
	text[length] = '\0';

	if (fillWithBlanks)
		for (int k = length; k < maxLength; k++)
			text[k] = ' ';

	write(text, x, y, color);
}

/* -------------------------------------------------------------------------- */
