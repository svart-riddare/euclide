#include "problem.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */
/* -- Problem implementation                                               -- */
/* -------------------------------------------------------------------------- */

Problem::Problem()
{
}

/* -------------------------------------------------------------------------- */

Problem::Problem(const EUCLIDE_Problem& problem)
{
	/* -- Initialize initial position and perform some basic checks -- */

	static const struct { Glyph glyph; Square square; } initialPosition[] = {
		{ WhiteKing, E1 }, { WhiteQueen, D1 }, { WhiteRook, A1 }, { WhiteRook, H1 }, { WhiteBishop, C1 }, { WhiteBishop, F1 }, { WhiteKnight, B1 }, { WhiteKnight, G1 },
		{ BlackKing, E8 }, { BlackQueen, D8 }, { BlackRook, A8 }, { BlackRook, H8 }, { BlackBishop, C8 }, { BlackBishop, F8 }, { BlackKnight, B8 }, { BlackKnight, G8 },
		{ WhitePawn, A2 }, { WhitePawn, B2 }, { WhitePawn, C2 }, { WhitePawn, D2 }, { WhitePawn, E2 }, { WhitePawn, F2 }, { WhitePawn, G2 }, { WhitePawn, H2 },
		{ BlackPawn, A7 }, { BlackPawn, B7 }, { BlackPawn, C7 }, { BlackPawn, D7 }, { BlackPawn, E7 }, { BlackPawn, F7 }, { BlackPawn, G7 }, { BlackPawn, H7 }
	};

	for (Square square : AllSquares())
		m_initialPosition[square] = static_cast<Glyph>(problem.initial[square]);

	if (!xstd::all_of(m_initialPosition, [](Glyph glyph) { return (glyph >= 0) && (glyph < NumGlyphs); }))
		throw InvalidProblem;

	if (xstd::all(m_initialPosition, Empty))
		for (unsigned k = 0; k < countof(initialPosition); k++)
			m_initialPosition[initialPosition[k].square] = initialPosition[k].glyph;

	if (xstd::count(m_initialPosition, WhiteKing) != 1)
		throw InvalidProblem;
	if (xstd::count(m_initialPosition, BlackKing) != 1)
		throw InvalidProblem;

	/* -- Initialize diagram position -- */

	for (Square square : AllSquares())
		m_diagramPosition[square] = static_cast<Glyph>(problem.diagram[square]);

	if (!xstd::all_of(m_diagramPosition, [](Glyph glyph) { return (glyph >= 0) && (glyph < NumGlyphs); }))
		throw InvalidProblem;

	/* -- Initialize fairy pieces -- */

	for (Glyph glyph : AllGlyphs())
		m_pieces[glyph] = static_cast<Species>(problem.pieces[glyph]);

	if (!xstd::all_of(m_pieces, [](Species species) { return (species >= 0) && (species < NumSpecies); }))
		throw InvalidProblem;

	static const Species nones[] = { None };
	static const Species kings[] = { King };
	static const Species queens[] = { Queen, Grasshopper, Amazon, Leo };
	static const Species rooks[] = { Rook, Empress, Pao };
	static const Species bishops[] = { Bishop, Princess, Vao };
	static const Species knights[] = { Knight, Nightrider, Alfil, Camel, Zebra, Mao };
	static const Species pawns[] = { Pawn };

	static const struct { const Species *species; int numSpecies; } species[] = {
		{ nones, countof(nones) },
		{ kings, countof(kings) }, { kings, countof(kings) },
		{ queens, countof(queens) }, { queens, countof(queens) },
		{ rooks, countof(rooks) }, { rooks, countof(rooks) },
		{ bishops, countof(bishops) }, { bishops, countof(bishops) },
		{ knights, countof(knights) }, { knights, countof(knights) },
		{ pawns, countof(pawns) }, { pawns, countof(pawns) }
	};

	if (xstd::all(m_pieces, None))
		for (int glyph = 0; glyph < NumGlyphs; glyph++)
			m_pieces[glyph] = species[glyph].species[0];

	for (Glyph glyph : AllGlyphs())
		if (xstd::none(species[glyph].species, species[glyph].species + species[glyph].numSpecies, m_pieces[glyph]))
			throw InvalidProblem;

	static const Species unimplementedSpecies[] = { Grasshopper, Leo, Pao, Vao };
	for (Glyph glyph : AllGlyphs())
		if (xstd::any(unimplementedSpecies, unimplementedSpecies + countof(unimplementedSpecies), m_pieces[glyph]))
			throw UnimplementedFeature;

	/* -- Initialize chess variants -- */

	m_variant = static_cast<Variant>(problem.variant);
	if ((m_variant < 0) || (m_variant >= NumVariants))
		throw InvalidProblem;

	static const Variant unimplementedVariants[] = { Cylinder, Glasgow };
	if (xstd::any(unimplementedVariants, unimplementedVariants + countof(unimplementedVariants), m_variant))
		throw UnimplementedFeature;

	/* -- Set available moves -- */

	if (problem.numHalfMoves < 0)
		throw InvalidProblem;
	if (problem.numHalfMoves == 0)
		throw UnimplementedFeature;
	if (problem.numHalfMoves > MaxMoves)
		throw InvalidProblem;

	m_moves[White] = (problem.numHalfMoves / 2) + (problem.numHalfMoves % (problem.blackToMove ? 1 : 2));
	m_moves[Black] = (problem.numHalfMoves / 2) + (problem.numHalfMoves % (problem.blackToMove ? 2 : 1));

	m_turn = problem.blackToMove ? Black : White;

	/* -- Set castling flags -- */

	m_castlings[White][KingSideCastling] = !problem.forbidWhiteKingSideCastling;
	m_castlings[White][QueenSideCastling] = !problem.forbidWhiteQueenSideCastling;
	m_castlings[Black][KingSideCastling] = !problem.forbidBlackKingSideCastling;
	m_castlings[Black][QueenSideCastling] = !problem.forbidBlackQueenSideCastling;

	/* -- Count number of pieces in diagrams -- */

	for (Color color : AllColors())
		if ((m_initialPieces[color] = xstd::count_if(m_initialPosition, [=](Glyph glyph) { return Euclide::color(glyph) == color; })) > MaxPieces)
			throw InvalidProblem;

	for (Color color : AllColors())
		if ((m_diagramPieces[color] = xstd::count_if(m_diagramPosition, [=](Glyph glyph) { return Euclide::color(glyph) == color; })) > m_initialPieces[color])
			throw InvalidProblem;

	m_initialPawns[White] = xstd::count(m_initialPosition, WhitePawn);
	m_initialPawns[Black] = xstd::count(m_initialPosition, BlackPawn);

	m_diagramPawns[White] = xstd::count(m_diagramPosition, WhitePawn);
	m_diagramPawns[Black] = xstd::count(m_diagramPosition, BlackPawn);
}

/* -------------------------------------------------------------------------- */

}
