#ifndef __EUCLIDE_PROBLEM_H
#define __EUCLIDE_PROBLEM_H

#include "includes.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

class Problem
{
	public :
		Problem(const EUCLIDE_Problem *problem);

		Glyph operator[](Square square) const;
		
		int moves(Color color) const;
		int captures(Color color) const;

	private :
		Glyph glyphs[NumSquares];
		int numHalfMoves;

		int numWhiteMen;
		int numBlackMen;
};

/* -------------------------------------------------------------------------- */

}

#endif
