#include "problem.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Problem::Problem(const EUCLIDE_Problem *problem)
{
	/* -- Check for null pointer -- */

	if (!problem)
		abort(IncorrectInputError);

	/* -- Check number of half moves -- */

	if (problem->numHalfMoves < 0)
		abort(IncorrectInputError);

	/* -- Convert from C enumerated type to C++ one -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (!(glyphs[square] = problem->glyphs[square]).isValid())
			abort(IncorrectInputError);

	/* -- Count number of mens -- */

	int numWhiteMen = (int)std::count_if(glyphs, glyphs + NumSquares, isWhiteGlyph);
	int numBlackMen = (int)std::count_if(glyphs, glyphs + NumSquares, isBlackGlyph);

	if ((numWhiteMen > NumMen) || (numBlackMen > NumMen))
		abort(IncorrectInputError);

	/* -- Store number of moves -- */

	numHalfMoves = problem->numHalfMoves;

	if (numHalfMoves <= 0)
		abort(IncorrectInputError);
}

/* -------------------------------------------------------------------------- */

Glyph Problem::operator[](Square square) const
{
	assert(square.isValid());
	return glyphs[square];
}

/* -------------------------------------------------------------------------- */

int Problem::moves(Color color) const
{
	assert(color.isValid());

	if (color == White)
		return (numHalfMoves + 1) / 2;

	return numHalfMoves / 2;
}

/* -------------------------------------------------------------------------- */

}
