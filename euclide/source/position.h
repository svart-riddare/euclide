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

		bool applyNonUbiquityPrinciple();
		bool applyMoveConstraints(int availableMoves);

		int computeRequiredMoves(const Board& board);
		int getRequiredMoves() const;
		
	private :
		array<Glyph, NumSquares> glyphs;
		array<FinalSquares, NumMen> squares;

		Castling castling;
		Color color;

		int requiredMoves;
		int requiredCaptures;
};

/* -------------------------------------------------------------------------- */

}

#endif
