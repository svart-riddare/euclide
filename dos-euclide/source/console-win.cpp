#include "console-win.h"

/* -------------------------------------------------------------------------- */

WinConsole::WinConsole(const Strings& strings)
	: Console(strings)
{
	/* -- Initialize console handles -- */

	_output = GetStdHandle(STD_OUTPUT_HANDLE);
	_input = GetStdHandle(STD_INPUT_HANDLE);

	if ((_output == INVALID_HANDLE_VALUE) || (_input == INVALID_HANDLE_VALUE))
		return;

	GetConsoleScreenBufferInfo(_output, &_initialState);
	_width = 2 * (std::min<int>(_initialState.dwSize.X, 120) / 2);
	_height = std::min<int>(_initialState.dwSize.Y, 25);

	if ((_width < 64) || (_height < 25))
		return;

	GetConsoleMode(_output, &_initialOutputMode);
	GetConsoleMode(_input, &_initialInputMode);
	SetConsoleMode(_input, ENABLE_PROCESSED_INPUT);
		
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(_output, &cursorInfo);
	_initialCursorVisibility = cursorInfo.bVisible;
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(_output, &cursorInfo);

	const SMALL_RECT window = { 0, 0, _width - 1, _height - 1};
	if ((_initialState.srWindow.Bottom < window.Bottom) || (_initialState.srWindow.Right < window.Right) || (_initialState.srWindow.Left > window.Left) || (_initialState.srWindow.Top > window.Top))
		SetConsoleWindowInfo(_output, TRUE, &window);

	/* -- Allocate output buffer -- */

	_characters = new CHAR_INFO[16 * _width];

	/* -- Clear input & output -- */

	_valid = true;
	clear();

	FlushConsoleInputBuffer(_input);
}

/* -------------------------------------------------------------------------- */

WinConsole::~WinConsole()
{
	/* -- Restore console output -- */

	COORD cursor = { 0, 8 };
	SetConsoleCursorPosition(_output, cursor);

	const int width = _initialState.dwSize.X;
	const int height = _initialState.dwSize.Y;

	DWORD written;
	FillConsoleOutputAttribute(_output, _initialState.wAttributes, width * (height - 8), cursor, &written);
	FillConsoleOutputCharacter(_output, ' ', width * (height - 8), cursor, &written);

	cursor.Y++;

	SetConsoleWindowInfo(_output, TRUE, &_initialState.srWindow);
	SetConsoleTextAttribute(_output, _initialState.wAttributes);
	SetConsoleCursorPosition(_output, cursor);

	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(_output, &cursorInfo);
	cursorInfo.bVisible = _initialCursorVisibility;
	SetConsoleCursorInfo(_output, &cursorInfo);

	SetConsoleMode(_output, _initialOutputMode);
	SetConsoleMode(_input, _initialInputMode);

	/* -- Delete allocated resources -- */

	delete[] _characters;
}

/* -------------------------------------------------------------------------- */

void WinConsole::clear()
{
	COORD cursor = { 0, 0 };
	SetConsoleCursorPosition(_output, cursor);

	const int width = _initialState.dwSize.X;
	const int height = _initialState.dwSize.Y;

	DWORD written;
	FillConsoleOutputCharacter(_output, ' ', width * height, cursor, &written);
	FillConsoleOutputAttribute(_output, Colors::Standard, width * height, cursor, &written);

	SetConsoleTextAttribute(_output, _initialState.wAttributes);
	SetConsoleCursorPosition(_output, cursor);

	Console::clear();
}

/* -------------------------------------------------------------------------- */

