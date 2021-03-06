#include "tables.h"

namespace Euclide
{
namespace Tables
{

/* -------------------------------------------------------------------------- */

static constexpr bool valid(Square square, int x, int y, bool cylindrical)
{
	return (cylindrical || ((col(square) + x) >= 0)) && (cylindrical || ((col(square) + x) < 8)) && ((row(square) + y) >= 0) && ((row(square) + y) < 8);
}

static constexpr uint64_t move(Square square, int x, int y, bool cylindrical)
{
	return valid(square, x, y, cylindrical) ? uint64_t(1) << Euclide::square((col(square) + x) & 7, row(square) + y) : 0;
}

static constexpr uint64_t leaper(Square square, int x, int y, bool cylindrical)
{
	return
		move(square, +x, +y, cylindrical) |
		move(square, -x, +y, cylindrical) |
		move(square, +x, -y, cylindrical) |
		move(square, -x, -y, cylindrical) |
		move(square, +y, +x, cylindrical) |
		move(square, -y, +x, cylindrical) |
		move(square, +y, -x, cylindrical) |
		move(square, -y, -x, cylindrical);
}

static constexpr uint64_t runner(Square square, int x, int y, bool cylindrical)
{
	return
		leaper(square, 1 * x, 1 * y, cylindrical) |
		leaper(square, 2 * x, 2 * y, cylindrical) |
		leaper(square, 3 * x, 3 * y, cylindrical) |
		leaper(square, 4 * x, 4 * y, cylindrical) |
		leaper(square, 5 * x, 5 * y, cylindrical) |
		leaper(square, 6 * x, 6 * y, cylindrical) |
		leaper(square, 7 * x, 7 * y, cylindrical);
}

static constexpr uint64_t pawn(Square square, Color color, bool capture, bool cylindrical)
{
	return (row(square) != (color ? 7 : 0)) ? move(square, capture ? -1 : 0, color ? -1 : +1, cylindrical) | move(square, capture ? +1 : 0, color ? -1 : +1, cylindrical) | (((row(square) == (color ? 6 : 1)) && !capture) ? move(square, 0, color ? -2 : 2, cylindrical) : 0) : 0;
}

/* -------------------------------------------------------------------------- */

#define MOVES(type, x, y, c) \
{ \
	type(A1, x, y, c), type(A2, x, y, c), type(A3, x, y, c), type(A4, x, y, c), type(A5, x, y, c), type(A6, x, y, c), type(A7, x, y, c), type(A8, x, y, c), \
	type(B1, x, y, c), type(B2, x, y, c), type(B3, x, y, c), type(B4, x, y, c), type(B5, x, y, c), type(B6, x, y, c), type(B7, x, y, c), type(B8, x, y, c), \
	type(C1, x, y, c), type(C2, x, y, c), type(C3, x, y, c), type(C4, x, y, c), type(C5, x, y, c), type(C6, x, y, c), type(C7, x, y, c), type(C8, x, y, c), \
	type(D1, x, y, c), type(D2, x, y, c), type(D3, x, y, c), type(D4, x, y, c), type(D5, x, y, c), type(D6, x, y, c), type(D7, x, y, c), type(D8, x, y, c), \
	type(E1, x, y, c), type(E2, x, y, c), type(E3, x, y, c), type(E4, x, y, c), type(E5, x, y, c), type(E6, x, y, c), type(E7, x, y, c), type(E8, x, y, c), \
	type(F1, x, y, c), type(F2, x, y, c), type(F3, x, y, c), type(F4, x, y, c), type(F5, x, y, c), type(F6, x, y, c), type(F7, x, y, c), type(F8, x, y, c), \
	type(G1, x, y, c), type(G2, x, y, c), type(G3, x, y, c), type(G4, x, y, c), type(G5, x, y, c), type(G6, x, y, c), type(G7, x, y, c), type(G8, x, y, c), \
	type(H1, x, y, c), type(H2, x, y, c), type(H3, x, y, c), type(H4, x, y, c), type(H5, x, y, c), type(H6, x, y, c), type(H7, x, y, c), type(H8, x, y, c) \
}

static constexpr ArrayOfSquares WazirMoves[2] = { MOVES(leaper, 0, 1, false), MOVES(leaper, 0, 1, true) };
static constexpr ArrayOfSquares FerzMoves[2] = { MOVES(leaper, 1, 1, false), MOVES(leaper, 1, 1, true) };
static constexpr ArrayOfSquares KnightMoves[2] = { MOVES(leaper, 1, 2, false), MOVES(leaper, 1, 2, true) };
static constexpr ArrayOfSquares AlfilMoves[2] = { MOVES(leaper, 2, 2, false), MOVES(leaper, 2, 2, true) };
static constexpr ArrayOfSquares CamelMoves[2] = { MOVES(leaper, 1, 3, false), MOVES(leaper, 1, 3, true) };
static constexpr ArrayOfSquares ZebraMoves[2] = { MOVES(leaper, 2, 3, false), MOVES(leaper, 2, 3, true) };

static constexpr ArrayOfSquares RookMoves[2] = { MOVES(runner, 0, 1, false), MOVES(runner, 0, 1, true) };
static constexpr ArrayOfSquares BishopMoves[2] = { MOVES(runner, 1, 1, false), MOVES(runner, 1, 1, true) };
static constexpr ArrayOfSquares NightriderMoves[2] = { MOVES(runner, 1, 2, false), MOVES(runner, 1, 2, true) };

static constexpr ArrayOfSquares PawnMoves[2][NumColors] = { { MOVES(pawn, White, false, false), MOVES(pawn, Black, false, false) }, { MOVES(pawn, White, false, true), MOVES(pawn, Black, false, true) } };
static constexpr ArrayOfSquares PawnCaptures[2][NumColors] = { { MOVES(pawn, White, true, false), MOVES(pawn, Black, true, false) }, { MOVES(pawn, White, true, true), MOVES(pawn, Black, true, true) } };

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

static constexpr Squares NoSquares;
static constexpr ArrayOfSquares NoCaptures =
{
	NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares,
	NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares,
	NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares,
	NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares,
	NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares,
	NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares,
	NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares,
	NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares, NoSquares
};

/* -------------------------------------------------------------------------- */

void initializeLegalMoves(ArrayOfSquares *moves, Species species, Color color, Variant variant, tribool capture, bool promotion)
{
	const int cylindrical = (variant == Cylinder) ? 1 : 0;

	/* -- Get list of legal moves for pieces -- */

	switch (species)
	{
		case None:
			break;
		case King:
			for (Square square : AllSquares())
				(*moves)[square] = WazirMoves[cylindrical][square] | FerzMoves[cylindrical][square];
			break;
		case Queen:
			for (Square square : AllSquares())
				(*moves)[square] = RookMoves[cylindrical][square] | BishopMoves[cylindrical][square];
			break;
		case Rook:
			xstd::copy(RookMoves[cylindrical], moves->begin());
			break;
		case Bishop:
			xstd::copy(BishopMoves[cylindrical], moves->begin());
			break;
		case Knight:
			xstd::copy(KnightMoves[cylindrical], moves->begin());
			break;
		case Pawn:
			if (!capture)
				xstd::copy(PawnMoves[cylindrical][color], moves->begin());
			else
			if (capture)
				xstd::copy(PawnCaptures[cylindrical][color], moves->begin());
			else
				for (Square square : AllSquares())
					(*moves)[square] = PawnMoves[cylindrical][color][square] | PawnCaptures[cylindrical][color][square];
			if (!promotion)
				for (Square square : AllSquares())
					(*moves)[square] &= ~PromotionSquares[color];
			break;
		case Grasshopper:
			for (Square square : AllSquares())
				(*moves)[square] = (RookMoves[cylindrical][square] | BishopMoves[cylindrical][square]) & ~(WazirMoves[cylindrical][square] | FerzMoves[cylindrical][square]);
			break;
		case Nightrider:
			xstd::copy(NightriderMoves[cylindrical], moves->begin());
			break;
		case Amazon:
			for (Square square : AllSquares())
				(*moves)[square] = RookMoves[cylindrical][square] | BishopMoves[cylindrical][square] | KnightMoves[cylindrical][square];
			break;
		case Empress:
			for (Square square : AllSquares())
				(*moves)[square] = RookMoves[cylindrical][square] | KnightMoves[cylindrical][square];
			break;
		case Princess:
			for (Square square : AllSquares())
				(*moves)[square] = BishopMoves[cylindrical][square] | KnightMoves[cylindrical][square];
			break;
		case Alfil:
			xstd::copy(AlfilMoves[cylindrical], moves->begin());
			break;
		case Camel:
			xstd::copy(CamelMoves[cylindrical], moves->begin());
			break;
		case Zebra:
			xstd::copy(ZebraMoves[cylindrical], moves->begin());
			break;
		case Leo:
			for (Square square : AllSquares())
				(*moves)[square] = RookMoves[cylindrical][square] | BishopMoves[cylindrical][square];
			break;
		case Pao:
			xstd::copy(RookMoves[cylindrical], moves->begin());
			break;
		case Vao:
			xstd::copy(BishopMoves[cylindrical], moves->begin());
			break;
		case Mao:
			xstd::copy(KnightMoves[cylindrical], moves->begin());
			break;
		case NumSpecies:
			assert(false);
			break;
	}

	/* -- Restrict legal moves according to variant -- */

	if (variant == Monochromatic)
		for (Square square : AllSquares())
			(*moves)[square] &= (WhiteSquares[square] ? WhiteSquares : BlackSquares);

	if (variant == Bichromatic)
		for (Square square : AllSquares())
			(*moves)[square] &= (WhiteSquares[square] ? BlackSquares : WhiteSquares);

	if (variant == Grid)
		for (Square square : AllSquares())
			(*moves)[square] &= ~GridSquares[square];
}

/* -------------------------------------------------------------------------- */

const ArrayOfSquares *getCaptureMoves(Species species, Color color, Variant variant, bool null)
{
	if (species == Pawn)
		return &PawnCaptures[variant == Cylinder][color];

	return null ? nullptr : &NoCaptures;
}

/* -------------------------------------------------------------------------- */

void initializeLineOfSights(const array<Species, NumGlyphs>& species, Variant variant, array<MatrixOfSquares, NumColors> *lines)
{
	for (Glyph glyph : AllGlyphs())
	{
		Color color = Euclide::color(glyph);

		ArrayOfSquares captures;
		initializeLegalMoves(&captures, species[glyph], color, variant, true, true);
		const MatrixOfSquares *constraints = getMoveConstraints(species[glyph], variant, true);

		for (Square from : AllSquares())
		{
			for (Square to : ValidSquares(captures[from]))
			{
				Squares shields = constraints ? (*constraints)[from][to] | Squares(to) : Squares(to);

				for (Square shield : ValidSquares(shields))
					(*lines)[!color][to][shield][from] = true;
			}
		}
	}
}

/* -------------------------------------------------------------------------- */

}}