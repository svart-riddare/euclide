#include "console-win.h"
#include "colors.h"
#include "strings.h"

/* -------------------------------------------------------------------------- */

WinConsole::WinConsole()
{
	/* -- Initialize console handles -- */

	output = GetStdHandle(STD_OUTPUT_HANDLE);
	input = GetStdHandle(STD_INPUT_HANDLE);

	if ((output == INVALID_HANDLE_VALUE) || (input == INVALID_HANDLE_VALUE))
		return;

	GetConsoleScreenBufferInfo(output, &initialState);
	width = 2 * (std::min((int)initialState.dwSize.X, 120) / 2);
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

	if ((initialState.srWindow.Bottom < window.Bottom) || (initialState.srWindow.Right < window.Right) || (initialState.srWindow.Left > window.Left) || (initialState.srWindow.Top > window.Top))
		SetConsoleWindowInfo(output, TRUE, &window);

	/* -- Allocate output buffer -- */

	characters = new CHAR_INFO[16 * width];

	/* -- Clear input & output -- */

	valid = true;
	clear();

	FlushConsoleInputBuffer(input);
}

/* -------------------------------------------------------------------------- */

WinConsole::~WinConsole()
{
	/* -- Restore console output -- */

	COORD cursor = { 0, 8 };
	SetConsoleCursorPosition(output, cursor);

	int width = initialState.dwSize.X;
	int height = initialState.dwSize.Y;

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

	/* -- Delete allocated resources -- */

	delete characters;
}

/* -------------------------------------------------------------------------- */

void WinConsole::clear()
{
	COORD cursor = { 0, 0 };
	SetConsoleCursorPosition(output, cursor);

	int width = initialState.dwSize.X;
	int height = initialState.dwSize.Y;

	DWORD written;
	FillConsoleOutputCharacter(output, ' ', width * height, cursor, &written);
	FillConsoleOutputAttribute(output, colors::standard, width * height, cursor, &written);

	SetConsoleTextAttribute(output, initialState.wAttributes);
	SetConsoleCursorPosition(output, cursor);

	Console::clear();
}

/* -------------------------------------------------------------------------- */

