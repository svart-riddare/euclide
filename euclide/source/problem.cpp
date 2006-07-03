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
		if (!isValidGlyph(glyphs[square] = static_cast<Glyph>(problem->glyphs[square])))
			abort(IncorrectInputError);

	/* -- Count number of mens -- */

	numWhiteMen = (int)std::count_if(glyphs, glyphs + NumSquares, isWhiteGlyph);
	numBlackMen = (int)std::count_if(glyphs, glyphs + NumSquares, isBlackGlyph);

	if ((numWhiteMen > NumMen) || (numBlackMen > NumMen))
		abort(IncorrectInputError);
}

/* -------------------------------------------------------------------------- */

Glyph Problem::operator[](Square square) const
{
	assert(isValidSquare(square));

	return glyphs[square];
}

/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */

}
