#ifndef __EUCLIDE_POSITION_H
#define __EUCLIDE_POSITION_H

#include "includes.h"
#include "destinations.h"
#include "partitions.h"
#include "targets.h"
#include "captures.h"

namespace euclide 
{

/* -------------------------------------------------------------------------- */

/**
 * \class Pieces
 * Static information (Partitions, Targets) associated with all pieces of a
 * given color.
 */

/* -------------------------------------------------------------------------- */

class Pieces : public Partitions
{
	public :
		Pieces(const Problem& problem, Color color);
		Pieces& operator+=(const Pieces& pieces);
		~Pieces();
		
		bool analysePartitions();
		bool analyseMoveConstraints(int availableMoves, bool quick = false);
		bool analyseCaptureConstraints(int availableCaptures, bool quick = false);

		bool analyseCaptures(const Board& board, Pieces& pieces);

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
		Color _color;            /**< Player color. */

	private :
		Captures captures;

	private :
		int requiredMoves;       /**< Total number of moves required to meet all targets for this player. */
		int requiredCaptures;    /**< Total number of captures required to meet all targets for this player. */
};

/* -------------------------------------------------------------------------- */

}

#endif
