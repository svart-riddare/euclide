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

void TwoPieceCache::add(Square squareA, int movesA, Square squareB, int movesB, int requiredMoves, bool exact)
{
	Line& line = m_cache[squareA][squareB];

	/* -- This is our new cache entry -- */

	const Entry entry = { short(movesA), short(movesB), requiredMoves };

	/* -- First three cache entries are for general case, last will be used for eact matches -- */

	if (!exact)
	{
		/* -- First cache entry is for overall minimum -- */

		if (movesA + movesB < line[0].movesA + line[0].movesB)
			line[0] = entry;

		/* -- Second and third entries are for minimum moves for one of the two pieces -- */

		if (movesA <= movesB)
		{
			if ((movesA < line[1].movesA) || ((movesA == line[1].movesA) && (movesB < line[1].movesB)))
				line[1] = entry;
		}
		else
		{ 
			if ((movesB < line[2].movesB) || ((movesB == line[2].movesB) && (movesA < line[2].movesA)))
				line[2] = entry;
		}
	}
	else
	{
		/* -- Update entries if both moves are equal or lower -- */

		if ((movesA <= line[3].movesA) && (movesB <= line[3].movesB))
			line[3] = entry;
	}
}

/* -------------------------------------------------------------------------- */

bool TwoPieceCache::hit(Square squareA, int movesA, Square squareB, int movesB, int *requiredMoves) const
{
	const Line& line = m_cache[squareA][squareB];

	/* -- Check in first tree entries if we have already reached this configuration in less or equal moves -- */

	for (int entry = 0; entry < 3; entry++)
	{
		const Entry& cache = line[entry];
		if ((movesA >= cache.movesA) && (movesB >= cache.movesB))
			return xstd::minimize(*requiredMoves, cache.requiredMoves + (movesA - cache.movesA) + (movesB - cache.movesB)), true;
	}

	/* -- Last cache entry is for exact match in the number of moves -- */

	const Entry& cache = line[3];
	if ((movesA == cache.movesA) && (movesB == cache.movesB))
		return xstd::minimize(*requiredMoves, cache.requiredMoves);

	/* -- Cache miss -- */

	return false;
}

/* -------------------------------------------------------------------------- */

void TwoPieceCache::add(Square squareA, int movesA, Square squareB, int movesB)
{
	add(squareA, movesA, squareB, movesB, 0);
}

/* -------------------------------------------------------------------------- */

bool TwoPieceCache::hit(Square squareA, int movesA, Square squareB, int movesB) const
{
	const Line& line = m_cache[squareA][squareB];

	for (const Entry& cache : line)
		if ((movesA >= cache.movesA) && (movesB >= cache.movesB))
			return true;

	return false;
}

/* -------------------------------------------------------------------------- */

void TwoPieceCache::add(const Position& position, int requiredMoves)
{
	add(position.squares[0], position.moves[0], position.squares[1], position.moves[1], requiredMoves);
}

/* -------------------------------------------------------------------------- */

bool TwoPieceCache::hit(const Position& position, int *requiredMoves) const
{
	return hit(position.squares[0], position.moves[0], position.squares[1], position.moves[1], requiredMoves);
}

/* -------------------------------------------------------------------------- */

void TwoPieceCache::add(const Position& position)
{
	add(position.squares[0], position.moves[0], position.squares[1], position.moves[1], 0);
}

/* -------------------------------------------------------------------------- */

bool TwoPieceCache::hit(const Position& position) const
{
	return hit(position.squares[0], position.moves[0], position.squares[1], position.moves[1]);
}

/* -------------------------------------------------------------------------- */

}
