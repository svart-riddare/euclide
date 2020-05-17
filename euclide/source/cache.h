#ifndef __EUCLIDE_CACHE_H
#define __EUCLIDE_CACHE_H

#include "includes.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */

class TwoPieceCache
{
	public:
		struct Position
		{
			array<Square, 2> squares;    /**< Squares on which the two pieces lie. */
			array<int, 2> moves;         /**< Number of moves played by each piece. */

			Position() {}
			Position(Square squareA, int movesA, Square squareB, int movesB)
				{ squares = { squareA, squareB }; moves = { movesA, movesB }; }
		};

	public:
		TwoPieceCache();

		void add(Square squareA, int movesA, Square squareB, int movesB, int requiredMoves, bool exact = false);
		bool hit(Square squareA, int movesA, Square squareB, int movesB, int *requiredMoves) const;

		void add(Square squareA, int movesA, Square squareB, int movesB);
		bool hit(Square squareA, int movesA, Square squareB, int movesB) const;

		void add(const Position& position, int requiredMoves);
		bool hit(const Position& position, int *requiredMoves) const;

		void add(const Position& position);
		bool hit(const Position& position) const;

	private:
		typedef struct { short movesA, movesB; int requiredMoves; } Entry;
		typedef array<Entry, 4> Line;                    /**< A cache line is made of up to four elements. */
		matrix<Line, NumSquares, NumSquares> m_cache;    /**< Cache, one line for each pair of squares. */
};

/* -------------------------------------------------------------------------- */

class TwoPieceCachePositions : public std::vector<TwoPieceCache::Position> {};

/* -------------------------------------------------------------------------- */

}

#endif
