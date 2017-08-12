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
		_initialPosition[square] = static_cast<Glyph>(problem.initial[square]);

	if (!xstd::all_of(_initialPosition, [](Glyph glyph) { return (glyph >= 0) && (glyph < NumGlyphs); }))
		throw InvalidProblem;

	if (xstd::all(_initialPosition, Empty))
		for (int k = 0; k < countof(initialPosition); k++)
			_initialPosition[initialPosition[k].square] = initialPosition[k].glyph;

	if (xstd::count(_initialPosition, WhiteKing) != 1)
		throw InvalidProblem;
	if (xstd::count(_initialPosition, BlackKing) != 1)
		throw InvalidProblem;

	/* -- Initialize diagram position -- */

	for (Square square : AllSquares())
		_diagramPosition[square] = static_cast<Glyph>(problem.diagram[square]);

	if (!xstd::all_of(_diagramPosition, [](Glyph glyph) { return (glyph >= 0) && (glyph < NumGlyphs); }))
		throw InvalidProblem;

	/* -- Initialize fairy pieces -- */

	for (Glyph glyph : AllGlyphs())
		_pieces[glyph] = static_cast<Species>(problem.pieces[glyph]);

	if (!xstd::all_of(_pieces, [](Species species) { return (species >= 0) && (species < NumSpecies); }))
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

	if (xstd::all(_pieces, None))
		for (int glyph = 0; glyph < NumGlyphs; glyph++)
			_pieces[glyph] = species[glyph].species[0];

	for (Glyph glyph : AllGlyphs())
		if (xstd::none(species[glyph].species, species[glyph].species + species[glyph].numSpecies, _pieces[glyph]))
			throw InvalidProblem;

	static const Species unimplemented[] = { Grasshopper, Leo, Pao, Vao };
	for (Glyph glyph : AllGlyphs())
		if (xstd::any(unimplemented, unimplemented + countof(unimplemented), _pieces[glyph]))
			throw UnimplementedFeature;

	/* -- Initialize chess variants -- */

	_variant = static_cast<Variant>(problem.variant);
	if ((_variant < 0) || (_variant >= NumVariants))
		throw InvalidProblem;

	if (_variant == Cylinder)
		throw UnimplementedFeature;

	/* -- Set available moves -- */

	if (problem.numHalfMoves < 0)
		throw InvalidProblem;
	if (problem.numHalfMoves == 0)
		throw UnimplementedFeature;

	_moves[White] = (problem.numHalfMoves / 2) + (problem.numHalfMoves % (problem.blackToMove ? 1 : 2));
	_moves[Black] = (problem.numHalfMoves / 2) + (problem.numHalfMoves % (problem.blackToMove ? 2 : 1));

	_turn = problem.blackToMove ? Black : White;

	/* -- Set castling flags -- */

	_castlings[White][KindSideCastling] = !problem.forbidWhiteKingSideCastling;
	_castlings[White][QueenSideCastling] = !problem.forbidWhiteQueenSideCastling;
	_castlings[Black][KindSideCastling] = !problem.forbidBlackKingSideCastling;
	_castlings[Black][QueenSideCastling] = !problem.forbidBlackQueenSideCastling;

	/* -- Count number of pieces in diagrams -- */

	for (Color color : AllColors())
		if ((_initialPieces[color] = xstd::count_if(_initialPosition, [=](Glyph glyph) { return Euclide::color(glyph) == color; })) > MaxPieces)
			throw InvalidProblem;

	for (Color color : AllColors())
		if ((_diagramPieces[color] = xstd::count_if(_diagramPosition, [=](Glyph glyph) { return Euclide::color(glyph) == color; })) > _initialPieces[color])
			throw InvalidProblem;
}

/* -------------------------------------------------------------------------- */

}
