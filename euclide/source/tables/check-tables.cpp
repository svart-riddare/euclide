#include "tables.h"

namespace Euclide
{
namespace Tables
{

/* -------------------------------------------------------------------------- */

static constexpr Squares WhiteSquares = UINT64_C(0x55AA55AA55AA55AA);
static constexpr Squares BlackSquares = UINT64_C(0xAA55AA55AA55AA55);

static constexpr Squares GridSquares[NumSquares] =
{
	UINT64_C(0x0303) <<  0, UINT64_C(0x0303) <<  0, UINT64_C(0x0C0C) <<  0, UINT64_C(0x0C0C) <<  0, UINT64_C(0x0303) <<  4, UINT64_C(0x0303) <<  4, UINT64_C(0x0C0C) <<  4, UINT64_C(0x0C0C) <<  4,
	UINT64_C(0x0303) <<  0, UINT64_C(0x0303) <<  0, UINT64_C(0x0C0C) <<  0, UINT64_C(0x0C0C) <<  0, UINT64_C(0x0303) <<  4, UINT64_C(0x0303) <<  4, UINT64_C(0x0C0C) <<  4, UINT64_C(0x0C0C) <<  4,
	UINT64_C(0x0303) << 16, UINT64_C(0x0303) << 16, UINT64_C(0x0C0C) << 16, UINT64_C(0x0C0C) << 16, UINT64_C(0x0303) << 20, UINT64_C(0x0303) << 20, UINT64_C(0x0C0C) << 20, UINT64_C(0x0C0C) << 20,
	UINT64_C(0x0303) << 16, UINT64_C(0x0303) << 16, UINT64_C(0x0C0C) << 16, UINT64_C(0x0C0C) << 16, UINT64_C(0x0303) << 20, UINT64_C(0x0303) << 20, UINT64_C(0x0C0C) << 20, UINT64_C(0x0C0C) << 20,
	UINT64_C(0x0303) << 32, UINT64_C(0x0303) << 32, UINT64_C(0x0C0C) << 32, UINT64_C(0x0C0C) << 32, UINT64_C(0x0303) << 36, UINT64_C(0x0303) << 36, UINT64_C(0x0C0C) << 36, UINT64_C(0x0C0C) << 36,
	UINT64_C(0x0303) << 32, UINT64_C(0x0303) << 32, UINT64_C(0x0C0C) << 32, UINT64_C(0x0C0C) << 32, UINT64_C(0x0303) << 36, UINT64_C(0x0303) << 36, UINT64_C(0x0C0C) << 36, UINT64_C(0x0C0C) << 36,
	UINT64_C(0x0303) << 48, UINT64_C(0x0303) << 48, UINT64_C(0x0C0C) << 48, UINT64_C(0x0C0C) << 48, UINT64_C(0x0303) << 52, UINT64_C(0x0303) << 52, UINT64_C(0x0C0C) << 52, UINT64_C(0x0C0C) << 52,
	UINT64_C(0x0303) << 48, UINT64_C(0x0303) << 48, UINT64_C(0x0C0C) << 48, UINT64_C(0x0C0C) << 48, UINT64_C(0x0303) << 52, UINT64_C(0x0303) << 52, UINT64_C(0x0C0C) << 52, UINT64_C(0x0C0C) << 52
};

/* -------------------------------------------------------------------------- */

static constexpr bool possible(Square squareA, Square squareB, Variant variant)
{
	return (variant == Orthodox) || (variant == Cylinder) ||
		((variant == Monochromatic) && (WhiteSquares[squareA] == WhiteSquares[squareB])) ||
		((variant == Bichromatic) && (WhiteSquares[squareA] == BlackSquares[squareB])) ||
		((variant == Grid) && GridSquares[squareA][squareB]);
}

static constexpr bool valid(Square square, int x, int y, Variant variant)
{
	return ((variant == Cylinder) || ((col(square) + x) >= 0)) && ((variant == Cylinder) || ((col(square) + x) < 8))
		&& ((row(square) + y) >= 0) && ((row(square) + y) < 8)
		&& possible(square, Euclide::square((col(square) + x) & 7, row(square) + y), variant);
}

static constexpr uint64_t move(Square square, int x, int y, Variant variant)
{
	return valid(square, x, y, variant) ? uint64_t(1) << Euclide::square((col(square) + x) & 7, row(square) + y) : 0;
}

static constexpr uint64_t leaper(Square square, int x, int y, Variant variant)
{
	return (x || y) ? (
		move(square, +x, +y, variant) |
		move(square, -x, +y, variant) |
		move(square, +x, -y, variant) |
		move(square, -x, -y, variant) |
		move(square, +y, +x, variant) |
		move(square, -y, +x, variant) |
		move(square, +y, -x, variant) |
		move(square, -y, -x, variant)) : 0;
}

static constexpr uint64_t vicinity(Square square, int x, int y, Variant variant)
{
	return leaper(square, x, y, variant) |
		move(square, -1, -1, variant) |
		move(square, -1,  0, variant) |
		move(square, -1,  1, variant) |
		move(square,  0, -1, variant) |
		move(square,  0,  1, variant) |
		move(square,  1, -1, variant) |
		move(square,  1,  0, variant) |
		move(square,  1,  1, variant);
}

static constexpr uint64_t knight(Square square, int x, int y, Variant variant)
{
	return leaper(square, 1, 2, variant) | leaper(square, x, y, variant);
}

static constexpr uint64_t pawn(Square square, int /*x*/, int y, Variant variant)
{
	return move(square, -1, y, variant) | move(square, +1, y, variant);
}

/* -------------------------------------------------------------------------- */

#define CHECKS(type, x, y, v) \
{ \
	type(A1, x, y, v), type(A2, x, y, v), type(A3, x, y, v), type(A4, x, y, v), type(A5, x, y, v), type(A6, x, y, v), type(A7, x, y, v), type(A8, x, y, v), \
	type(B1, x, y, v), type(B2, x, y, v), type(B3, x, y, v), type(B4, x, y, v), type(B5, x, y, v), type(B6, x, y, v), type(B7, x, y, v), type(B8, x, y, v), \
	type(C1, x, y, v), type(C2, x, y, v), type(C3, x, y, v), type(C4, x, y, v), type(C5, x, y, v), type(C6, x, y, v), type(C7, x, y, v), type(C8, x, y, v), \
	type(D1, x, y, v), type(D2, x, y, v), type(D3, x, y, v), type(D4, x, y, v), type(D5, x, y, v), type(D6, x, y, v), type(D7, x, y, v), type(D8, x, y, v), \
	type(E1, x, y, v), type(E2, x, y, v), type(E3, x, y, v), type(E4, x, y, v), type(E5, x, y, v), type(E6, x, y, v), type(E7, x, y, v), type(E8, x, y, v), \
	type(F1, x, y, v), type(F2, x, y, v), type(F3, x, y, v), type(F4, x, y, v), type(F5, x, y, v), type(F6, x, y, v), type(F7, x, y, v), type(F8, x, y, v), \
	type(G1, x, y, v), type(G2, x, y, v), type(G3, x, y, v), type(G4, x, y, v), type(G5, x, y, v), type(G6, x, y, v), type(G7, x, y, v), type(G8, x, y, v), \
	type(H1, x, y, v), type(H2, x, y, v), type(H3, x, y, v), type(H4, x, y, v), type(H5, x, y, v), type(H6, x, y, v), type(H7, x, y, v), type(H8, x, y, v) \
}

#define ALLCHECKS(type, x, y) \
{ \
	CHECKS(type, x, y, Orthodox), \
	CHECKS(type, x, y, Monochromatic), \
	CHECKS(type, x, y, Bichromatic), \
	CHECKS(type, x, y, Grid), \
	CHECKS(type, x, y, Cylinder) \
}

static constexpr ArrayOfSquares NoChecks = CHECKS(leaper, 0, 0, Orthodox);
static constexpr ArrayOfSquares KingChecks[NumVariants] = ALLCHECKS(vicinity, 0, 0);
static constexpr ArrayOfSquares RookChecks[NumVariants] = ALLCHECKS(leaper, 0, 1);
static constexpr ArrayOfSquares BishopChecks[NumVariants] = ALLCHECKS(leaper, 1, 1);
static constexpr ArrayOfSquares KnightChecks[NumVariants] = ALLCHECKS(knight, 0, 0);
static constexpr ArrayOfSquares AmazonChecks[NumVariants] = ALLCHECKS(vicinity, 1, 2);
static constexpr ArrayOfSquares EmpressChecks[NumVariants] = ALLCHECKS(knight, 0, 1);
static constexpr ArrayOfSquares PrincessChecks[NumVariants] = ALLCHECKS(knight, 1, 1);
static constexpr ArrayOfSquares AlfilChecks[NumVariants] = ALLCHECKS(leaper, 2, 2);
static constexpr ArrayOfSquares CamelChecks[NumVariants] = ALLCHECKS(leaper, 1, 3);
static constexpr ArrayOfSquares ZebraChecks[NumVariants] = ALLCHECKS(leaper, 2, 3);
static constexpr ArrayOfSquares PawnChecks[NumColors][NumVariants] = { ALLCHECKS(pawn, 0, 1), ALLCHECKS(pawn, 0, -1) };

/* -------------------------------------------------------------------------- */

const ArrayOfSquares *getUnstoppableChecks(Species species, Color color, Variant variant)
{
	switch (species)
	{
		case None:
			return &NoChecks;
		case King:
			return &KingChecks[variant];
		case Queen:
			return &KingChecks[variant];
		case Rook:
			return &RookChecks[variant];
		case Bishop:
			return &BishopChecks[variant];
		case Knight:
			return &KnightChecks[variant];
		case Pawn:
			return &PawnChecks[color][variant];
		case Grasshopper:
			return &NoChecks;
		case Nightrider:
			return &KnightChecks[variant];
		case Amazon:
			return &AmazonChecks[variant];
		case Empress:
			return &EmpressChecks[variant];
		case Princess:
			return &PrincessChecks[variant];
		case Alfil:
			return &AlfilChecks[variant];
		case Camel:
			return &CamelChecks[variant];
		case Zebra:
			return &ZebraChecks[variant];
		case Leo:
		case Pao:
		case Vao:
		case Mao:
			return &NoChecks;
		case NumSpecies:
			assert(false);
			break;
	}

	return &NoChecks;
}

/* -------------------------------------------------------------------------- */

}}