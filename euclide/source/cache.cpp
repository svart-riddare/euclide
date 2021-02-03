#include "cache.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */
/* -- TwoPieceFastCache                                                   -- */
/* -------------------------------------------------------------------------- */

TwoPieceFastCache::TwoPieceFastCache()
{
	const Entry entry = { Infinity, Infinity };
	const Line line = { entry, entry, entry };
	m_cache.fill(line);
}

/* -------------------------------------------------------------------------- */

void TwoPieceFastCache::add(Square squareA, int movesA, Square squareB, int movesB)
{
	Line& line = m_cache[squareA][squareB];

	/* -- This is our new cache entry -- */

	const Entry entry = { short(movesA), short(movesB) };

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

/* -------------------------------------------------------------------------- */

bool TwoPieceFastCache::hit(Square squareA, int movesA, Square squareB, int movesB) const
{
	const Line& line = m_cache[squareA][squareB];

	for (const Entry& cache : line)
		if ((movesA >= cache.movesA) && (movesB >= cache.movesB))
			return true;

	return false;
}

/* -------------------------------------------------------------------------- */

void TwoPieceFastCache::add(const Position& position)
{
	add(position.squares[0], position.moves[0], position.squares[1], position.moves[1]);
}

/* -------------------------------------------------------------------------- */

bool TwoPieceFastCache::hit(const Position& position) const
{
	return hit(position.squares[0], position.moves[0], position.squares[1], position.moves[1]);
}


/* -------------------------------------------------------------------------- */
/* -- TwoPieceFullCache                                                    -- */
/* -------------------------------------------------------------------------- */

template<typename State>
TwoPieceFullCache<State>::TwoPieceFullCache()
{
#if 1
	const Entry entry = { false, Infinity, Infinity, Infinity, State(), State() };
	const Line line = { entry, entry, entry, entry, entry, entry, entry, entry };
	m_cache.fill(line);
#else
	for (Square squareA : AllSquares())
		for (Square squareB : AllSquares())
			for (Entry& entry : m_cache[squareA][squareB])
				entry.valid = false;
#endif

	m_id = 0;
}

/* -------------------------------------------------------------------------- */

template<typename State>
void TwoPieceFullCache<State>::add(Square squareA, int movesA, State stateA, Square squareB, int movesB, State stateB, int requiredMoves)
{
	Line& line = m_cache[squareA][squareB];

	/* -- This is our new cache entry -- */

	const Entry entry = { true, short(movesA), short(movesB), short(requiredMoves), stateA, stateB };

	/* -- Find insertion point -- */

	for (Entry& cache : line)
	{
		/* -- Already present in cache -- */

		const bool hit = cache.valid && (stateA == cache.stateA) && (stateB == cache.stateB) && (movesA >= cache.movesA) && (movesB >= cache.movesB);
		if (hit)
			return;

		/* -- Better entry -- */

		const bool best = cache.valid && (stateA == cache.stateA) && (stateB == cache.stateB) && (movesA <= cache.movesA) && (movesB <= cache.movesB);
		if (best || !cache.valid)
			return void(cache = entry);
	}

	/* -- No space left for this cache line -- */

	line[m_id++ % line.size()] = entry;
}

/* -------------------------------------------------------------------------- */

template<typename State>
bool TwoPieceFullCache<State>::hit(Square squareA, int movesA, State stateA, Square squareB, int movesB, State stateB, int *requiredMoves) const
{
	const Line& line = m_cache[squareA][squareB];

	/* -- Check if we have already reached this configuration in less or equal moves -- */

	for (const Entry& cache : line)
	{
		if (!cache.valid)
			break;

		if ((stateA == cache.stateA) && (stateB == cache.stateB))
			if ((movesA >= cache.movesA) && (movesB >= cache.movesB))
				return xstd::minimize(*requiredMoves, cache.requiredMoves + (movesA - cache.movesA) + (movesB - cache.movesB)), true;
	}

	/* -- Cache miss -- */

	return false;
}

/* -------------------------------------------------------------------------- */

template<typename State>
void TwoPieceFullCache<State>::add(const Position& position, int requiredMoves)
{
	add(position.squares[0], position.moves[0], position.states[0], position.squares[1], position.moves[1], position.states[1], requiredMoves);
}

/* -------------------------------------------------------------------------- */

template<typename State>
bool TwoPieceFullCache<State>::hit(const Position& position, int *requiredMoves) const
{
	return hit(position.squares[0], position.moves[0], position.states[0], position.squares[1], position.moves[1], position.states[1], requiredMoves);
}

/* -------------------------------------------------------------------------- */

template class TwoPieceFullCache<Squares>;

/* -------------------------------------------------------------------------- */

}
