#ifndef __EUCLIDE_CACHE_H
#define __EUCLIDE_CACHE_H

#include "includes.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */

class TwoPieceCache
{
	public :
		TwoPieceCache();

		void add(Square squareA, int movesA, Square squareB, int movesB, int requiredMoves);
		bool hit(Square squareA, int movesA, Square squareB, int movesB, int *requiredMoves) const;

	private :
		typedef struct { short movesA, movesB; int requiredMoves; } Entry;
		typedef array<Entry, 4> Line;                    /**< A cache line is made of up to four elements. */
		matrix<Line, NumSquares, NumSquares> m_cache;    /**< Cache, one line for each pair of squares. */
};

/* -------------------------------------------------------------------------- */

}

#endif
