#ifndef __EUCLIDE_DEFINITIONS_H
#define __EUCLIDE_DEFINITIONS_H

#include "includes.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */
/* -- Constants                                                            -- */
/* -------------------------------------------------------------------------- */

static const int MaxPieces = 32;
static const int MaxMoves = 128;

/* -------------------------------------------------------------------------- */
/* -- Status                                                               -- */
/* -------------------------------------------------------------------------- */

typedef enum { Ok, NoSolution, AssertionFailed, NullPointer, SystemError, OutOfMemory, UserAborted, InvalidProblem, UnimplementedFeature, NumStatuses } Status;
static_assert(NumStatuses == int(EUCLIDE_NUM_STATUSES));

/* -------------------------------------------------------------------------- */
/* -- Color                                                                -- */
/* -------------------------------------------------------------------------- */

typedef enum { Neutral = -1, White, Black, NumColors } Color;

typedef EnumRange<Color, NumColors> AllColors;

constexpr Color operator!(Color color)
{
	return static_cast<Color>(1 - color);
}

/* -------------------------------------------------------------------------- */
/* -- Glyph                                                                -- */
/* -------------------------------------------------------------------------- */

typedef enum { Empty, WhiteKing, BlackKing, WhiteQueen, BlackQueen, WhiteRook, BlackRook, WhiteBishop, BlackBishop, WhiteKnight, BlackKnight, WhitePawn, BlackPawn, NumGlyphs } Glyph;
static_assert(NumGlyphs == int(EUCLIDE_NUM_GLYPHS));

typedef BitSet<Glyph, NumGlyphs> Glyphs;
typedef EnumRange<Glyph, NumGlyphs> AllGlyphs;
typedef EnumRange<Glyph, NumGlyphs, WhiteKing> MostGlyphs;
typedef Glyphs::BitSetRange ValidGlyphs;

constexpr Color color(Glyph glyph)
{
	return glyph ? (glyph & 1) ? White : Black : Neutral;
}

/* -------------------------------------------------------------------------- */
/* -- Species                                                              -- */
/* -------------------------------------------------------------------------- */

typedef enum { None, King, Queen, Rook, Bishop, Knight, Pawn, Grasshopper, Nightrider, Amazon, Empress, Princess, Alfil, Camel, Zebra, Leo, Pao, Vao, Mao, NumSpecies } Species;
static_assert(NumSpecies == int(EUCLIDE_NUM_PIECES));

typedef EnumRange<Species, NumSpecies> AllSpecies;

/* -------------------------------------------------------------------------- */
/* -- Variant                                                              -- */
/* -------------------------------------------------------------------------- */

typedef enum { Orthodox, Monochromatic, Bichromatic, Grid, Cylinder, Glasgow, NumVariants } Variant;
static_assert(NumVariants == int(EUCLIDE_NUM_VARIANTS));

typedef EnumRange<Variant, NumVariants> AllVariants;

/* -------------------------------------------------------------------------- */
/* -- Square/Squares                                                       -- */
/* -------------------------------------------------------------------------- */

typedef enum { Nowhere = -1, A1, A2, A3, A4, A5, A6, A7, A8, B1, B2, B3, B4, B5, B6, B7, B8, C1, C2, C3, C4, C5, C6, C7, C8, D1, D2, D3, D4, D5, D6, D7, D8, E1, E2, E3, E4, E5, E6, E7, E8, F1, F2, F3, F4, F5, F6, F7, F8, G1, G2, G3, G4, G5, G6, G7, G8, H1, H2, H3, H4, H5, H6, H7, H8, NumSquares } Square;
static_assert(NumSquares == 64);

typedef BitSet<Square, NumSquares> Squares;
typedef EnumRange<Square, NumSquares> AllSquares;
typedef Squares::BitSetRange ValidSquares;

typedef array<Squares, NumSquares> ArrayOfSquares;
typedef array<ArrayOfSquares, NumSquares> MatrixOfSquares;

constexpr int col(Square square)
{
	return square >> 3;
}

constexpr int row(Square square)
{
	return square & 7;
}

constexpr Square square(int col, int row)
{
	return assert(col >= 0 && col <= 7 && row >= 0 && row <= 7), static_cast<Square>((col << 3) | row);
}

/* -------------------------------------------------------------------------- */
/* -- Castling                                                             -- */
/* -------------------------------------------------------------------------- */

typedef enum { NoCastling = -1, KingSideCastling, QueenSideCastling, NumCastlingSides } CastlingSide;

typedef EnumRange<CastlingSide, NumCastlingSides> AllCastlingSides;

struct Castling { Square from; Square free; Square to; Square rook; };

static const Castling Castlings[NumColors][NumCastlingSides] =
{
	{ { E1, F1, G1, H1 }, { E1, D1, C1, A1 } },
	{ { E8, F8, G8, H8 }, { E8, D8, C8, A8 } }
};

/* -------------------------------------------------------------------------- */
/* -- Promotion                                                            -- */
/* -------------------------------------------------------------------------- */

static const Squares PromotionSquares[NumColors] =
{
	(uint64_t(1) << A8) | (uint64_t(1) << B8) | (uint64_t(1) << C8) | (uint64_t(1) << D8) | (uint64_t(1) << E8) | (uint64_t(1) << F8) | (uint64_t(1) << G8) | (uint64_t(1) << H8),
	(uint64_t(1) << A1) | (uint64_t(1) << B1) | (uint64_t(1) << C1) | (uint64_t(1) << D1) | (uint64_t(1) << E1) | (uint64_t(1) << F1) | (uint64_t(1) << G1) | (uint64_t(1) << H1)
};

static const Glyphs PromotionGlyphs[NumColors] =
{
	(uint32_t(1) << WhiteQueen) | (uint32_t(1) << WhiteRook) | (uint32_t(1) << WhiteBishop) | (uint32_t(1) << WhiteKnight),
	(uint32_t(1) << BlackQueen) | (uint32_t(1) << BlackRook) | (uint32_t(1) << BlackBishop) | (uint32_t(1) << BlackKnight),
};

/* -------------------------------------------------------------------------- */
/* -- Man                                                                  -- */
/* -------------------------------------------------------------------------- */

typedef int Man;
typedef BitSet<Man, MaxPieces> Men;

typedef EnumRange<Man, MaxPieces> AllMen;
typedef Men::BitSetRange ValidMen;

/* -------------------------------------------------------------------------- */

}

#endif
