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
		
		void computeRequiredMoves(const Board& board);
		void computeRequiredCaptures(const Board& board);
		void updateRequiredMoves(bool updateDestinations = true);
		void updateRequiredCaptures(bool updateDestinations = true);

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
		int requiredCaptures;
};

/* -------------------------------------------------------------------------- */

}

#endif
