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

	private :
		Glyph glyphs[NumSquares];

		int numWhiteMen;
		int numBlackMen;
};

/* -------------------------------------------------------------------------- */

}

#endif
