#include "position.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Pieces::Pieces(const Problem& problem, Color color)
{
	this->color = color;

	/* -- Associate with each man a possible final square -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
	{
		Glyph glyph = problem[square];
		glyphs[square] = glyph;

		if (glyph == NoGlyph)
			continue;

		for (Man man = FirstMan; man <= LastMan; man++)
		{
			/* -- Associate glyphs to men -- */

			if (tables::validGlyphManColor[glyph][man][color])
			{
				/* -- Handle promoted pawns -- */

				if (!isGlyphPawn(glyph) && isManPawn(man))
				{
					for (Column column = FirstColumn; column <= LastColumn; column++)
						squares[man] += FinalSquare(square, tables::getGlyphSuperman[glyph][column], false);
				}
				else
				{
					squares[man] += FinalSquare(square, man, false);
				}
			}
		}
	}
}

/* -------------------------------------------------------------------------- */

void Pieces::applyNonUbiquityPrinciple()
{
	array<bool, NumSquares> unique;
	array<Man, NumSquares> men;

	unique.assign(false);
	men.assign(UndefinedMan);

	for (Man man = FirstMan; man <= LastMan; man++)
	{
		const vector<FinalSquare>& squares = this->squares[man];

		for (vector<FinalSquare>::const_iterator I = squares.begin(); I != squares.end(); I++)
		{
			Square square = *I;

			if (men[square] == UndefinedMan)
			{
				unique[square] = true;
				men[square] = man;
			}
			else
			if (men[square] != man)
				unique[square] = false;
		}
	}

	bool recursive = false;

	for (Square square = FirstSquare; square <= LastSquare; square++)
	{
		if (isValidGlyph(glyphs[square], color))
			if (men[square] == UndefinedMan)
				abort(NoSolution);

		if (unique[square])
		{
			squares[men[square]] = square;
			recursive = true;
		}
	}	

	if (recursive)
		applyNonUbiquityPrinciple();
}

/* -------------------------------------------------------------------------- */

}
