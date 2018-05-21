#include "cache.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */

TwoPieceCache::TwoPieceCache()
{
	const Entry entry = { Infinity, Infinity, Infinity };
	const Line line = { entry, entry, entry, entry };
	m_cache.fill(line);
}

/* -------------------------------------------------------------------------- */

void TwoPieceCache::add(Square squareA, int movesA, Square squareB, int movesB, int requiredMoves)
{
	Line& line = m_cache[squareA][squareB];

	/* -- This is our new cache entry -- */

	const Entry entry = { short(movesA), short(movesB), requiredMoves };

	/* -- First cache entry is for overall minimum -- */

	if (movesA + movesB < line[0].movesA + line[0].movesB)
		line[0] = entry;

	/* -- Then one entry for each of the cases movesA <=> movesB -- */

	if (movesA < movesB)
		if ((movesA < line[1].movesA) || ((movesA == line[1].movesA) && (movesB < line[1].movesB)))
			line[1] = entry;
	
	if (movesA == movesB)
		if (movesA < line[2].movesA)
			line[2] = entry;

	if (movesA > movesB)
		if ((movesB < line[3].movesB) || (movesB == line[3].movesB) && (movesA < line[3].movesA))
			line[3] = entry;
}

/* -------------------------------------------------------------------------- */

bool TwoPieceCache::hit(Square squareA, int movesA, Square squareB, int movesB, int *requiredMoves) const
{
	const Line& line = m_cache[squareA][squareB];

	for (const Entry& cache : line)
		if ((movesA >= cache.movesA) && (movesB >= cache.movesB))
			return xstd::minimize(*requiredMoves, cache.requiredMoves + (movesA - cache.movesA) + (movesB - cache.movesB)), true;

	return false;
}

/* -------------------------------------------------------------------------- */

}
