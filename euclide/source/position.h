#ifndef __EUCLIDE_POSITION_H
#define __EUCLIDE_POSITION_H

#include "includes.h"
#include "destinations.h"
#include "targets.h"

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

		void updateTargets();
		void analyseCaptures(const Board& board, const Pieces& pieces);

		int getRequiredMoves(Man man) const;
		int getNumDestinations(Man man) const;
		const Destination& getDestination(Man man) const;
		
	public :
		inline int getRequiredMoves() const
			{ return requiredMoves; }
		inline int getRequiredCaptures() const
			{ return requiredCaptures; }
		
		inline const Destinations& getDestinations() const
			{ return destinations; }
		inline const Targets& getTargets() const
			{ return targets; }

	private :
		array<Glyph, NumSquares> glyphs;
		
		Destinations destinations;
		Targets targets;

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
