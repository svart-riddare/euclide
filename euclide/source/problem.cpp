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

	for (int square = 0; square < NumSquares; square++)
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

	if (xstd::count_if(_initialPosition, [](Glyph glyph) { return color(glyph) == White; }) > 16)
		throw InvalidProblem;
	if (xstd::count_if(_initialPosition, [](Glyph glyph) { return color(glyph) == Black; }) > 16)
		throw InvalidProblem;

	/* -- Initialize diagram position -- */

	for (int square = 0; square < NumSquares; square++)
		_diagramPosition[square] = static_cast<Glyph>(problem.diagram[square]);

	if (!xstd::all_of(_diagramPosition, [](Glyph glyph) { return (glyph >= 0) && (glyph < NumGlyphs); }))
		throw InvalidProblem;

	/* -- Initialize fairy pieces -- */

	for (int glyph = 0; glyph < NumGlyphs; glyph++)
		_pieces[glyph] = static_cast<Piece>(problem.pieces[glyph]);

	if (!xstd::all_of(_pieces, [](Piece piece) { return (piece >= 0) && (piece < NumPieces); }))
		throw InvalidProblem;

	static const Piece nones[] = { None };
	static const Piece kings[] = { King };
	static const Piece queens[] = { Queen, Grasshopper, Amazon, Leo };
	static const Piece rooks[] = { Rook, Empress, Pao };
	static const Piece bishops[] = { Bishop, Princess, Vao };
	static const Piece knights[] = { Knight, Nightrider, Alfil, Camel, Zebra, Mao };
	static const Piece pawns[] = { Pawn };

	static const struct { const Piece *pieces; int numPieces; } pieces[] = { 
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
			_pieces[glyph] = pieces[glyph].pieces[0];

	for (int glyph = 0; glyph < NumGlyphs; glyph++)
		if (xstd::none(pieces[glyph].pieces, pieces[glyph].pieces + pieces[glyph].numPieces, _pieces[glyph]))
			throw InvalidProblem;

	static const Piece unimplemented[] = { Grasshopper, Leo, Pao, Vao };
	for (int glyph = 0; glyph < NumGlyphs; glyph++)
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

	_forbidWhiteKingSideCastling = problem.forbidWhiteKingSideCastling;
	_forbidWhiteQueenSideCastling = problem.forbidWhiteQueenSideCastling;
	_forbidBlackKingSideCastling = problem.forbidBlackKingSideCastling;
	_forbidBlackQueenSideCastling = problem.forbidBlackQueenSideCastling;

	if ((_initialPosition[E1] != WhiteKing) || (_initialPosition[H1] != WhiteRook))
		_forbidWhiteKingSideCastling = true;
	if ((_initialPosition[E1] != WhiteKing) || (_initialPosition[A1] != WhiteRook))
		_forbidWhiteQueenSideCastling = true;
	if ((_initialPosition[E8] != BlackKing) || (_initialPosition[H1] != BlackRook))
		_forbidBlackKingSideCastling = true;
	if ((_initialPosition[E8] != BlackKing) || (_initialPosition[A1] != BlackRook))
		_forbidBlackQueenSideCastling = true;
}

/* -------------------------------------------------------------------------- */

}
