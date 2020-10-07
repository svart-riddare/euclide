#ifndef __COLORS_H
#define __COLORS_H

#include "includes.h"

namespace Colors
{

/* -------------------------------------------------------------------------- */

typedef enum
{

#ifdef EUCLIDE_WINDOWS
	Timer = FOREGROUND_GREEN,
	Error = FOREGROUND_RED | FOREGROUND_INTENSITY,
	Standard = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	Question = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	Copyright = FOREGROUND_RED | FOREGROUND_BLUE,
	FreeMoves = FOREGROUND_GREEN,
	Complexity = FOREGROUND_GREEN,
	Verdict = FOREGROUND_BLUE,
	Thinking = FOREGROUND_RED | FOREGROUND_GREEN  | FOREGROUND_BLUE,

	WhitePieces = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	BlackPieces = 0,
	LightSquares = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
	DarkSquares = BACKGROUND_RED | BACKGROUND_BLUE,

	WhiteMoves = FOREGROUND_RED | FOREGROUND_GREEN  | FOREGROUND_BLUE,
	WhiteCaptures = FOREGROUND_RED | FOREGROUND_GREEN,
	BlackMoves = FOREGROUND_RED | FOREGROUND_GREEN  | FOREGROUND_BLUE,
	BlackCaptures = FOREGROUND_RED | FOREGROUND_GREEN,

	NumWhiteMoves = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	NumWhiteSquares = FOREGROUND_RED,
	NumWhiteExtraMoves = FOREGROUND_RED,
	NumBlackMoves = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	NumBlackSquares = FOREGROUND_RED,
	NumBlackExtraMoves = FOREGROUND_RED,
#else
	Timer = COLOR_PAIR(COLOR_GREEN),
	Error = COLOR_PAIR(COLOR_RED) | A_BOLD,
	Standard = COLOR_PAIR(COLOR_WHITE) | A_BOLD,
	Question = COLOR_PAIR(COLOR_WHITE),
	Copyright = COLOR_PAIR(COLOR_CYAN),
	FreeMoves = COLOR_PAIR(COLOR_GREEN),
	Complexity = COLOR_PAIR(COLOR_GREEN),
	Verdict = COLOR_PAIR(COLOR_BLUE),
	Thinking = COLOR_PAIR(COLOR_WHITE),

	WhitePiecesOnLightSquares = COLOR_PAIR(12) | A_BOLD,
	WhitePiecesOnDarkSquares = COLOR_PAIR(13) | A_BOLD,
	BlackPiecesOnLightSquares = COLOR_PAIR(14) | A_BOLD,
	BlackPiecesOnDarkSquares = COLOR_PAIR(15) | A_BOLD,

	WhiteMoves = COLOR_PAIR(COLOR_WHITE),
	WhiteCaptures = COLOR_PAIR(COLOR_YELLOW),
	BlackMoves = COLOR_PAIR(COLOR_WHITE),
	BlackCaptures = COLOR_PAIR(COLOR_YELLOW),

	NumWhiteMoves = COLOR_PAIR(COLOR_WHITE) | A_BOLD,
	NumWhiteSquares = COLOR_PAIR(COLOR_RED),
	NumWhiteExtraMoves = COLOR_PAIR(COLOR_RED),
	NumBlackMoves = COLOR_PAIR(COLOR_WHITE) | A_BOLD,
	NumBlackSquares = COLOR_PAIR(COLOR_RED),
	NumBlackExtraMoves = COLOR_PAIR(COLOR_RED),
#endif

} Color;

/* -------------------------------------------------------------------------- */

}

using Colors::Color;

#endif
