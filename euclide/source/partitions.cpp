#include "partitions.h"
#include "captures.h"
#include "targets.h"
#include "pieces.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */
/* -- Partition                                                            -- */
/* -------------------------------------------------------------------------- */

Partition::Partition()
{
	m_requiredMoves = 0;
	m_requiredCaptures = 0;

	m_assignedRequiredMoves = 0;
	m_assignedRequiredCaptures = 0;

	m_unassignedRequiredMoves = 0;
	m_unassignedRequiredCaptures = 0;
}

/* -------------------------------------------------------------------------- */

bool Partition::merge(const Target& target)
{
	if (m_men && !(m_men & target.men()))
		return false;

	m_destinations.push_back({ target.square(), target.square(), target.glyph(), target.glyph(), target.men(), &target, nullptr });

	m_squares.set(target.square());
	m_glyphs.set(target.glyph());
	m_men |= target.men();

	m_requiredMoves += target.requiredMoves();
	m_requiredCaptures += target.requiredCaptures();

	return true;
}

/* -------------------------------------------------------------------------- */

bool Partition::merge(const Capture& capture)
{
	if (m_men && !(m_men & capture.men()))
		return false;

	m_destinations.push_back({ capture.squares(), capture.square(), capture.glyphs(), capture.glyph(), capture.men(), nullptr, &capture });

	m_captures |= capture.squares();
	m_glyphs |= capture.glyphs();
	m_men |= capture.men();

	m_requiredMoves += capture.requiredMoves();
	m_requiredCaptures += capture.requiredCaptures();

	return true;
}

/* -------------------------------------------------------------------------- */

void Partition::assign(const Pieces& pieces)
{
	const int m = m_men.count();
	const int s = m_destinations.size();

	/* -- No solution if there are less men than destinations -- */

	if (m < s)
		throw NoSolution;

	/* -- Count assigned moves and captures -- */

	m_assignedRequiredMoves = 0;
	for (Man man : ValidMen(m_men))
		m_assignedRequiredMoves += pieces[man].requiredMoves();

	m_assignedRequiredCaptures = 0;
	for (Man man : ValidMen(m_men))
		m_assignedRequiredCaptures += pieces[man].requiredCaptures();

	m_requiredMoves = std::max(m_requiredMoves, m_assignedRequiredMoves);
	m_requiredCaptures = std::max(m_requiredCaptures, m_assignedRequiredCaptures);

	/* -- Find required moves and captures by trying all men permutations, if reasonable -- */

	constexpr int MaxMen = 8;
	int minRequiredMoves = (m > MaxMen) ? m_requiredMoves : Infinity;
	int minRequiredCaptures = (m > MaxMen) ? m_requiredCaptures : Infinity;

	if (m <= MaxMen)
	{
		struct Cost { int moves; int captures; };
		matrix<Cost, MaxMen, MaxMen> costs;

		/* -- Precompute required moves and captures for all possibilities -- */

		int i = 0;
		for (Man man : ValidMen(m_men))
		{
			for (int j = 0; j < s; j++)
			{
				const Destination& destination = m_destinations[j];

				costs[i][j].moves = destination.men[man] ? std::max(destination.glyph ? pieces[man].requiredMovesTo(destination.squares, destination.glyph) : pieces[man].requiredMovesTo(destination.squares), pieces[man].requiredMoves()) : Infinity;
				costs[i][j].captures = destination.men[man] ? std::max(destination.glyph ? pieces[man].requiredCapturesTo(destination.squares, destination.glyph) : pieces[man].requiredCapturesTo(destination.squares), pieces[man].requiredCaptures()) : Infinity;
			}

			i += 1;
		}

		/* -- Find required moves and captures by trying all permutations of men and squares -- */

		int indices[MaxMen];
		for (int k = 0; k < m; k++)
			indices[k] = k;

		do
		{
			int requiredMoves = 0, requiredCaptures = 0;

			for (int k = 0; k < s; k++)
			{
				requiredMoves += costs[indices[k]][k].moves;
				requiredCaptures += costs[indices[k]][k].captures;
			}

			xstd::minimize(minRequiredMoves, requiredMoves);
			xstd::minimize(minRequiredCaptures, requiredCaptures);

			if ((minRequiredMoves <= m_requiredMoves) && (minRequiredCaptures <= m_requiredCaptures))
				break;

		} while (std::next_permutation(indices, indices + m));
	}

	/* -- Update required moves and captures -- */

	assert(minRequiredMoves >= m_requiredMoves);
	assert(minRequiredCaptures >= m_requiredCaptures);

	m_unassignedRequiredMoves = minRequiredMoves - m_assignedRequiredMoves;
	m_unassignedRequiredCaptures = minRequiredCaptures - m_assignedRequiredCaptures;

	assert(m_unassignedRequiredMoves >= 0);
	assert(m_unassignedRequiredCaptures >= 0);

	m_requiredMoves = std::max(m_requiredMoves, minRequiredMoves);
	m_requiredCaptures = std::max(m_requiredCaptures, minRequiredCaptures);
}

