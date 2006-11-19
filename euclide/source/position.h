#ifndef __EUCLIDE_POSITION_H
#define __EUCLIDE_POSITION_H

#include "includes.h"
#include "destinations.h"
#include "partitions.h"
#include "targets.h"

namespace euclide 
{

/* -------------------------------------------------------------------------- */

class Pieces : public Partitions
{
	public :
		Pieces(const Problem& problem, Color color);

		bool analysePartitions();
		bool analyseMoveConstraints(int availableMoves);
		bool analyseCaptureConstraints(int availableCaptures);

		bool analyseCaptures(const Board& board, const Pieces& pieces);

		void computeRequiredMoves(const Board& board);
		void computeRequiredCaptures(const Board& board);
		void computePossiblePaths(const Board& board, int availableMoves, int availableCaptures);

		int updateRequiredMoves(bool recursive);
		int updateRequiredCaptures(bool recursive);

	public :
		inline int getRequiredMoves() const
			{ return requiredMoves; }
		inline int getRequiredCaptures() const
			{ return requiredCaptures; }

	public :
		inline Color color() const
			{ return _color; }
		
	private :
		array<Glyph, NumSquares> glyphs;
		Color _color;

	private :
		int requiredMoves;
		int requiredCaptures;		
};

/* -------------------------------------------------------------------------- */

}

#endif
