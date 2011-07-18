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
 * \class Position
 * Static information (Partitions, Targets) associated with all pieces of a
 * given color.
 */

/* -------------------------------------------------------------------------- */

class Position : public Partitions
{
	public :
		Position(const Problem& problem, Color color);
		Position& operator+=(const Position& position);
		~Position();
		
		bool analysePartitions();
		bool analyseMoveConstraints(int availableMoves, bool quick = false);
		bool analyseCaptureConstraints(int availableCaptures, bool quick = false);

		bool analyseCaptures(const Board& board, Position& position);

		void computeRequiredMoves(const Board& board);
		void computeRequiredCaptures(const Board& board);
		void computePossiblePaths(const Board& board, int availableMoves, int availableCaptures);

		int updateRequiredMoves(bool recursive);
		int updateRequiredCaptures(bool recursive);

	public :
		inline int requiredMoves() const
			{ return _requiredMoves; }
		inline int requiredCaptures() const
			{ return _requiredCaptures; }
		
		inline int availableMoves() const
			{ return _availableMoves; }
		inline int availableCaptures() const
			{ return _availableCaptures; }

	public :
		inline Color color() const
			{ return _color; }
		
	private :
		Color _color;             /**< Player color. */

	private :
		Captures captures;

	private :
		int _requiredMoves;       /**< Total number of moves required to meet all targets for this player. */
		int _requiredCaptures;    /**< Total number of captures required to meet all targets for this player. */

		int _availableMoves;      /**< Total number of moves available to solve this problem. */
		int _availableCaptures;   /**< Total number of captures available to solve this problem. */
};

/* -------------------------------------------------------------------------- */

}

#endif
