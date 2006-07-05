#ifndef __EUCLIDE_POSITION_H
#define __EUCLIDE_POSITION_H

#include "includes.h"
#include "finalsquares.h"

namespace euclide 
{

/* -------------------------------------------------------------------------- */

class Pieces
{
	public :
		Pieces(const Problem& problem, Color color);

		void applyNonUbiquityPrinciple();

	private :
		array<Glyph, NumSquares> glyphs;
		array<FinalSquares, NumMen> squares;
		Color color;

		int requiredMoves;
		int requiredCaptures;
};

/* -------------------------------------------------------------------------- */

}

#endif