bool WinConsole::wait()
{
	Console::wait();

	FlushConsoleInputBuffer(input);

	DWORD records;
	INPUT_RECORD record;
	record.EventType = 0;

	ZeroMemory(&record, sizeof(record));

	while (record.EventType != KEY_EVENT)
		if (!ReadConsoleInput(input, &record, 1, &records))
			return abort = true, false;
	
	if (record.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
		abort = true;

	write(L"", width - 1, true, 0, height - 1, colors::standard);
	return !abort;
}

/* -------------------------------------------------------------------------- */

void WinConsole::displayProblem(const EUCLIDE_Problem *problem)
{
	const wchar_t *glyphs = strings::load(strings::GlyphSymbols);
	CHAR_INFO chessboard[64];

	for (int square = 0; square < 64; square++)
	{
		EUCLIDE_Glyph glyph = problem->glyphs[square];

		bool isWhiteGlyph = ((glyph == EUCLIDE_GLYPH_WHITE_KING) || (glyph == EUCLIDE_GLYPH_WHITE_QUEEN) || (glyph == EUCLIDE_GLYPH_WHITE_ROOK) || (glyph == EUCLIDE_GLYPH_WHITE_BISHOP) || (glyph == EUCLIDE_GLYPH_WHITE_KNIGHT) || (glyph == EUCLIDE_GLYPH_WHITE_PAWN));

		int column = square / 8;
		int row = square % 8;
		int index = 8 * (7 - row) + column;
		
		chessboard[index].Char.UnicodeChar = (WCHAR)toupper(glyphs[glyph]);
		chessboard[index].Attributes = (((column & 1) ^ (row & 1)) ? colors::lightSquares : colors::darkSquares);
		chessboard[index].Attributes |= (isWhiteGlyph ? colors::whitePieces : colors::blackPieces);
	}

	COORD size = { 8, 8 };
	COORD corner = { 0, 0 };
	SMALL_RECT window = { 0, 0, 7, 7 };
	WriteConsoleOutput(output, chessboard, size, corner, &window);

	Console::displayProblem(problem);
}

/* -------------------------------------------------------------------------- */

void WinConsole::displayDeductions(const EUCLIDE_Deductions *deductions)
{
	const wchar_t *symbols = strings::load(strings::GlyphSymbols);
	CHAR_INFO *characters = this->characters;
	
	for (int piece = 0; piece < 16; piece++)
	{
		for (int color = 0; color <= 1; color++)
		{
			const EUCLIDE_Deduction *deduction = color ? &deductions->blackPieces[piece] : &deductions->whitePieces[piece];

			/* -- Set default attributes and clear area -- */

			for (int k = 0; k < width / 2; k++)
			{
				characters[k].Attributes = color ? (deduction->captured ? colors::blackCaptures : colors::blackMoves) : (deduction->captured ? colors::whiteCaptures : colors::whiteMoves);
				characters[k].Char.UnicodeChar = ' ';
			}

			/* -- Print required moves -- */

			if (deduction->requiredMoves > 0)
			{
				characters[1].Attributes = color ? colors::numBlackMoves : colors::numWhiteMoves;
				characters[2].Attributes = color ? colors::numBlackMoves : colors::numWhiteMoves;

				if (deduction->requiredMoves > 9)
					characters[1].Char.UnicodeChar = (WCHAR)('0' + (deduction->requiredMoves / 10 % 10));
				
				characters[2].Char.UnicodeChar = (WCHAR)('0' + deduction->requiredMoves % 10);
			}

			/* -- Print deduction -- */

			if (deduction->finalSquare >= 0)
			{
				characters[5].Char.UnicodeChar = (WCHAR)toupper(symbols[deduction->initialGlyph]);
				characters[6].Char.UnicodeChar = (WCHAR)('a' + deduction->initialSquare / 8);
				characters[7].Char.UnicodeChar = (WCHAR)('1' + deduction->initialSquare % 8);

				if ((deduction->finalSquare != deduction->initialSquare) || (deduction->requiredMoves > 0))
				{
					characters[9].Char.UnicodeChar = '-';
					characters[10].Char.UnicodeChar = '>';

					characters[12].Char.UnicodeChar = (WCHAR)toupper(symbols[deduction->promotionGlyph]);
					characters[13].Char.UnicodeChar = (WCHAR)('a' + deduction->finalSquare / 8);
					characters[14].Char.UnicodeChar = (WCHAR)('1' + deduction->finalSquare % 8);
				}
			}

			/* -- Print number of possible squares -- */

			if (deduction->numSquares > 1)
			{
				if (deduction->numSquares >= 10000)
					characters[20].Char.UnicodeChar = (WCHAR)('0' + (deduction->numSquares / 10000) % 10);
				if (deduction->numSquares >= 1000)
					characters[21].Char.UnicodeChar = (WCHAR)('0' + (deduction->numSquares / 1000) % 10);
				if (deduction->numSquares >= 100)
					characters[22].Char.UnicodeChar = (WCHAR)('0' + (deduction->numSquares / 100) % 10);
				if (deduction->numSquares >= 10)
					characters[23].Char.UnicodeChar = (WCHAR)('0' + (deduction->numSquares / 10) % 10);
				if (deduction->numSquares >= 1)
					characters[24].Char.UnicodeChar = (WCHAR)('0' + (deduction->numSquares / 1) % 10);

				characters[20].Attributes = color ? colors::numBlackSquares : colors::numWhiteSquares;
				characters[21].Attributes = color ? colors::numBlackSquares : colors::numWhiteSquares;
				characters[22].Attributes = color ? colors::numBlackSquares : colors::numWhiteSquares;
				characters[23].Attributes = color ? colors::numBlackSquares : colors::numWhiteSquares;
				characters[24].Attributes = color ? colors::numBlackSquares : colors::numWhiteSquares;
			}

			/* -- Print number of possible moves -- */

			if (deduction->requiredMoves < deduction->numMoves)
			{
				int numExtraMoves = deduction->numMoves - deduction->requiredMoves;

				if (numExtraMoves >= 10000)
					characters[27].Char.UnicodeChar = (WCHAR)('0' + (numExtraMoves / 10000) % 10);
				if (numExtraMoves >= 1000)
					characters[28].Char.UnicodeChar = (WCHAR)('0' + (numExtraMoves / 1000) % 10);
				if (numExtraMoves >= 100)
					characters[29].Char.UnicodeChar = (WCHAR)('0' + (numExtraMoves / 100) % 10);
				if (numExtraMoves >= 10)
					characters[30].Char.UnicodeChar = (WCHAR)('0' + (numExtraMoves / 10) % 10);
				if (numExtraMoves >= 1)
					characters[31].Char.UnicodeChar = (WCHAR)('0' + (numExtraMoves / 1) % 10);

				characters[27].Attributes = color ? colors::numBlackExtraMoves : colors::numWhiteExtraMoves;
				characters[28].Attributes = color ? colors::numBlackExtraMoves : colors::numWhiteExtraMoves;
				characters[29].Attributes = color ? colors::numBlackExtraMoves : colors::numWhiteExtraMoves;
				characters[30].Attributes = color ? colors::numBlackExtraMoves : colors::numWhiteExtraMoves;
				characters[31].Attributes = color ? colors::numBlackExtraMoves : colors::numWhiteExtraMoves;
			}


			/* -- Move on -- */

			characters += width / 2;
		}
	}

	/* -- Output to screen -- */

	COORD size = { (short)width, 16 };
	COORD zero = { 0, 0 };
	SMALL_RECT window = { 0, 8, (short)(width - 1), (short)(height - 1) };

	WriteConsoleOutput(output, this->characters, size, zero, &window);

	Console::displayDeductions(deductions);
}

/* -------------------------------------------------------------------------- */

void WinConsole::write(const wchar_t *string, int x, int y, unsigned color)
{
	DWORD written;
	DWORD length = (DWORD)wcslen(string);

	COORD position = { (SHORT)x, (SHORT)y };

	SetConsoleTextAttribute(output, (WORD)color);
	SetConsoleCursorPosition(output, position);
	WriteConsole(output, string, length, &written, NULL);

	Console::write(string, x, y, color);
}

/* -------------------------------------------------------------------------- */

void WinConsole::write(const wchar_t *string, int maxLength, bool fillWithBlanks, int x, int y, unsigned color)
{
	Console::write(string, maxLength, fillWithBlanks, x, y, color);
}

/* -------------------------------------------------------------------------- */
