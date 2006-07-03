#ifndef __COLORS_H
#define __COLORS_H

#include "includes.h"

namespace colors
{

/* -------------------------------------------------------------------------- */

static const WORD timer = FOREGROUND_GREEN;
static const WORD error = FOREGROUND_RED | FOREGROUND_INTENSITY;
static const WORD standard = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
static const WORD question = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
static const WORD copyright = FOREGROUND_RED | FOREGROUND_BLUE;
static const WORD freeMoves = FOREGROUND_GREEN;

static const WORD whitePieces = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
static const WORD blackPieces = 0;
static const WORD lightSquares = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
static const WORD darkSquares = BACKGROUND_RED | BACKGROUND_BLUE;

/* -------------------------------------------------------------------------- */

}

#endif
