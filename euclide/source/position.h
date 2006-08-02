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
		bool applyCaptureConstraints(int availableCaptures);
		
		int computeRequiredMoves(const Board& board);
		int getRequiredMoves() const;

		int computeRequiredCaptures(const Board& board);
		int getRequiredCaptures() const;

		const FinalSquares& operator[](Man man) const;
		
	private :
		array<Glyph, NumSquares> glyphs;
		array<FinalSquares, NumMen> squares;

		array<int, NumSquares> assignedMoves;
		array<int, NumSquares> assignedCaptures;

		Castling castling;
		Color color;

		int requiredMoves;
		int requiredCaptures;
};

/* -------------------------------------------------------------------------- */

}

#endif
