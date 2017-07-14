#include "console-linux.h"
#include <locale.h>

/* -------------------------------------------------------------------------- */

LinuxConsole::LinuxConsole(const Strings& strings)
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

	width = getmaxx(stdscr);
	height = getmaxy(stdscr);

	if ((width < 64) || (height < 25))
		return;

	/* -- Clear input & output -- */

	valid = true;
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
	wchar_t *blank = new wchar_t[width + 1];

	std::fill_n(blank, width, ' ');
	blank[width] = '\0';

	for (int y = 0; y < height; y++)
		write(blank, 0, y, colors::standard);

	delete[] blank;
	Console::clear();
}

/* -------------------------------------------------------------------------- */

bool LinuxConsole::wait()
{
	Console::wait();

	if (getch() == 0x1B)
		abort = true;

	write(L"", width - 1, true, 0, height - 1, colors::standard);
	return !abort;
}

/* -------------------------------------------------------------------------- */

void LinuxConsole::displayProblem(const EUCLIDE_Problem *problem)
{
	const wchar_t *glyphs = strings::load(strings::GlyphSymbols);

	for (int square = 0; square < 64; square++)
	{
		EUCLIDE_Glyph glyph = problem->glyphs[square];

		bool isWhiteGlyph = ((glyph == EUCLIDE_GLYPH_WHITE_KING) || (glyph == EUCLIDE_GLYPH_WHITE_QUEEN) || (glyph == EUCLIDE_GLYPH_WHITE_ROOK) || (glyph == EUCLIDE_GLYPH_WHITE_BISHOP) || (glyph == EUCLIDE_GLYPH_WHITE_KNIGHT) || (glyph == EUCLIDE_GLYPH_WHITE_PAWN));
		bool isLightSquare = ((square / 8) & 1) == ((square % 8) & 1);

		move(7 - (square % 8), square / 8);
		addch(toupper(glyphs[glyph]) | (isWhiteGlyph ? (isLightSquare ? colors::whitePiecesOnLightSquares : colors::whitePiecesOnDarkSquares) : (isLightSquare ? colors::blackPiecesOnLightSquares : colors::blackPiecesOnDarkSquares)));
	}

	Console::displayProblem(problem);
}

/* -------------------------------------------------------------------------- */

void LinuxConsole::displayDeductions(const EUCLIDE_Deductions *deductions)
{
	const wchar_t *symbols = strings::load(strings::GlyphSymbols);

	for (int piece = 0; piece < 16; piece++)
	{
		for (int color = 0; color <= 1; color++)
		{
			const EUCLIDE_Deduction *deduction = color ? &deductions->blackPieces[piece] : &deductions->whitePieces[piece];
			int x = color * width / 2;
			int y = 8 + piece;

			/* - Clear area -- */

			move(y, x);
			for (int k = 0; k < width / 2; k++)
				addch(' ' | colors::standard);

			/* -- Print required moves -- */

			if (deduction->requiredMoves > 10)
				mvaddch(y, x + 1, ('0' + deduction->requiredMoves / 10) | (color ? colors::numBlackMoves : colors::numWhiteMoves));

			if (deduction->requiredMoves > 0)
				mvaddch(y, x + 2, ('0' + deduction->requiredMoves % 10) | (color ? colors::numBlackMoves : colors::numWhiteMoves));

			/* -- Print deduction -- */

			if (deduction->finalSquare >= 0)
			{
				unsigned attribute =  color ? (deduction->captured ? colors::blackCaptures : colors::blackMoves) : (deduction->captured ? colors::whiteCaptures : colors::whiteMoves);

				mvaddch(y, x + 5, toupper(symbols[deduction->initialGlyph]) | attribute);
				mvaddch(y, x + 6, ('a' + deduction->initialSquare / 8) | attribute);
				mvaddch(y, x + 7, ('1' + deduction->initialSquare % 8) | attribute);

				if ((deduction->finalSquare != deduction->initialSquare) || (deduction->requiredMoves > 0))
				{
					mvaddch(y, x + 9, '-' | attribute);
					mvaddch(y, x + 10, '>' | attribute);
					mvaddch(y, x + 12, toupper(symbols[deduction->promotionGlyph]) | attribute);
					mvaddch(y, x + 13, ('a' + deduction->finalSquare / 8) | attribute);
					mvaddch(y, x + 14, ('1' + deduction->finalSquare % 8) | attribute);
				}
			}

			/* -- Print number of possible squares -- */

			if (deduction->numSquares > 1)
				for (int k = 0; k < 4; k++)
					if (deduction->numSquares >= pow(10, k))
						mvaddch(y, x + 24 - k, ('0' + (deduction->numSquares / (int)pow(10, k)) % 10) | (color ? colors::numBlackSquares : colors::numWhiteSquares));

			/* -- Print number of possible moves -- */

			int numExtraMoves = deduction->numMoves - deduction->requiredMoves;
			for (int k = 0; k < 4; k++)
				if (numExtraMoves >= pow(10, k))
					mvaddch(y, x + 31 - k, ('0' + (numExtraMoves / (int)pow(10, k)) % 10) | (color ? colors::numBlackExtraMoves : colors::numWhiteExtraMoves));
		}
	}

	Console::displayDeductions(deductions);
}

/* -------------------------------------------------------------------------- */

void LinuxConsole::write(const wchar_t *string, int x, int y, unsigned color)
{
	attrset(color);
	mvaddwstr(y, x, string);
	refresh();

	Console::write(string, x, y, color);
}

/* -------------------------------------------------------------------------- */

void LinuxConsole::write(const wchar_t *string, int maxLength, bool fillWithBlanks, int x, int y, unsigned color)
{
	Console::write(string, maxLength, fillWithBlanks, x, y, color);
}

/* -------------------------------------------------------------------------- */
