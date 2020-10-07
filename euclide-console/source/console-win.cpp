#include "console-win.h"

/* -------------------------------------------------------------------------- */

WinConsole::WinConsole(const Strings& strings)
	: Console(strings), m_characters(nullptr)
{
	/* -- Initialize console handles -- */

	m_output = GetStdHandle(STD_OUTPUT_HANDLE);
	m_input = GetStdHandle(STD_INPUT_HANDLE);

	if ((m_output == INVALID_HANDLE_VALUE) || (m_input == INVALID_HANDLE_VALUE))
		return;

	if (!GetConsoleScreenBufferInfo(m_output, &m_initialState))
		return;

	m_width = 2 * (std::min<int>(m_initialState.dwSize.X, 120) / 2);
	m_height = std::min<int>(m_initialState.dwSize.Y, 25);

	if ((m_width < 64) || (m_height < 25))
		return;

	GetConsoleMode(m_output, &m_initialOutputMode);
	GetConsoleMode(m_input, &m_initialInputMode);
	SetConsoleMode(m_input, ENABLE_PROCESSED_INPUT);

	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(m_output, &cursorInfo);
	m_initialCursorVisibility = cursorInfo.bVisible;
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(m_output, &cursorInfo);

	const SMALL_RECT window = { 0, 0, m_width - 1, m_height - 1};
	if ((m_initialState.srWindow.Bottom < window.Bottom) || (m_initialState.srWindow.Right < window.Right) || (m_initialState.srWindow.Left > window.Left) || (m_initialState.srWindow.Top > window.Top))
		SetConsoleWindowInfo(m_output, TRUE, &window);

	/* -- Allocate output buffer -- */

	m_characters = new CHAR_INFO[16 * m_width];

	/* -- Clear input & output -- */

	m_valid = true;
	clear();

	FlushConsoleInputBuffer(m_input);
}

/* -------------------------------------------------------------------------- */

WinConsole::~WinConsole()
{
	/* -- Restore console output -- */

	COORD cursor = { 0, 8 };
	SetConsoleCursorPosition(m_output, cursor);

	const int width = m_initialState.dwSize.X;
	const int height = m_initialState.dwSize.Y;

	DWORD written;
	FillConsoleOutputAttribute(m_output, m_initialState.wAttributes, width * (height - 8), cursor, &written);
	FillConsoleOutputCharacter(m_output, ' ', width * (height - 8), cursor, &written);

	cursor.Y++;

	SetConsoleWindowInfo(m_output, TRUE, &m_initialState.srWindow);
	SetConsoleTextAttribute(m_output, m_initialState.wAttributes);
	SetConsoleCursorPosition(m_output, cursor);

	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(m_output, &cursorInfo);
	cursorInfo.bVisible = m_initialCursorVisibility;
	SetConsoleCursorInfo(m_output, &cursorInfo);

	SetConsoleMode(m_output, m_initialOutputMode);
	SetConsoleMode(m_input, m_initialInputMode);

	/* -- Delete allocated resources -- */

	delete[] m_characters;
}

/* -------------------------------------------------------------------------- */

void WinConsole::clear()
{
	COORD cursor = { 0, 0 };
	SetConsoleCursorPosition(m_output, cursor);

	const int width = m_initialState.dwSize.X;
	const int height = m_initialState.dwSize.Y;

	DWORD written;
	FillConsoleOutputCharacter(m_output, ' ', width * height, cursor, &written);
	FillConsoleOutputAttribute(m_output, Colors::Standard, width * height, cursor, &written);

	SetConsoleTextAttribute(m_output, m_initialState.wAttributes);
	SetConsoleCursorPosition(m_output, cursor);

	Console::clear();
}

/* -------------------------------------------------------------------------- */