bool WinConsole::wait()
{
	Console::wait();

	FlushConsoleInputBuffer(_input);

	DWORD records = 0;
	INPUT_RECORD record;
	ZeroMemory(&record, sizeof(record));
	
	record.EventType = 0;
	while (record.EventType != KEY_EVENT)
		if (!ReadConsoleInput(_input, &record, 1, &records))
			return _abort = true, false;
	
	if (record.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
		_abort = true;

	write(L"", _width - 1, true, 0, _height - 1, Colors::Standard);
	return !_abort;
}

/* -------------------------------------------------------------------------- */

void WinConsole::displayProblem(const EUCLIDE_Problem& problem) const
{
	const wchar_t *glyphs = _strings[Strings::GlyphSymbols];
	CHAR_INFO chessboard[64];

	for (int square = 0; square < 64; square++)
	{
		const EUCLIDE_Glyph glyph = problem.diagram[square];

		const bool isWhiteGlyph = ((glyph == EUCLIDE_GLYPH_WHITE_KING) || (glyph == EUCLIDE_GLYPH_WHITE_QUEEN) || (glyph == EUCLIDE_GLYPH_WHITE_ROOK) || (glyph == EUCLIDE_GLYPH_WHITE_BISHOP) || (glyph == EUCLIDE_GLYPH_WHITE_KNIGHT) || (glyph == EUCLIDE_GLYPH_WHITE_PAWN));

		const int column = square / 8;
		const int row = square % 8;
		const int index = 8 * (7 - row) + column;
		
		chessboard[index].Char.UnicodeChar = toupper(glyphs[glyph]);
		chessboard[index].Attributes = (((column & 1) ^ (row & 1)) ? Colors::LightSquares : Colors::DarkSquares);
		chessboard[index].Attributes |= (isWhiteGlyph ? Colors::WhitePieces : Colors::BlackPieces);
	}

	const COORD size = { 8, 8 };
	const COORD corner = { 0, 0 };
	SMALL_RECT window = { 0, 0, 7, 7 };
	WriteConsoleOutput(_output, chessboard, size, corner, &window);

	Console::displayProblem(problem);
}

/* -------------------------------------------------------------------------- */

void WinConsole::displayDeductions(const EUCLIDE_Deductions& deductions) const
{
	const wchar_t *symbols = _strings[Strings::GlyphSymbols];
	CHAR_INFO *characters = this->_characters;
	
	for (int piece = 0; piece < 16; piece++)
	{
		for (int color = 0; color <= 1; color++)
		{
			const EUCLIDE_Deduction *deduction = color ? &deductions.blackPieces[piece] : &deductions.whitePieces[piece];

			/* -- Set default attributes and clear area -- */

			for (int k = 0; k < _width / 2; k++)
			{
				characters[k].Attributes = color ? (deduction->captured ? Colors::BlackCaptures : Colors::BlackMoves) : (deduction->captured ? Colors::WhiteCaptures : Colors::WhiteMoves);
				characters[k].Char.UnicodeChar = ' ';
			}

			/* -- Print required moves -- */

			if (deduction->requiredMoves > 0)
			{
				characters[1].Attributes = color ? Colors::NumBlackMoves : Colors::NumWhiteMoves;
				characters[2].Attributes = color ? Colors::NumBlackMoves : Colors::NumWhiteMoves;

				if (deduction->requiredMoves > 9)
					characters[1].Char.UnicodeChar = '0' + (deduction->requiredMoves / 10 % 10);
				
				characters[2].Char.UnicodeChar = '0' + deduction->requiredMoves % 10;
			}

			/* -- Print deduction -- */

			if (deduction->finalSquare >= 0)
			{
				characters[5].Char.UnicodeChar = toupper(symbols[deduction->initialGlyph]);
				characters[6].Char.UnicodeChar = 'a' + (deduction->initialSquare / 8);
				characters[7].Char.UnicodeChar = '1' + (deduction->initialSquare % 8);

				if ((deduction->finalSquare != deduction->initialSquare) || (deduction->requiredMoves > 0))
				{
					characters[9].Char.UnicodeChar = '-';
					characters[10].Char.UnicodeChar = '>';

					characters[12].Char.UnicodeChar = toupper(symbols[deduction->promotionGlyph]);
					characters[13].Char.UnicodeChar = 'a' + (deduction->finalSquare / 8);
					characters[14].Char.UnicodeChar = '1' + (deduction->finalSquare % 8);
				}
			}

			/* -- Print number of possible squares -- */

			if (deduction->numSquares > 1)
			{
				if (deduction->numSquares >= 10000)
					characters[20].Char.UnicodeChar = '0' + ((deduction->numSquares / 10000) % 10);
				if (deduction->numSquares >= 1000)
					characters[21].Char.UnicodeChar = '0' + ((deduction->numSquares / 1000) % 10);
				if (deduction->numSquares >= 100)
					characters[22].Char.UnicodeChar = '0' + ((deduction->numSquares / 100) % 10);
				if (deduction->numSquares >= 10)
					characters[23].Char.UnicodeChar = '0' + ((deduction->numSquares / 10) % 10);
				if (deduction->numSquares >= 1)
					characters[24].Char.UnicodeChar = '0' + ((deduction->numSquares / 1) % 10);

				characters[20].Attributes = color ? Colors::NumBlackSquares : Colors::NumWhiteSquares;
				characters[21].Attributes = color ? Colors::NumBlackSquares : Colors::NumWhiteSquares;
				characters[22].Attributes = color ? Colors::NumBlackSquares : Colors::NumWhiteSquares;
				characters[23].Attributes = color ? Colors::NumBlackSquares : Colors::NumWhiteSquares;
				characters[24].Attributes = color ? Colors::NumBlackSquares : Colors::NumWhiteSquares;
			}

			/* -- Print number of possible moves -- */

			if (deduction->requiredMoves < deduction->numMoves)
			{
				int numExtraMoves = deduction->numMoves - deduction->requiredMoves;

				if (numExtraMoves >= 10000)
					characters[27].Char.UnicodeChar = '0' + ((numExtraMoves / 10000) % 10);
				if (numExtraMoves >= 1000)
					characters[28].Char.UnicodeChar = '0' + ((numExtraMoves / 1000) % 10);
				if (numExtraMoves >= 100)
					characters[29].Char.UnicodeChar = '0' + ((numExtraMoves / 100) % 10);
				if (numExtraMoves >= 10)
					characters[30].Char.UnicodeChar = '0' + ((numExtraMoves / 10) % 10);
				if (numExtraMoves >= 1)
					characters[31].Char.UnicodeChar = '0' + ((numExtraMoves / 1) % 10);

				characters[27].Attributes = color ? Colors::NumBlackExtraMoves : Colors::NumWhiteExtraMoves;
				characters[28].Attributes = color ? Colors::NumBlackExtraMoves : Colors::NumWhiteExtraMoves;
				characters[29].Attributes = color ? Colors::NumBlackExtraMoves : Colors::NumWhiteExtraMoves;
				characters[30].Attributes = color ? Colors::NumBlackExtraMoves : Colors::NumWhiteExtraMoves;
				characters[31].Attributes = color ? Colors::NumBlackExtraMoves : Colors::NumWhiteExtraMoves;
			}


			/* -- Move on -- */

			characters += _width / 2;
		}
	}

	/* -- Output to screen -- */

	const COORD size = { _width, 16 };
	const COORD zero = { 0, 0 };
	SMALL_RECT window = { 0, 8, _width - 1, _height - 1 };
	WriteConsoleOutput(_output, _characters, size, zero, &window);

	Console::displayDeductions(deductions);
}

/* -------------------------------------------------------------------------- */

void WinConsole::write(const wchar_t *string, int x, int y, Color color) const
{
	DWORD written = 0;
	const COORD position = { x, y };

	SetConsoleTextAttribute(_output, color);
	SetConsoleCursorPosition(_output, position);
	WriteConsole(_output, string, wcslen(string), &written, NULL);

	Console::write(string, x, y, color);
}

/* -------------------------------------------------------------------------- */

void WinConsole::write(const wchar_t *string, int maxLength, bool fillWithBlanks, int x, int y, Color color) const
{
	Console::write(string, maxLength, fillWithBlanks, x, y, color);
}

/* -------------------------------------------------------------------------- */
