#ifndef __EUCLIDE_HASHTABLES_H
#define __EUCLIDE_HASHTABLES_H

#include "includes.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

class MiniHashEntry
{
	public :
		MiniHashEntry();

	public :
		bool visited(const int moves[2], int *requiredMoves = NULL) const;
		void visited(const int moves[2], int requiredMoves);

	private :
		array<pair<array<int8_t, 2>, int16_t>, 7> _visits;
		int32_t _nvisits;
};

/* -------------------------------------------------------------------------- */

typedef matrix<MiniHashEntry, NumSquares, NumSquares> MiniHash;

/* -------------------------------------------------------------------------- */

}

#endif
