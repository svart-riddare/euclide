#ifndef __EUCLIDE_POSITION_H
#define __EUCLIDE_POSITION_H

#include "includes.h"
#include "destinations.h"

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
		int computeRequiredCaptures(const Board& board);

		int getRequiredMoves(Man man) const;
		int getNumDestinations(Man man) const;
		const Destination& getDestination(Man man) const;

	public :
		int getRequiredMoves() const
			{ return requiredMoves; }
		
		int getRequiredCaptures() const
			{ return requiredCaptures; }
	
	private :
		array<Glyph, NumSquares> glyphs;
		Destinations destinations;

		Castling castling;
		Color color;

		int requiredMoves;
		int requiredMovesByMen;
		int requiredMovesBySquares;
		
		int requiredCaptures;
		int requiredCapturesByMen;
		int requiredCapturesBySquares;
};

/* -------------------------------------------------------------------------- */

}

#endif