/* -------------------------------------------------------------------------- */

bool Partition::split(Pieces& pieces, int freeMoves, int freeCaptures, Targets& targets, Captures& captures) const
{
	/* -- Get available moves and captures -- */

	const int availableMoves = m_assignedRequiredMoves + m_unassignedRequiredMoves + freeMoves;
	const int availableCaptures = m_assignedRequiredCaptures + m_unassignedRequiredCaptures + freeCaptures;

	/* -- Split partition, working with subsets of men if there are too many possible men --  */

	static constexpr int MaxMen = 8;

	bool updated = false;
	if (m_men.count() > MaxMen)
		for (Glyph glyph : ValidGlyphs(m_glyphs))
			if (split(pieces, glyph, availableMoves, availableCaptures, targets, captures))
				updated = true;

	if (split(pieces, Empty, availableMoves, availableCaptures, targets, captures))
		updated = true;

	/* -- Done -- */

	return updated;
}

/* -------------------------------------------------------------------------- */

bool Partition::split(Pieces& pieces, Glyph glyph, int availableMoves, int availableCaptures, Targets& targets, Captures& captures) const
{
	/* -- List men -- */

	Men selected;
	for (const Destination& destination : m_destinations)
		if (!glyph || (destination.glyph == glyph))
			selected |= destination.men;

	int m = 0;
	array<Man, MaxPieces> men;
	for (Man man : ValidMen(selected))
		men[m++] = man;

	/* -- List destinations -- */

	int s = 0;
	array<size_t, MaxPieces> destinations;
	for (size_t k = 0; k < m_destinations.size(); k++)
		if (!glyph || (m_destinations[k].glyph == glyph))
			destinations[s++] = k;

	/* -- This should not happen -- */

	if (m < s)
		throw NoSolution;

	/* -- Early exit if computations are too expensive -- */

	static constexpr int MaxMen = 8;
	if (m > MaxMen)
		return false;

	/* -- Prefetch required moves and captures -- */

	struct Cost { int moves; int captures; };
	matrix<Cost, MaxMen, MaxMen> costs;

	for (int i = 0; i < m; i++)
	{
		const Man man = men[i];

		for (int j = 0; j < s; j++)
		{
			const Destination& destination = m_destinations[destinations[j]];

			costs[i][j].moves = destination.men[man] ? std::max(destination.glyph ? pieces[man].requiredMovesTo(destination.squares, destination.glyph) : pieces[man].requiredMovesTo(destination.squares), pieces[man].requiredMoves()) : Infinity;
			costs[i][j].captures = destination.men[man] ? std::max(destination.glyph ? pieces[man].requiredCapturesTo(destination.squares, destination.glyph) : pieces[man].requiredCapturesTo(destination.squares), pieces[man].requiredCaptures()) : Infinity;
		}
	}

	/* -- Try all permutations -- */

	array<Men, MaxPieces> permutations;
	array<Squares, MaxPieces> squares;

	int indices[MaxMen];
	for (int k = 0; k < m; k++)
		indices[k] = k;

	do
	{
		int requiredMoves = 0, requiredCaptures = 0;

		for (int k = 0; k < s; k++)
		{
			requiredMoves += costs[indices[k]][k].moves;
			requiredCaptures += costs[indices[k]][k].captures;
		}

		if ((requiredMoves <= availableMoves) && (requiredCaptures <= availableCaptures))
		{
			for (int k = 0; k < m; k++)
				permutations[k].set(men[indices[k]]);

			for (int k = 0; k < s; k++)
			{
				const int myAvailableMoves = costs[indices[k]][k].moves + (availableMoves - requiredMoves);
				const int myAvailableCaptures = costs[indices[k]][k].captures + (availableCaptures - requiredCaptures);

				const Destination& destination = m_destinations[destinations[k]];
				const Squares reachableSquares = destination.glyph ? pieces[men[indices[k]]].reachableSquares(destination.squares, myAvailableMoves, myAvailableCaptures, destination.glyph) : pieces[men[indices[k]]].reachableSquares(destination.squares, myAvailableMoves, myAvailableCaptures);

				squares[indices[k]] |= reachableSquares;
			}
		}

	} while (std::next_permutation(indices, indices + m));

	/* -- Update targets and captures -- */

	BitSet<ptrdiff_t, MaxPieces> selectedTargets;
	BitSet<ptrdiff_t, MaxPieces> selectedCaptures;

	bool updated = false;
	for (int k = 0; k < s; k++)
	{
		const Destination& destination = m_destinations[destinations[k]];

		if (destination.target)
		{
			const ptrdiff_t index = destination.target - targets.data();
			selectedTargets.set(index);

			Target& target = targets[index];
			if (target.updatePossibleMen(permutations[k]))
				updated = true;
		}

		if (destination.capture)
		{
			const ptrdiff_t index = destination.capture - captures.data();
			selectedCaptures.set(index);

			Capture& capture = captures[index];
			if (capture.updatePossibleMen(permutations[k], capture.xmen()))
				updated = true;
		}
	}

	if (m == s)
	{
		for (size_t k = 0; k < targets.size(); k++)
			if (!selectedTargets[k])
				if (targets[k].updatePossibleMen(~selected))
					updated = true;

		for (size_t k = 0; k < captures.size(); k++)
			if (!selectedCaptures[k])
				if (captures[k].updatePossibleMen(~selected, captures[k].xmen()))
					updated = true;
	}

	/* -- Update pieces -- */

	for (int i = 0; i < m; i++)
		if (glyph)
			pieces[men[i]].piece(glyph)->setPossibleSquares(squares[i]);
		else
			pieces[men[i]].setPossibleSquares(squares[i]);

	/* -- Done -- */

	return updated;
}

