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

		/* -- Associate glyphs to men -- */

		for (Man man = FirstMan; man <= LastMan; man++)
			if (tables::supermanToGlyph[man][color] == glyph)
				squares[man] += FinalSquare(square, man, man, false);

		/* -- Handle promoted pieces -- */

		for (Superman superman = FirstPromotedMan; superman <= LastPromotedMan; superman++)
			if (tables::supermanToGlyph[superman][color] == glyph)
				for (Man man = FirstPawn; man <= LastPawn; man++)
					squares[man] += FinalSquare(square, man, superman, false);
	}
}

/* -------------------------------------------------------------------------- */

void Pieces::applyNonUbiquityPrinciple()
{
	array<bool, NumSquares> unique;
	array<Man, NumSquares> men;

	unique.assign(false);
	men.assign(UndefinedMan);

	/* -- Scan, for each man, the list of possible final squares -- */

	for (Man man = FirstMan; man <= LastMan; man++)
	{
		const vector<FinalSquare>& squares = this->squares[man];

		for (vector<FinalSquare>::const_iterator I = squares.begin(); I != squares.end(); I++)
		{
			Square square = *I;

			/* -- Check if it is the only man that can end on this square -- */

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

	bool modified = false;

	/* -- If a man is the only one that can end on a given occupied square, 
	      then this man must indeed end on that square -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
	{
		/* -- If there is no possible man for a given square,
		      the problem has no solution -- */

		if (glyphs[square].isColor(color))
			if (men[square] == UndefinedMan)
				abort(NoSolution);

		/* -- Non ubiquity deduction -- */

		if (unique[square])
			if (squares[men[square]] = square)
				modified = true;
	}	

	/* -- Make some further deductions if possible -- */

	if (modified)
		applyNonUbiquityPrinciple();
}

/* -------------------------------------------------------------------------- */

}
