#include "fate.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Fates::Fates(const Problem& problem, Color color)
{
	/* -- Associate with each man a possible final square -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
	{
		Glyph glyph = problem[square];

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
						//Fates[Man].PossibleSquares.Push(new possiblesquare(Square, StaticGlyphToSuperMan[Glyph][Column]));
						;
				}
				else
				{
					//Fates[Man].PossibleSquares.Push(new possiblesquare(Square, Man));
				}
			}
		}
	}
}

/* -------------------------------------------------------------------------- */

}