/* -------------------------------------------------------------------------- */
/* -- Partitions                                                           -- */
/* -------------------------------------------------------------------------- */

Partitions::Partitions(const Pieces& pieces, const Targets& targets, const Captures& captures)
{
	assert(targets.size() + captures.size() == pieces.size());
	reserve(pieces.size());

	/* -- Label targets and captures -- */

	typedef BitSet<int, MaxPieces> Pool;
	Pool targetPool(Pool::mask(targets.size()));
	Pool capturePool(Pool::mask(captures.size()));

	/* -- Build partitions -- */

	while (targetPool || capturePool)
	{
		Partition partition;

		for (bool updated = true; updated; )
		{
			updated = false;
			for (int target : Pool::BitSetRange(targetPool))
				if (partition.merge(targets[target]))
					updated = (targetPool.reset(target), true);

			for (int capture : Pool::BitSetRange(capturePool))
				if (partition.merge(captures[capture]))
					updated = (capturePool.reset(capture), true);
		}

		push_back(partition);
	}

	/* -- Assign required moves and captures -- */

	m_requiredMoves = 0;
	m_requiredCaptures = 0;
	m_unassignedRequiredMoves = 0;
	m_unassignedRequiredCaptures = 0;

	for (Partition& partition : *this)
	{
		partition.assign(pieces);

		m_requiredMoves += partition.requiredMoves();
		m_requiredCaptures += partition.requiredCaptures();
		m_unassignedRequiredMoves += partition.unassignedRequiredMoves();
		m_unassignedRequiredCaptures += partition.unassignedRequiredCaptures();
	}

	/* -- Fill quick access map -- */

	m_map.fill(&m_null);
	for (const Partition& partition : *this)
		for (Man man : ValidMen(partition.men()))
			m_map[man] = &partition;
}

/* -------------------------------------------------------------------------- */

}
