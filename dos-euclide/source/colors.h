#ifndef __COLORS_H
#define __COLORS_H

#include "includes.h"

namespace colors
{

/* -------------------------------------------------------------------------- */

#ifndef EUCLIDE_LINUX
static const unsigned timer = FOREGROUND_GREEN;
static const unsigned error = FOREGROUND_RED | FOREGROUND_INTENSITY;
static const unsigned standard = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
static const unsigned question = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
static const unsigned copyright = FOREGROUND_RED | FOREGROUND_BLUE;
static const unsigned freeMoves = FOREGROUND_GREEN;

static const unsigned whitePieces = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
static const unsigned blackPieces = 0;
static const unsigned lightSquares = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
static const unsigned darkSquares = BACKGROUND_RED | BACKGROUND_BLUE;

static const unsigned whiteMoves = FOREGROUND_RED | FOREGROUND_GREEN  | FOREGROUND_BLUE;
static const unsigned whiteCaptures = FOREGROUND_RED | FOREGROUND_GREEN;
static const unsigned blackMoves = FOREGROUND_RED | FOREGROUND_GREEN  | FOREGROUND_BLUE;
static const unsigned blackCaptures = FOREGROUND_RED | FOREGROUND_GREEN;

static const unsigned numWhiteMoves = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
static const unsigned numWhiteSquares = FOREGROUND_RED;
static const unsigned numWhiteExtraMoves = FOREGROUND_RED;
static const unsigned numBlackMoves = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
static const unsigned numBlackSquares = FOREGROUND_RED;
static const unsigned numBlackExtraMoves = FOREGROUND_RED;
#else
static const unsigned timer = COLOR_PAIR(COLOR_GREEN);
static const unsigned error = COLOR_PAIR(COLOR_RED) | A_BOLD;
static const unsigned standard = COLOR_PAIR(COLOR_WHITE) | A_BOLD;
static const unsigned question = COLOR_PAIR(COLOR_WHITE);
static const unsigned copyright = COLOR_PAIR(COLOR_CYAN);
static const unsigned freeMoves = COLOR_PAIR(COLOR_GREEN);

static const unsigned whitePiecesOnLightSquares = COLOR_PAIR(12) | A_BOLD;
static const unsigned whitePiecesOnDarkSquares = COLOR_PAIR(13) | A_BOLD;
static const unsigned blackPiecesOnLightSquares = COLOR_PAIR(14) | A_BOLD;
static const unsigned blackPiecesOnDarkSquares = COLOR_PAIR(15) | A_BOLD;

static const unsigned whiteMoves = COLOR_PAIR(COLOR_WHITE);
static const unsigned whiteCaptures = COLOR_PAIR(COLOR_YELLOW);
static const unsigned blackMoves = COLOR_PAIR(COLOR_WHITE);
static const unsigned blackCaptures = COLOR_PAIR(COLOR_YELLOW);

static const unsigned numWhiteMoves = COLOR_PAIR(COLOR_WHITE) | A_BOLD;
static const unsigned numWhiteSquares = COLOR_PAIR(COLOR_RED);
static const unsigned numWhiteExtraMoves = COLOR_PAIR(COLOR_RED);
static const unsigned numBlackMoves = COLOR_PAIR(COLOR_WHITE) | A_BOLD;
static const unsigned numBlackSquares = COLOR_PAIR(COLOR_RED);
static const unsigned numBlackExtraMoves = COLOR_PAIR(COLOR_RED);
#endif

/* -------------------------------------------------------------------------- */

}

#endif
