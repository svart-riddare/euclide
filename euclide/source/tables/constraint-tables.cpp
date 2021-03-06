#include "tables.h"

namespace Euclide
{
namespace Tables
{

/* -------------------------------------------------------------------------- */

static constexpr uint64_t none(Square /*from*/, Square /*to*/, bool /*capture*/)
{
	return 0;
}

static constexpr uint64_t leaper(Square /*from*/, Square to, bool capture)
{
	return (uint64_t(!capture) << to);
}

static constexpr int gcd(int m, int n)
{
	return n ? gcd(n, m % n) : m;
}

static constexpr int delta(int z, int n)
{
	return (z && n) ? z / gcd((z > 0) ? z : -z, (n > 0) ? n : -n) : (z ? (z > 0) ? 1 : -1 : 0);
}

static constexpr uint64_t runner(Square from, Square to, int dx, int dy, bool capture)
{
	return (from != to) ? (uint64_t(!capture) << to) | runner(from, Euclide::square(col(to) - dx, row(to) - dy), dx, dy, false) : 0;
}

static constexpr uint64_t runner(Square from, Square to, bool capture)
{
	return runner(from, to, delta(col(to) - col(from), row(to) - row(from)), delta(row(to) - row(from), col(to) - col(from)), capture);
}

static constexpr uint64_t king(Square from, Square to, bool capture)
{
	return leaper(from, to, capture)
		| (((from == E1) && (to == C1)) ? runner(E1, B1, false) : 0)
		| (((from == E1) && (to == G1)) ? runner(E1, G1, false) : 0)
		| (((from == E8) && (to == C8)) ? runner(E8, B8, false) : 0)
		| (((from == E8) && (to == G8)) ? runner(E8, G8, false) : 0)
	;
}

static constexpr uint64_t pawn(Square from, Square to, bool /*capture*/)
{
	return runner(from, to, col(from) != col(to));
}

static constexpr uint64_t neighbors(Square square)
{
	return 0
		| ((col(square) > 0 && row(square) > 0) ? uint64_t(1) << Euclide::square(col(square) - 1, row(square) - 1) : 0)
		| ((col(square) > 0 && row(square) < 7) ? uint64_t(1) << Euclide::square(col(square) - 1, row(square) + 1) : 0)
		| ((col(square) < 7 && row(square) > 0) ? uint64_t(1) << Euclide::square(col(square) + 1, row(square) - 1) : 0)
		| ((col(square) < 7 && row(square) < 7) ? uint64_t(1) << Euclide::square(col(square) + 1, row(square) + 1) : 0)
		| ((col(square) > 0) ? uint64_t(1) << Euclide::square(col(square) - 1, row(square)) : 0)
		| ((col(square) < 7) ? uint64_t(1) << Euclide::square(col(square) + 1, row(square)) : 0)
		| ((row(square) > 0) ? uint64_t(1) << Euclide::square(col(square), row(square) - 1) : 0)
		| ((row(square) < 7) ? uint64_t(1) << Euclide::square(col(square), row(square) + 1) : 0);
}

#ifdef EUCLIDE_WIN_IMPLEMENTATION
   static constexpr uint64_t Neighbors[NumSquares] = {
	   neighbors(A1), neighbors(A2), neighbors(A3), neighbors(A4), neighbors(A5), neighbors(A6), neighbors(A7), neighbors(A8),
	   neighbors(B1), neighbors(B2), neighbors(B3), neighbors(B4), neighbors(B5), neighbors(B6), neighbors(B7), neighbors(B8),
	   neighbors(C1), neighbors(C2), neighbors(C3), neighbors(C4), neighbors(C5), neighbors(C6), neighbors(C7), neighbors(C8),
	   neighbors(D1), neighbors(D2), neighbors(D3), neighbors(D4), neighbors(D5), neighbors(D6), neighbors(D7), neighbors(D8),
	   neighbors(E1), neighbors(E2), neighbors(E3), neighbors(E4), neighbors(E5), neighbors(E6), neighbors(E7), neighbors(E8),
	   neighbors(F1), neighbors(F2), neighbors(F3), neighbors(F4), neighbors(F5), neighbors(F6), neighbors(F7), neighbors(F8),
	   neighbors(G1), neighbors(G2), neighbors(G3), neighbors(G4), neighbors(G5), neighbors(G6), neighbors(G7), neighbors(G8),
	   neighbors(H1), neighbors(H2), neighbors(H3), neighbors(H4), neighbors(H5), neighbors(H6), neighbors(H7), neighbors(H8)
   };
   #define neighbors(square) Neighbors[square]
#endif

static constexpr uint64_t grasshopper(Square from, Square to, bool capture)
{
	return runner(from, to, capture) & neighbors(to);
}

static constexpr uint64_t mao(Square from, Square to, bool capture)
{
	return leaper(from, to, capture) |
		(uint64_t(1) << Euclide::square(col(from) + (col(to) - col(from)) / 2, row(from) + (row(to) - row(from)) / 2));
}

/* -------------------------------------------------------------------------- */

#ifdef EUCLIDE_WIN_IMPLEMENTATION
	#ifndef NDEBUG
		#define constexpr const
	#endif
#endif

/* -------------------------------------------------------------------------- */

#define CONSTRAINTS(function, square, capture) \
{ \
	function(square, A1, capture), function(square, A2, capture), function(square, A3, capture), function(square, A4, capture), function(square, A5, capture), function(square, A6, capture), function(square, A7, capture), function(square, A8, capture), \
	function(square, B1, capture), function(square, B2, capture), function(square, B3, capture), function(square, B4, capture), function(square, B5, capture), function(square, B6, capture), function(square, B7, capture), function(square, B8, capture), \
	function(square, C1, capture), function(square, C2, capture), function(square, C3, capture), function(square, C4, capture), function(square, C5, capture), function(square, C6, capture), function(square, C7, capture), function(square, C8, capture), \
	function(square, D1, capture), function(square, D2, capture), function(square, D3, capture), function(square, D4, capture), function(square, D5, capture), function(square, D6, capture), function(square, D7, capture), function(square, D8, capture), \
	function(square, E1, capture), function(square, E2, capture), function(square, E3, capture), function(square, E4, capture), function(square, E5, capture), function(square, E6, capture), function(square, E7, capture), function(square, E8, capture), \
	function(square, F1, capture), function(square, F2, capture), function(square, F3, capture), function(square, F4, capture), function(square, F5, capture), function(square, F6, capture), function(square, F7, capture), function(square, F8, capture), \
	function(square, G1, capture), function(square, G2, capture), function(square, G3, capture), function(square, G4, capture), function(square, G5, capture), function(square, G6, capture), function(square, G7, capture), function(square, G8, capture), \
	function(square, H1, capture), function(square, H2, capture), function(square, H3, capture), function(square, H4, capture), function(square, H5, capture), function(square, H6, capture), function(square, H7, capture), function(square, H8, capture)  \
}

static constexpr MatrixOfSquares NoConstraints = { {
	CONSTRAINTS(none, A1, false), CONSTRAINTS(none, A2, false), CONSTRAINTS(none, A3, false), CONSTRAINTS(none, A4, false), CONSTRAINTS(none, A5, false), CONSTRAINTS(none, A6, false), CONSTRAINTS(none, A7, false), CONSTRAINTS(none, A8, false),
	CONSTRAINTS(none, B1, false), CONSTRAINTS(none, B2, false), CONSTRAINTS(none, B3, false), CONSTRAINTS(none, B4, false), CONSTRAINTS(none, B5, false), CONSTRAINTS(none, B6, false), CONSTRAINTS(none, B7, false), CONSTRAINTS(none, B8, false),
	CONSTRAINTS(none, C1, false), CONSTRAINTS(none, C2, false), CONSTRAINTS(none, C3, false), CONSTRAINTS(none, C4, false), CONSTRAINTS(none, C5, false), CONSTRAINTS(none, C6, false), CONSTRAINTS(none, C7, false), CONSTRAINTS(none, C8, false),
	CONSTRAINTS(none, D1, false), CONSTRAINTS(none, D2, false), CONSTRAINTS(none, D3, false), CONSTRAINTS(none, D4, false), CONSTRAINTS(none, D5, false), CONSTRAINTS(none, D6, false), CONSTRAINTS(none, D7, false), CONSTRAINTS(none, D8, false),
	CONSTRAINTS(none, E1, false), CONSTRAINTS(none, E2, false), CONSTRAINTS(none, E3, false), CONSTRAINTS(none, E4, false), CONSTRAINTS(none, E5, false), CONSTRAINTS(none, E6, false), CONSTRAINTS(none, E7, false), CONSTRAINTS(none, E8, false),
	CONSTRAINTS(none, F1, false), CONSTRAINTS(none, F2, false), CONSTRAINTS(none, F3, false), CONSTRAINTS(none, F4, false), CONSTRAINTS(none, F5, false), CONSTRAINTS(none, F6, false), CONSTRAINTS(none, F7, false), CONSTRAINTS(none, F8, false),
	CONSTRAINTS(none, G1, false), CONSTRAINTS(none, G2, false), CONSTRAINTS(none, G3, false), CONSTRAINTS(none, G4, false), CONSTRAINTS(none, G5, false), CONSTRAINTS(none, G6, false), CONSTRAINTS(none, G7, false), CONSTRAINTS(none, G8, false),
	CONSTRAINTS(none, H1, false), CONSTRAINTS(none, H2, false), CONSTRAINTS(none, H3, false), CONSTRAINTS(none, H4, false), CONSTRAINTS(none, H5, false), CONSTRAINTS(none, H6, false), CONSTRAINTS(none, H7, false), CONSTRAINTS(none, H8, false)
}};

static constexpr MatrixOfSquares KingMoveConstraints = {{
	CONSTRAINTS(king, A1, false), CONSTRAINTS(king, A2, false), CONSTRAINTS(king, A3, false), CONSTRAINTS(king, A4, false), CONSTRAINTS(king, A5, false), CONSTRAINTS(king, A6, false), CONSTRAINTS(king, A7, false), CONSTRAINTS(king, A8, false),
	CONSTRAINTS(king, B1, false), CONSTRAINTS(king, B2, false), CONSTRAINTS(king, B3, false), CONSTRAINTS(king, B4, false), CONSTRAINTS(king, B5, false), CONSTRAINTS(king, B6, false), CONSTRAINTS(king, B7, false), CONSTRAINTS(king, B8, false),
	CONSTRAINTS(king, C1, false), CONSTRAINTS(king, C2, false), CONSTRAINTS(king, C3, false), CONSTRAINTS(king, C4, false), CONSTRAINTS(king, C5, false), CONSTRAINTS(king, C6, false), CONSTRAINTS(king, C7, false), CONSTRAINTS(king, C8, false),
	CONSTRAINTS(king, D1, false), CONSTRAINTS(king, D2, false), CONSTRAINTS(king, D3, false), CONSTRAINTS(king, D4, false), CONSTRAINTS(king, D5, false), CONSTRAINTS(king, D6, false), CONSTRAINTS(king, D7, false), CONSTRAINTS(king, D8, false),
	CONSTRAINTS(king, E1, false), CONSTRAINTS(king, E2, false), CONSTRAINTS(king, E3, false), CONSTRAINTS(king, E4, false), CONSTRAINTS(king, E5, false), CONSTRAINTS(king, E6, false), CONSTRAINTS(king, E7, false), CONSTRAINTS(king, E8, false),
	CONSTRAINTS(king, F1, false), CONSTRAINTS(king, F2, false), CONSTRAINTS(king, F3, false), CONSTRAINTS(king, F4, false), CONSTRAINTS(king, F5, false), CONSTRAINTS(king, F6, false), CONSTRAINTS(king, F7, false), CONSTRAINTS(king, F8, false),
	CONSTRAINTS(king, G1, false), CONSTRAINTS(king, G2, false), CONSTRAINTS(king, G3, false), CONSTRAINTS(king, G4, false), CONSTRAINTS(king, G5, false), CONSTRAINTS(king, G6, false), CONSTRAINTS(king, G7, false), CONSTRAINTS(king, G8, false),
	CONSTRAINTS(king, H1, false), CONSTRAINTS(king, H2, false), CONSTRAINTS(king, H3, false), CONSTRAINTS(king, H4, false), CONSTRAINTS(king, H5, false), CONSTRAINTS(king, H6, false), CONSTRAINTS(king, H7, false), CONSTRAINTS(king, H8, false)
}};

static constexpr MatrixOfSquares KingCaptureConstraints = {{
	CONSTRAINTS(king, A1, true), CONSTRAINTS(king, A2, true), CONSTRAINTS(king, A3, true), CONSTRAINTS(king, A4, true), CONSTRAINTS(king, A5, true), CONSTRAINTS(king, A6, true), CONSTRAINTS(king, A7, true), CONSTRAINTS(king, A8, true),
	CONSTRAINTS(king, B1, true), CONSTRAINTS(king, B2, true), CONSTRAINTS(king, B3, true), CONSTRAINTS(king, B4, true), CONSTRAINTS(king, B5, true), CONSTRAINTS(king, B6, true), CONSTRAINTS(king, B7, true), CONSTRAINTS(king, B8, true),
	CONSTRAINTS(king, C1, true), CONSTRAINTS(king, C2, true), CONSTRAINTS(king, C3, true), CONSTRAINTS(king, C4, true), CONSTRAINTS(king, C5, true), CONSTRAINTS(king, C6, true), CONSTRAINTS(king, C7, true), CONSTRAINTS(king, C8, true),
	CONSTRAINTS(king, D1, true), CONSTRAINTS(king, D2, true), CONSTRAINTS(king, D3, true), CONSTRAINTS(king, D4, true), CONSTRAINTS(king, D5, true), CONSTRAINTS(king, D6, true), CONSTRAINTS(king, D7, true), CONSTRAINTS(king, D8, true),
	CONSTRAINTS(king, E1, true), CONSTRAINTS(king, E2, true), CONSTRAINTS(king, E3, true), CONSTRAINTS(king, E4, true), CONSTRAINTS(king, E5, true), CONSTRAINTS(king, E6, true), CONSTRAINTS(king, E7, true), CONSTRAINTS(king, E8, true),
	CONSTRAINTS(king, F1, true), CONSTRAINTS(king, F2, true), CONSTRAINTS(king, F3, true), CONSTRAINTS(king, F4, true), CONSTRAINTS(king, F5, true), CONSTRAINTS(king, F6, true), CONSTRAINTS(king, F7, true), CONSTRAINTS(king, F8, true),
	CONSTRAINTS(king, G1, true), CONSTRAINTS(king, G2, true), CONSTRAINTS(king, G3, true), CONSTRAINTS(king, G4, true), CONSTRAINTS(king, G5, true), CONSTRAINTS(king, G6, true), CONSTRAINTS(king, G7, true), CONSTRAINTS(king, G8, true),
	CONSTRAINTS(king, H1, true), CONSTRAINTS(king, H2, true), CONSTRAINTS(king, H3, true), CONSTRAINTS(king, H4, true), CONSTRAINTS(king, H5, true), CONSTRAINTS(king, H6, true), CONSTRAINTS(king, H7, true), CONSTRAINTS(king, H8, true)
}};

static const MatrixOfSquares LeaperMoveConstraints = {{
	CONSTRAINTS(leaper, A1, false), CONSTRAINTS(leaper, A2, false), CONSTRAINTS(leaper, A3, false), CONSTRAINTS(leaper, A4, false), CONSTRAINTS(leaper, A5, false), CONSTRAINTS(leaper, A6, false), CONSTRAINTS(leaper, A7, false), CONSTRAINTS(leaper, A8, false),
	CONSTRAINTS(leaper, B1, false), CONSTRAINTS(leaper, B2, false), CONSTRAINTS(leaper, B3, false), CONSTRAINTS(leaper, B4, false), CONSTRAINTS(leaper, B5, false), CONSTRAINTS(leaper, B6, false), CONSTRAINTS(leaper, B7, false), CONSTRAINTS(leaper, B8, false),
	CONSTRAINTS(leaper, C1, false), CONSTRAINTS(leaper, C2, false), CONSTRAINTS(leaper, C3, false), CONSTRAINTS(leaper, C4, false), CONSTRAINTS(leaper, C5, false), CONSTRAINTS(leaper, C6, false), CONSTRAINTS(leaper, C7, false), CONSTRAINTS(leaper, C8, false),
	CONSTRAINTS(leaper, D1, false), CONSTRAINTS(leaper, D2, false), CONSTRAINTS(leaper, D3, false), CONSTRAINTS(leaper, D4, false), CONSTRAINTS(leaper, D5, false), CONSTRAINTS(leaper, D6, false), CONSTRAINTS(leaper, D7, false), CONSTRAINTS(leaper, D8, false),
	CONSTRAINTS(leaper, E1, false), CONSTRAINTS(leaper, E2, false), CONSTRAINTS(leaper, E3, false), CONSTRAINTS(leaper, E4, false), CONSTRAINTS(leaper, E5, false), CONSTRAINTS(leaper, E6, false), CONSTRAINTS(leaper, E7, false), CONSTRAINTS(leaper, E8, false),
	CONSTRAINTS(leaper, F1, false), CONSTRAINTS(leaper, F2, false), CONSTRAINTS(leaper, F3, false), CONSTRAINTS(leaper, F4, false), CONSTRAINTS(leaper, F5, false), CONSTRAINTS(leaper, F6, false), CONSTRAINTS(leaper, F7, false), CONSTRAINTS(leaper, F8, false),
	CONSTRAINTS(leaper, G1, false), CONSTRAINTS(leaper, G2, false), CONSTRAINTS(leaper, G3, false), CONSTRAINTS(leaper, G4, false), CONSTRAINTS(leaper, G5, false), CONSTRAINTS(leaper, G6, false), CONSTRAINTS(leaper, G7, false), CONSTRAINTS(leaper, G8, false),
	CONSTRAINTS(leaper, H1, false), CONSTRAINTS(leaper, H2, false), CONSTRAINTS(leaper, H3, false), CONSTRAINTS(leaper, H4, false), CONSTRAINTS(leaper, H5, false), CONSTRAINTS(leaper, H6, false), CONSTRAINTS(leaper, H7, false), CONSTRAINTS(leaper, H8, false)
}};

static constexpr MatrixOfSquares RunnerMoveConstraints = {{
	CONSTRAINTS(runner, A1, false), CONSTRAINTS(runner, A2, false), CONSTRAINTS(runner, A3, false), CONSTRAINTS(runner, A4, false), CONSTRAINTS(runner, A5, false), CONSTRAINTS(runner, A6, false), CONSTRAINTS(runner, A7, false), CONSTRAINTS(runner, A8, false),
	CONSTRAINTS(runner, B1, false), CONSTRAINTS(runner, B2, false), CONSTRAINTS(runner, B3, false), CONSTRAINTS(runner, B4, false), CONSTRAINTS(runner, B5, false), CONSTRAINTS(runner, B6, false), CONSTRAINTS(runner, B7, false), CONSTRAINTS(runner, B8, false),
	CONSTRAINTS(runner, C1, false), CONSTRAINTS(runner, C2, false), CONSTRAINTS(runner, C3, false), CONSTRAINTS(runner, C4, false), CONSTRAINTS(runner, C5, false), CONSTRAINTS(runner, C6, false), CONSTRAINTS(runner, C7, false), CONSTRAINTS(runner, C8, false),
	CONSTRAINTS(runner, D1, false), CONSTRAINTS(runner, D2, false), CONSTRAINTS(runner, D3, false), CONSTRAINTS(runner, D4, false), CONSTRAINTS(runner, D5, false), CONSTRAINTS(runner, D6, false), CONSTRAINTS(runner, D7, false), CONSTRAINTS(runner, D8, false),
	CONSTRAINTS(runner, E1, false), CONSTRAINTS(runner, E2, false), CONSTRAINTS(runner, E3, false), CONSTRAINTS(runner, E4, false), CONSTRAINTS(runner, E5, false), CONSTRAINTS(runner, E6, false), CONSTRAINTS(runner, E7, false), CONSTRAINTS(runner, E8, false),
	CONSTRAINTS(runner, F1, false), CONSTRAINTS(runner, F2, false), CONSTRAINTS(runner, F3, false), CONSTRAINTS(runner, F4, false), CONSTRAINTS(runner, F5, false), CONSTRAINTS(runner, F6, false), CONSTRAINTS(runner, F7, false), CONSTRAINTS(runner, F8, false),
	CONSTRAINTS(runner, G1, false), CONSTRAINTS(runner, G2, false), CONSTRAINTS(runner, G3, false), CONSTRAINTS(runner, G4, false), CONSTRAINTS(runner, G5, false), CONSTRAINTS(runner, G6, false), CONSTRAINTS(runner, G7, false), CONSTRAINTS(runner, G8, false),
	CONSTRAINTS(runner, H1, false), CONSTRAINTS(runner, H2, false), CONSTRAINTS(runner, H3, false), CONSTRAINTS(runner, H4, false), CONSTRAINTS(runner, H5, false), CONSTRAINTS(runner, H6, false), CONSTRAINTS(runner, H7, false), CONSTRAINTS(runner, H8, false)
}};

static constexpr MatrixOfSquares RunnerCaptureConstraints = {{
	CONSTRAINTS(runner, A1, true), CONSTRAINTS(runner, A2, true), CONSTRAINTS(runner, A3, true), CONSTRAINTS(runner, A4, true), CONSTRAINTS(runner, A5, true), CONSTRAINTS(runner, A6, true), CONSTRAINTS(runner, A7, true), CONSTRAINTS(runner, A8, true),
	CONSTRAINTS(runner, B1, true), CONSTRAINTS(runner, B2, true), CONSTRAINTS(runner, B3, true), CONSTRAINTS(runner, B4, true), CONSTRAINTS(runner, B5, true), CONSTRAINTS(runner, B6, true), CONSTRAINTS(runner, B7, true), CONSTRAINTS(runner, B8, true),
	CONSTRAINTS(runner, C1, true), CONSTRAINTS(runner, C2, true), CONSTRAINTS(runner, C3, true), CONSTRAINTS(runner, C4, true), CONSTRAINTS(runner, C5, true), CONSTRAINTS(runner, C6, true), CONSTRAINTS(runner, C7, true), CONSTRAINTS(runner, C8, true),
	CONSTRAINTS(runner, D1, true), CONSTRAINTS(runner, D2, true), CONSTRAINTS(runner, D3, true), CONSTRAINTS(runner, D4, true), CONSTRAINTS(runner, D5, true), CONSTRAINTS(runner, D6, true), CONSTRAINTS(runner, D7, true), CONSTRAINTS(runner, D8, true),
	CONSTRAINTS(runner, E1, true), CONSTRAINTS(runner, E2, true), CONSTRAINTS(runner, E3, true), CONSTRAINTS(runner, E4, true), CONSTRAINTS(runner, E5, true), CONSTRAINTS(runner, E6, true), CONSTRAINTS(runner, E7, true), CONSTRAINTS(runner, E8, true),
	CONSTRAINTS(runner, F1, true), CONSTRAINTS(runner, F2, true), CONSTRAINTS(runner, F3, true), CONSTRAINTS(runner, F4, true), CONSTRAINTS(runner, F5, true), CONSTRAINTS(runner, F6, true), CONSTRAINTS(runner, F7, true), CONSTRAINTS(runner, F8, true),
	CONSTRAINTS(runner, G1, true), CONSTRAINTS(runner, G2, true), CONSTRAINTS(runner, G3, true), CONSTRAINTS(runner, G4, true), CONSTRAINTS(runner, G5, true), CONSTRAINTS(runner, G6, true), CONSTRAINTS(runner, G7, true), CONSTRAINTS(runner, G8, true),
	CONSTRAINTS(runner, H1, true), CONSTRAINTS(runner, H2, true), CONSTRAINTS(runner, H3, true), CONSTRAINTS(runner, H4, true), CONSTRAINTS(runner, H5, true), CONSTRAINTS(runner, H6, true), CONSTRAINTS(runner, H7, true), CONSTRAINTS(runner, H8, true)
}};

static constexpr MatrixOfSquares PawnMoveAndCaptureConstraints = {{
	CONSTRAINTS(pawn, A1, false), CONSTRAINTS(pawn, A2, false), CONSTRAINTS(pawn, A3, false), CONSTRAINTS(pawn, A4, false), CONSTRAINTS(pawn, A5, false), CONSTRAINTS(pawn, A6, false), CONSTRAINTS(pawn, A7, false), CONSTRAINTS(pawn, A8, false),
	CONSTRAINTS(pawn, B1, false), CONSTRAINTS(pawn, B2, false), CONSTRAINTS(pawn, B3, false), CONSTRAINTS(pawn, B4, false), CONSTRAINTS(pawn, B5, false), CONSTRAINTS(pawn, B6, false), CONSTRAINTS(pawn, B7, false), CONSTRAINTS(pawn, B8, false),
	CONSTRAINTS(pawn, C1, false), CONSTRAINTS(pawn, C2, false), CONSTRAINTS(pawn, C3, false), CONSTRAINTS(pawn, C4, false), CONSTRAINTS(pawn, C5, false), CONSTRAINTS(pawn, C6, false), CONSTRAINTS(pawn, C7, false), CONSTRAINTS(pawn, C8, false),
	CONSTRAINTS(pawn, D1, false), CONSTRAINTS(pawn, D2, false), CONSTRAINTS(pawn, D3, false), CONSTRAINTS(pawn, D4, false), CONSTRAINTS(pawn, D5, false), CONSTRAINTS(pawn, D6, false), CONSTRAINTS(pawn, D7, false), CONSTRAINTS(pawn, D8, false),
	CONSTRAINTS(pawn, E1, false), CONSTRAINTS(pawn, E2, false), CONSTRAINTS(pawn, E3, false), CONSTRAINTS(pawn, E4, false), CONSTRAINTS(pawn, E5, false), CONSTRAINTS(pawn, E6, false), CONSTRAINTS(pawn, E7, false), CONSTRAINTS(pawn, E8, false),
	CONSTRAINTS(pawn, F1, false), CONSTRAINTS(pawn, F2, false), CONSTRAINTS(pawn, F3, false), CONSTRAINTS(pawn, F4, false), CONSTRAINTS(pawn, F5, false), CONSTRAINTS(pawn, F6, false), CONSTRAINTS(pawn, F7, false), CONSTRAINTS(pawn, F8, false),
	CONSTRAINTS(pawn, G1, false), CONSTRAINTS(pawn, G2, false), CONSTRAINTS(pawn, G3, false), CONSTRAINTS(pawn, G4, false), CONSTRAINTS(pawn, G5, false), CONSTRAINTS(pawn, G6, false), CONSTRAINTS(pawn, G7, false), CONSTRAINTS(pawn, G8, false),
	CONSTRAINTS(pawn, H1, false), CONSTRAINTS(pawn, H2, false), CONSTRAINTS(pawn, H3, false), CONSTRAINTS(pawn, H4, false), CONSTRAINTS(pawn, H5, false), CONSTRAINTS(pawn, H6, false), CONSTRAINTS(pawn, H7, false), CONSTRAINTS(pawn, H8, false)
}};

static constexpr MatrixOfSquares GrasshopperMoveConstraints = {{
	CONSTRAINTS(grasshopper, A1, false), CONSTRAINTS(grasshopper, A2, false), CONSTRAINTS(grasshopper, A3, false), CONSTRAINTS(grasshopper, A4, false), CONSTRAINTS(grasshopper, A5, false), CONSTRAINTS(grasshopper, A6, false), CONSTRAINTS(grasshopper, A7, false), CONSTRAINTS(grasshopper, A8, false),
	CONSTRAINTS(grasshopper, B1, false), CONSTRAINTS(grasshopper, B2, false), CONSTRAINTS(grasshopper, B3, false), CONSTRAINTS(grasshopper, B4, false), CONSTRAINTS(grasshopper, B5, false), CONSTRAINTS(grasshopper, B6, false), CONSTRAINTS(grasshopper, B7, false), CONSTRAINTS(grasshopper, B8, false),
	CONSTRAINTS(grasshopper, C1, false), CONSTRAINTS(grasshopper, C2, false), CONSTRAINTS(grasshopper, C3, false), CONSTRAINTS(grasshopper, C4, false), CONSTRAINTS(grasshopper, C5, false), CONSTRAINTS(grasshopper, C6, false), CONSTRAINTS(grasshopper, C7, false), CONSTRAINTS(grasshopper, C8, false),
	CONSTRAINTS(grasshopper, D1, false), CONSTRAINTS(grasshopper, D2, false), CONSTRAINTS(grasshopper, D3, false), CONSTRAINTS(grasshopper, D4, false), CONSTRAINTS(grasshopper, D5, false), CONSTRAINTS(grasshopper, D6, false), CONSTRAINTS(grasshopper, D7, false), CONSTRAINTS(grasshopper, D8, false),
	CONSTRAINTS(grasshopper, E1, false), CONSTRAINTS(grasshopper, E2, false), CONSTRAINTS(grasshopper, E3, false), CONSTRAINTS(grasshopper, E4, false), CONSTRAINTS(grasshopper, E5, false), CONSTRAINTS(grasshopper, E6, false), CONSTRAINTS(grasshopper, E7, false), CONSTRAINTS(grasshopper, E8, false),
	CONSTRAINTS(grasshopper, F1, false), CONSTRAINTS(grasshopper, F2, false), CONSTRAINTS(grasshopper, F3, false), CONSTRAINTS(grasshopper, F4, false), CONSTRAINTS(grasshopper, F5, false), CONSTRAINTS(grasshopper, F6, false), CONSTRAINTS(grasshopper, F7, false), CONSTRAINTS(grasshopper, F8, false),
	CONSTRAINTS(grasshopper, G1, false), CONSTRAINTS(grasshopper, G2, false), CONSTRAINTS(grasshopper, G3, false), CONSTRAINTS(grasshopper, G4, false), CONSTRAINTS(grasshopper, G5, false), CONSTRAINTS(grasshopper, G6, false), CONSTRAINTS(grasshopper, G7, false), CONSTRAINTS(grasshopper, G8, false),
	CONSTRAINTS(grasshopper, H1, false), CONSTRAINTS(grasshopper, H2, false), CONSTRAINTS(grasshopper, H3, false), CONSTRAINTS(grasshopper, H4, false), CONSTRAINTS(grasshopper, H5, false), CONSTRAINTS(grasshopper, H6, false), CONSTRAINTS(grasshopper, H7, false), CONSTRAINTS(grasshopper, H8, false)
}};

static constexpr MatrixOfSquares GrasshopperCaptureConstraints = {{
	CONSTRAINTS(grasshopper, A1, true), CONSTRAINTS(grasshopper, A2, true), CONSTRAINTS(grasshopper, A3, true), CONSTRAINTS(grasshopper, A4, true), CONSTRAINTS(grasshopper, A5, true), CONSTRAINTS(grasshopper, A6, true), CONSTRAINTS(grasshopper, A7, true), CONSTRAINTS(grasshopper, A8, true),
	CONSTRAINTS(grasshopper, B1, true), CONSTRAINTS(grasshopper, B2, true), CONSTRAINTS(grasshopper, B3, true), CONSTRAINTS(grasshopper, B4, true), CONSTRAINTS(grasshopper, B5, true), CONSTRAINTS(grasshopper, B6, true), CONSTRAINTS(grasshopper, B7, true), CONSTRAINTS(grasshopper, B8, true),
	CONSTRAINTS(grasshopper, C1, true), CONSTRAINTS(grasshopper, C2, true), CONSTRAINTS(grasshopper, C3, true), CONSTRAINTS(grasshopper, C4, true), CONSTRAINTS(grasshopper, C5, true), CONSTRAINTS(grasshopper, C6, true), CONSTRAINTS(grasshopper, C7, true), CONSTRAINTS(grasshopper, C8, true),
	CONSTRAINTS(grasshopper, D1, true), CONSTRAINTS(grasshopper, D2, true), CONSTRAINTS(grasshopper, D3, true), CONSTRAINTS(grasshopper, D4, true), CONSTRAINTS(grasshopper, D5, true), CONSTRAINTS(grasshopper, D6, true), CONSTRAINTS(grasshopper, D7, true), CONSTRAINTS(grasshopper, D8, true),
	CONSTRAINTS(grasshopper, E1, true), CONSTRAINTS(grasshopper, E2, true), CONSTRAINTS(grasshopper, E3, true), CONSTRAINTS(grasshopper, E4, true), CONSTRAINTS(grasshopper, E5, true), CONSTRAINTS(grasshopper, E6, true), CONSTRAINTS(grasshopper, E7, true), CONSTRAINTS(grasshopper, E8, true),
	CONSTRAINTS(grasshopper, F1, true), CONSTRAINTS(grasshopper, F2, true), CONSTRAINTS(grasshopper, F3, true), CONSTRAINTS(grasshopper, F4, true), CONSTRAINTS(grasshopper, F5, true), CONSTRAINTS(grasshopper, F6, true), CONSTRAINTS(grasshopper, F7, true), CONSTRAINTS(grasshopper, F8, true),
	CONSTRAINTS(grasshopper, G1, true), CONSTRAINTS(grasshopper, G2, true), CONSTRAINTS(grasshopper, G3, true), CONSTRAINTS(grasshopper, G4, true), CONSTRAINTS(grasshopper, G5, true), CONSTRAINTS(grasshopper, G6, true), CONSTRAINTS(grasshopper, G7, true), CONSTRAINTS(grasshopper, G8, true),
	CONSTRAINTS(grasshopper, H1, true), CONSTRAINTS(grasshopper, H2, true), CONSTRAINTS(grasshopper, H3, true), CONSTRAINTS(grasshopper, H4, true), CONSTRAINTS(grasshopper, H5, true), CONSTRAINTS(grasshopper, H6, true), CONSTRAINTS(grasshopper, H7, true), CONSTRAINTS(grasshopper, H8, true)
}};

static constexpr MatrixOfSquares MaoMoveConstraints = {{
	CONSTRAINTS(mao, A1, false), CONSTRAINTS(mao, A2, false), CONSTRAINTS(mao, A3, false), CONSTRAINTS(mao, A4, false), CONSTRAINTS(mao, A5, false), CONSTRAINTS(mao, A6, false), CONSTRAINTS(mao, A7, false), CONSTRAINTS(mao, A8, false),
	CONSTRAINTS(mao, B1, false), CONSTRAINTS(mao, B2, false), CONSTRAINTS(mao, B3, false), CONSTRAINTS(mao, B4, false), CONSTRAINTS(mao, B5, false), CONSTRAINTS(mao, B6, false), CONSTRAINTS(mao, B7, false), CONSTRAINTS(mao, B8, false),
	CONSTRAINTS(mao, C1, false), CONSTRAINTS(mao, C2, false), CONSTRAINTS(mao, C3, false), CONSTRAINTS(mao, C4, false), CONSTRAINTS(mao, C5, false), CONSTRAINTS(mao, C6, false), CONSTRAINTS(mao, C7, false), CONSTRAINTS(mao, C8, false),
	CONSTRAINTS(mao, D1, false), CONSTRAINTS(mao, D2, false), CONSTRAINTS(mao, D3, false), CONSTRAINTS(mao, D4, false), CONSTRAINTS(mao, D5, false), CONSTRAINTS(mao, D6, false), CONSTRAINTS(mao, D7, false), CONSTRAINTS(mao, D8, false),
	CONSTRAINTS(mao, E1, false), CONSTRAINTS(mao, E2, false), CONSTRAINTS(mao, E3, false), CONSTRAINTS(mao, E4, false), CONSTRAINTS(mao, E5, false), CONSTRAINTS(mao, E6, false), CONSTRAINTS(mao, E7, false), CONSTRAINTS(mao, E8, false),
	CONSTRAINTS(mao, F1, false), CONSTRAINTS(mao, F2, false), CONSTRAINTS(mao, F3, false), CONSTRAINTS(mao, F4, false), CONSTRAINTS(mao, F5, false), CONSTRAINTS(mao, F6, false), CONSTRAINTS(mao, F7, false), CONSTRAINTS(mao, F8, false),
	CONSTRAINTS(mao, G1, false), CONSTRAINTS(mao, G2, false), CONSTRAINTS(mao, G3, false), CONSTRAINTS(mao, G4, false), CONSTRAINTS(mao, G5, false), CONSTRAINTS(mao, G6, false), CONSTRAINTS(mao, G7, false), CONSTRAINTS(mao, G8, false),
	CONSTRAINTS(mao, H1, false), CONSTRAINTS(mao, H2, false), CONSTRAINTS(mao, H3, false), CONSTRAINTS(mao, H4, false), CONSTRAINTS(mao, H5, false), CONSTRAINTS(mao, H6, false), CONSTRAINTS(mao, H7, false), CONSTRAINTS(mao, H8, false)
}};

static constexpr MatrixOfSquares MaoCaptureConstraints = {{
	CONSTRAINTS(mao, A1, true), CONSTRAINTS(mao, A2, true), CONSTRAINTS(mao, A3, true), CONSTRAINTS(mao, A4, true), CONSTRAINTS(mao, A5, true), CONSTRAINTS(mao, A6, true), CONSTRAINTS(mao, A7, true), CONSTRAINTS(mao, A8, true),
	CONSTRAINTS(mao, B1, true), CONSTRAINTS(mao, B2, true), CONSTRAINTS(mao, B3, true), CONSTRAINTS(mao, B4, true), CONSTRAINTS(mao, B5, true), CONSTRAINTS(mao, B6, true), CONSTRAINTS(mao, B7, true), CONSTRAINTS(mao, B8, true),
	CONSTRAINTS(mao, C1, true), CONSTRAINTS(mao, C2, true), CONSTRAINTS(mao, C3, true), CONSTRAINTS(mao, C4, true), CONSTRAINTS(mao, C5, true), CONSTRAINTS(mao, C6, true), CONSTRAINTS(mao, C7, true), CONSTRAINTS(mao, C8, true),
	CONSTRAINTS(mao, D1, true), CONSTRAINTS(mao, D2, true), CONSTRAINTS(mao, D3, true), CONSTRAINTS(mao, D4, true), CONSTRAINTS(mao, D5, true), CONSTRAINTS(mao, D6, true), CONSTRAINTS(mao, D7, true), CONSTRAINTS(mao, D8, true),
	CONSTRAINTS(mao, E1, true), CONSTRAINTS(mao, E2, true), CONSTRAINTS(mao, E3, true), CONSTRAINTS(mao, E4, true), CONSTRAINTS(mao, E5, true), CONSTRAINTS(mao, E6, true), CONSTRAINTS(mao, E7, true), CONSTRAINTS(mao, E8, true),
	CONSTRAINTS(mao, F1, true), CONSTRAINTS(mao, F2, true), CONSTRAINTS(mao, F3, true), CONSTRAINTS(mao, F4, true), CONSTRAINTS(mao, F5, true), CONSTRAINTS(mao, F6, true), CONSTRAINTS(mao, F7, true), CONSTRAINTS(mao, F8, true),
	CONSTRAINTS(mao, G1, true), CONSTRAINTS(mao, G2, true), CONSTRAINTS(mao, G3, true), CONSTRAINTS(mao, G4, true), CONSTRAINTS(mao, G5, true), CONSTRAINTS(mao, G6, true), CONSTRAINTS(mao, G7, true), CONSTRAINTS(mao, G8, true),
	CONSTRAINTS(mao, H1, true), CONSTRAINTS(mao, H2, true), CONSTRAINTS(mao, H3, true), CONSTRAINTS(mao, H4, true), CONSTRAINTS(mao, H5, true), CONSTRAINTS(mao, H6, true), CONSTRAINTS(mao, H7, true), CONSTRAINTS(mao, H8, true)
}};

/* -------------------------------------------------------------------------- */

const MatrixOfSquares *getMoveConstraints(Species species, Variant variant, bool capture, bool null)
{
	assert(variant != Cylinder);
	if (variant == Cylinder)
		return nullptr;  // TODO

	static const MatrixOfSquares *constraints[][2] = {
		{ nullptr, nullptr },                                                // None
		{ &KingMoveConstraints, &KingCaptureConstraints },                   // King
		{ &RunnerMoveConstraints, &RunnerCaptureConstraints },               // Queen
		{ &RunnerMoveConstraints, &RunnerCaptureConstraints },               // Rook
		{ &RunnerMoveConstraints, &RunnerCaptureConstraints },               // Bishop
		{ &LeaperMoveConstraints, &NoConstraints},                           // Knight
		{ &PawnMoveAndCaptureConstraints, &PawnMoveAndCaptureConstraints },  // Pawn
		{ &GrasshopperMoveConstraints, &GrasshopperCaptureConstraints },     // Grasshopper
		{ &RunnerMoveConstraints, &RunnerCaptureConstraints },               // Nightrider
		{ &RunnerMoveConstraints, &RunnerCaptureConstraints },               // Amazon
		{ &RunnerMoveConstraints, &RunnerCaptureConstraints },               // Empress
		{ &RunnerMoveConstraints, &RunnerCaptureConstraints },               // Princess
		{ &LeaperMoveConstraints, &NoConstraints },                          // Alfil
		{ &LeaperMoveConstraints, &NoConstraints },                          // Camel
		{ &LeaperMoveConstraints, &NoConstraints },                          // Zebra
		{ &RunnerMoveConstraints, &NoConstraints },                          // Leo
		{ &RunnerMoveConstraints, &NoConstraints },                          // Pao
		{ &RunnerMoveConstraints, &NoConstraints },                          // Vao
		{ &MaoMoveConstraints, &MaoCaptureConstraints }                      // Mao
	};

	static_assert(countof(constraints) == NumSpecies);
	return (null || constraints[species][capture]) ? constraints[species][capture] : &NoConstraints;
}

/* -------------------------------------------------------------------------- */

}}
