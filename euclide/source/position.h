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

		void analyseCaptures(const Board& board, const Pieces& pieces);
		bool analyseStaticPieces(Board& board);

		int computeRequiredMoves(const Board& board);
		int computeRequiredCaptures(const Board& board);
		int updateRequiredMoves();
		int updateRequiredCaptures();
		
	public :
		inline int getRequiredMoves() const
			{ return requiredMoves; }
		inline int getRequiredCaptures() const
			{ return requiredCaptures; }
		
	private :
		array<Glyph, NumSquares> glyphs;

		Castling castling;
		Color _color;

	private :
		int requiredMoves;
		int requiredCaptures;
};

/* -------------------------------------------------------------------------- */

}

#endif
