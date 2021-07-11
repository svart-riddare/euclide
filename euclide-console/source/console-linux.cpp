#include "console-linux.h"
#include <locale.h>

/* -------------------------------------------------------------------------- */

LinuxConsole::LinuxConsole(const Strings& strings)
	: Console(strings)
{
	setlocale(LC_ALL, "");

	/* -- Initialize ncurses library -- */

	initscr();
	start_color();

	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_BLUE, COLOR_BLACK);
	init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(6, COLOR_CYAN, COLOR_BLACK);
	init_pair(7, COLOR_WHITE, COLOR_BLACK);

	init_pair(12, COLOR_WHITE, COLOR_WHITE);
	init_pair(13, COLOR_WHITE, COLOR_CYAN);
	init_pair(14, COLOR_BLACK, COLOR_WHITE);
	init_pair(15, COLOR_BLACK, COLOR_CYAN);

	curs_set(0);
	cbreak();
	noecho();

	/* -- Get console width & height -- */

	m_width = getmaxx(stdscr);
	m_height = getmaxy(stdscr);

	if ((m_width < 64) || (m_height < 25))
		return;

	/* -- Clear input & output -- */

	m_valid = true;
	clear();
}

/* -------------------------------------------------------------------------- */

LinuxConsole::~LinuxConsole()
{
	/* -- Close ncurses library -- */

	endwin();
}

/* -------------------------------------------------------------------------- */

void LinuxConsole::clear()
{
	const std::wstring blank(m_width, ' ');

	for (int y = 0; y < m_height; y++)
		write(blank.c_str(), 0, y, Colors::Standard);

	Console::clear();
}

/* -------------------------------------------------------------------------- */

bool LinuxConsole::wait()
{
	Console::wait();

	if (getch() == 0x1B)
		m_abort = true;

	write(L"", m_width - 1, true, 0, m_height - 1, Colors::Standard);
	return !m_abort;
}

/* -------------------------------------------------------------------------- */

void LinuxConsole::displayProblem(const EUCLIDE_Problem& problem) const
{
	const wchar_t *glyphs = m_strings[Strings::GlyphSymbols];

	for (int square = 0; square < 64; square++)
	{
		const EUCLIDE_Glyph glyph = problem.diagram[square];

		const bool isWhiteGlyph = ((glyph == EUCLIDE_GLYPH_WHITE_KING) || (glyph == EUCLIDE_GLYPH_WHITE_QUEEN) || (glyph == EUCLIDE_GLYPH_WHITE_ROOK) || (glyph == EUCLIDE_GLYPH_WHITE_BISHOP) || (glyph == EUCLIDE_GLYPH_WHITE_KNIGHT) || (glyph == EUCLIDE_GLYPH_WHITE_PAWN));
		const bool isLightSquare = ((square / 8) & 1) == ((square % 8) & 1);

		move(7 - (square % 8), square / 8);
		addch(toupper(glyphs[glyph]) | (isWhiteGlyph ? (isLightSquare ? Colors::WhitePiecesOnLightSquares : Colors::WhitePiecesOnDarkSquares) : (isLightSquare ? Colors::BlackPiecesOnLightSquares : Colors::BlackPiecesOnDarkSquares)));
	}

	Console::displayProblem(problem);
}

/* -------------------------------------------------------------------------- */

void LinuxConsole::displayDeductions(const EUCLIDE_Deductions& deductions) const
{
	const wchar_t *symbols = m_strings[Strings::GlyphSymbols];

	for (int piece = 0; piece < 16; piece++)
	{
		for (int color = 0; color <= 1; color++)
		{
			const EUCLIDE_Deduction& deduction = color ? deductions.blackPieces[piece] : deductions.whitePieces[piece];
			const int x = color * m_width / 2;
			const int y = 8 + piece;

			/* - Clear area -- */

			move(y, x);
			for (int k = 0; k < m_width / 2; k++)
				addch(' ' | Colors::Standard);

			/* -- Early return if there was not 16 pieces in the initial position -- */

			if (piece >= (color ? deductions.numBlackPieces : deductions.numWhitePieces))
				continue;

			/* -- Print required moves -- */

			if (deduction.requiredMoves > 10)
				mvaddch(y, x + 1, ('0' + deduction.requiredMoves / 10) | (color ? Colors::NumBlackMoves : Colors::NumWhiteMoves));

			if (deduction.requiredMoves > 0)
				mvaddch(y, x + 2, ('0' + deduction.requiredMoves % 10) | (color ? Colors::NumBlackMoves : Colors::NumWhiteMoves));

			/* -- Print deduction -- */

			if (deduction.final.square >= 0)
			{
				const unsigned attribute =  color ? (deduction.captured ? Colors::BlackCaptures : Colors::BlackMoves) : (deduction.captured ? Colors::WhiteCaptures : Colors::WhiteMoves);

				mvaddch(y, x + 5, toupper(symbols[deduction.initial.glyph]) | attribute);
				mvaddch(y, x + 6, ('a' + deduction.initial.square / 8) | attribute);
				mvaddch(y, x + 7, ('1' + deduction.initial.square % 8) | attribute);

				if ((deduction.final.square != deduction.initial.square) || (deduction.requiredMoves > 0))
				{
					mvaddch(y, x + 9, '-' | attribute);
					mvaddch(y, x + 10, '>' | attribute);
					mvaddch(y, x + 12, toupper(symbols[deduction.final.glyph]) | attribute);
					mvaddch(y, x + 13, ('a' + deduction.final.square / 8) | attribute);
					mvaddch(y, x + 14, ('1' + deduction.final.square % 8) | attribute);
				}

				if (deduction.captured && (deduction.capturer.square >= 0))
				{
					mvaddch(y, x + 16, '(' | attribute);
					mvaddch(y, x + 17, toupper(symbols[deduction.capturer.glyph]) | attribute);
					mvaddch(y, x + 18, ('a' + deduction.capturer.square / 8) | attribute);
					mvaddch(y, x + 19, ('1' + deduction.capturer.square % 8) | attribute);
					mvaddch(y, x + 20, ')' | attribute);
				}
			}

			/* -- Print number of possible squares -- */

			if (deduction.numSquares > 1)
				for (int k = 0; k < 4; k++)
					if (deduction.numSquares >= std::pow(10, k))
						mvaddch(y, x + 24 - k, ('0' + (deduction.numSquares / (int)std::pow(10, k)) % 10) | (color ? Colors::NumBlackSquares : Colors::NumWhiteSquares));

			/* -- Print number of possible moves -- */

			const int numExtraMoves = deduction.numMoves - deduction.requiredMoves;
			for (int k = 0; k < 4; k++)
				if (numExtraMoves >= std::pow(10, k))
					mvaddch(y, x + 31 - k, ('0' + (numExtraMoves / (int)std::pow(10, k)) % 10) | (color ? Colors::NumBlackExtraMoves : Colors::NumWhiteExtraMoves));
		}
	}

	Console::displayDeductions(deductions);
}

/* -------------------------------------------------------------------------- */

void LinuxConsole::write(const wchar_t *string, int x, int y, Color color) const
{
	attrset(color);
	mvaddwstr(y, x, string);
	refresh();

	Console::write(string, x, y, color);
}

/* -------------------------------------------------------------------------- */

void LinuxConsole::write(const wchar_t *string, int maxLength, bool fillWithBlanks, int x, int y, Color color) const
{
	Console::write(string, maxLength, fillWithBlanks, x, y, color);
}

/* -------------------------------------------------------------------------- */
