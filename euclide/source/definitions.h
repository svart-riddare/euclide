#ifndef __EUCLIDE_DEFINITIONS_H
#define __EUCLIDE_DEFINITIONS_H

#include "includes.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */
/* -- Status                                                               -- */
/* -------------------------------------------------------------------------- */

typedef enum { Ok, Cooked, AssertionFailed, NullPointer, SystemError, OutOfMemory, InvalidProblem, UnimplementedFeature, NumStatuses } Status;
static_assert(NumStatuses == EUCLIDE_NUM_STATUSES);

/* -------------------------------------------------------------------------- */
/* -- Color                                                                -- */
/* -------------------------------------------------------------------------- */

typedef enum { Neutral = -1, White, Black, NumColors } Color;

/* -------------------------------------------------------------------------- */
/* -- Glyph                                                                -- */
/* -------------------------------------------------------------------------- */

typedef enum { Empty, WhiteKing, BlackKing, WhiteQueen, BlackQueen, WhiteRook, BlackRook, WhiteBishop, BlackBishop, WhiteKnight, BlackKnight, WhitePawn, BlackPawn, NumGlyphs } Glyph;
static_assert(NumGlyphs == EUCLIDE_NUM_GLYPHS);

static inline Color color(Glyph glyph)
{
	return glyph ? (glyph & 1) ? White : Black : Neutral;
}

/* -------------------------------------------------------------------------- */
/* -- Piece                                                                -- */
/* -------------------------------------------------------------------------- */

typedef enum { None, King, Queen, Rook, Bishop, Knight, Pawn, Grasshopper, Nightrider, Amazon, Empress, Princess, Alfil, Camel, Zebra, Leo, Pao, Vao, Mao, NumSpecies } Species;
static_assert(NumSpecies == EUCLIDE_NUM_PIECES);

/* -------------------------------------------------------------------------- */
/* -- Variant                                                              -- */
/* -------------------------------------------------------------------------- */

typedef enum { Orthodox, Monocromatic, Bichromatic, Grid, Cylinder, NumVariants } Variant;
static_assert(NumVariants == EUCLIDE_NUM_VARIANTS);

/* -------------------------------------------------------------------------- */
/* -- Square                                                               -- */
/* -------------------------------------------------------------------------- */

typedef enum { A1, A2, A3, A4, A5, A6, A7, A8, B1, B2, B3, B4, B5, B6, B7, B8, C1, C2, C3, C4, C5, C6, C7, C8, D1, D2, D3, D4, D5, D6, D7, D8, E1, E2, E3, E4, E5, E6, E7, E8, F1, F2, F3, F4, F5, F6, F7, F8, G1, G2, G3, G4, G5, G6, G7, G8, H1, H2, H3, H4, H5, H6, H7, H8, NumSquares } Square;
static_assert(NumSquares == 64);

/* -------------------------------------------------------------------------- */

}

#endif