bool WinConsole::wait()
{
	Console::wait();

	FlushConsoleInputBuffer(m_input);

	DWORD records = 0;
	INPUT_RECORD record;
	ZeroMemory(&record, sizeof(record));

	record.EventType = 0;
	while ((record.EventType != KEY_EVENT) || !record.Event.KeyEvent.bKeyDown)
		if (!ReadConsoleInput(m_input, &record, 1, &records))
			return m_abort = true, false;

	if (record.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
		m_abort = true;

	write(L"", m_width - 1, true, 0, m_height - 1, Colors::Standard);
	return !m_abort;
}

/* -------------------------------------------------------------------------- */

void WinConsole::displayProblem(const EUCLIDE_Problem& problem) const
{
	const wchar_t *glyphs = m_strings[Strings::GlyphSymbols];
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
	WriteConsoleOutput(m_output, chessboard, size, corner, &window);

	Console::displayProblem(problem);
}

/* -------------------------------------------------------------------------- */

void WinConsole::displayDeductions(const EUCLIDE_Deductions& deductions) const
{
	const wchar_t *symbols = m_strings[Strings::GlyphSymbols];
	CHAR_INFO *characters = this->m_characters;

	for (int piece = 0; piece < 16; piece++)
	{
		for (int color = 0; color <= 1; color++)
		{
			const EUCLIDE_Deduction& deduction = color ? deductions.blackPieces[piece] : deductions.whitePieces[piece];

			/* -- Set default attributes and clear area -- */

			for (int k = 0; k < m_width / 2; k++)
			{
				characters[k].Attributes = color ? (deduction.captured ? Colors::BlackCaptures : Colors::BlackMoves) : (deduction.captured ? Colors::WhiteCaptures : Colors::WhiteMoves);
				characters[k].Char.UnicodeChar = ' ';
			}

			/* -- Print required moves -- */

			if (deduction.requiredMoves > 0)
			{
				characters[1].Attributes = color ? Colors::NumBlackMoves : Colors::NumWhiteMoves;
				characters[2].Attributes = color ? Colors::NumBlackMoves : Colors::NumWhiteMoves;

				if (deduction.requiredMoves > 9)
					characters[1].Char.UnicodeChar = '0' + (deduction.requiredMoves / 10 % 10);

				characters[2].Char.UnicodeChar = '0' + deduction.requiredMoves % 10;
			}

			/* -- Print deduction -- */

			if (deduction.final.square >= 0)
			{
				characters[5].Char.UnicodeChar = toupper(symbols[deduction.initial.glyph]);
				characters[6].Char.UnicodeChar = 'a' + (deduction.initial.square / 8);
				characters[7].Char.UnicodeChar = '1' + (deduction.initial.square % 8);

				if ((deduction.final.square != deduction.initial.square) || (deduction.requiredMoves > 0))
				{
					characters[9].Char.UnicodeChar = '-';
					characters[10].Char.UnicodeChar = '>';

					characters[12].Char.UnicodeChar = toupper(symbols[deduction.final.glyph]);
					characters[13].Char.UnicodeChar = 'a' + (deduction.final.square / 8);
					characters[14].Char.UnicodeChar = '1' + (deduction.final.square % 8);
				}

				if (deduction.captured && (deduction.capturer.square >= 0))
				{
					characters[16].Char.UnicodeChar = '(';
					characters[17].Char.UnicodeChar = toupper(symbols[deduction.capturer.glyph]);
					characters[18].Char.UnicodeChar = 'a' + (deduction.capturer.square / 8);
					characters[19].Char.UnicodeChar = '1' + (deduction.capturer.square % 8);
					characters[20].Char.UnicodeChar = ')';
				}
			}

			/* -- Print number of possible squares -- */

			if (deduction.numSquares > 1)
			{
				if (deduction.numSquares >= 100)
					characters[22].Char.UnicodeChar = '0' + ((deduction.numSquares / 100) % 10);
				if (deduction.numSquares >= 10)
					characters[23].Char.UnicodeChar = '0' + ((deduction.numSquares / 10) % 10);
				if (deduction.numSquares >= 1)
					characters[24].Char.UnicodeChar = '0' + ((deduction.numSquares / 1) % 10);

				characters[22].Attributes = color ? Colors::NumBlackSquares : Colors::NumWhiteSquares;
				characters[23].Attributes = color ? Colors::NumBlackSquares : Colors::NumWhiteSquares;
				characters[24].Attributes = color ? Colors::NumBlackSquares : Colors::NumWhiteSquares;
			}

			/* -- Print number of possible moves -- */

			if (deduction.requiredMoves < deduction.numMoves)
			{
				int numExtraMoves = deduction.numMoves - deduction.requiredMoves;

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

			characters += m_width / 2;
		}
	}

	/* -- Output to screen -- */

	const COORD size = { m_width, 16 };
	const COORD zero = { 0, 0 };
	SMALL_RECT window = { 0, 8, m_width - 1, m_height - 1 };
	WriteConsoleOutput(m_output, m_characters, size, zero, &window);

	Console::displayDeductions(deductions);
}

/* -------------------------------------------------------------------------- */

void WinConsole::write(const wchar_t *string, int x, int y, Color color) const
{
	DWORD written = 0;
	const COORD position = { x, y };

	SetConsoleTextAttribute(m_output, color);
	SetConsoleCursorPosition(m_output, position);
	WriteConsole(m_output, string, wcslen(string), &written, NULL);

	Console::write(string, x, y, color);
}

/* -------------------------------------------------------------------------- */

void WinConsole::write(const wchar_t *string, int maxLength, bool fillWithBlanks, int x, int y, Color color) const
{
	Console::write(string, maxLength, fillWithBlanks, x, y, color);
}

/* -------------------------------------------------------------------------- */
