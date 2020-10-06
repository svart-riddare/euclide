#include "targets.h"
#include "pieces.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */
/* -- Target                                                               -- */
/* -------------------------------------------------------------------------- */

Target::Target(Glyph glyph, Square square, Men men)
{
	m_glyph = glyph;
	m_color = Euclide::color(glyph);

	m_square = square;

	m_requiredMoves = 0;
	m_requiredCaptures = 0;

	m_men = men;
	m_man = -1;
}

/* -------------------------------------------------------------------------- */

bool Target::updatePossibleMen(Men men)
{
	if ((m_men & men) == m_men)
		return false;

	m_men &= men;
	if (m_men.count() == 1)
		m_man = m_men.first();

	if (!m_men)
		throw NoSolution;

	return true;
}

/* -------------------------------------------------------------------------- */

bool Target::applyPigeonHolePrinciple(Targets& targets) const
{
	bool updated = false;

	Squares squares;
	for (const Target& target : targets)
		if (target.men() == m_men)
			squares.set(target.square());

	if (squares.count() >= m_men.count())
		for (Target& target : targets)
			if (target.men() != m_men)
				if (target.updatePossibleMen(~m_men))
					updated = true;

	return updated;
}

/* -------------------------------------------------------------------------- */
/* -- Targets                                                              -- */
/* -------------------------------------------------------------------------- */

bool Targets::update()
{
	bool reduced = false;
	for (bool updated = true; updated; reduced |= updated)
	{
		updated = false;
		for (const Target& target : *this)
			updated |= target.applyPigeonHolePrinciple(*this);
	}

	return reduced;
}

/* -------------------------------------------------------------------------- */
/* -- TargetPartition                                                      -- */
/* -------------------------------------------------------------------------- */

TargetPartition::TargetPartition()
{
	m_requiredMoves = 0;
	m_requiredCaptures = 0;

	m_unassignedRequiredMoves = 0;
	m_unassignedRequiredCaptures = 0;
}

/* -------------------------------------------------------------------------- */

bool TargetPartition::merge(const Target& target)
{
	if (m_men && !(m_men & target.men()))
		return false;

	m_squares.set(target.square());
	m_men |= target.men();

	m_requiredMoves += target.requiredMoves();
	m_requiredCaptures += target.requiredCaptures();

	return true;
}

/* -------------------------------------------------------------------------- */

void TargetPartition::assign(const Pieces& pieces)
{
	/* -- List men and target squares -- */

	Square squares[MaxPieces]; int s = 0;
	for (Square square : ValidSquares(m_squares))
		squares[s++] = square;

	const Piece *men[MaxPieces]; int m = 0;
	for (Man man : ValidMen(m_men))
		men[m++] = &pieces[man];

	if (m < s)
		throw NoSolution;

	/* -- Find required moves and captures by trying all permutations of men and squares -- */

	int minRequiredMoves = Infinity;
	int minRequiredCaptures = Infinity;

	do
	{
		int requiredMoves = 0, requiredCaptures = 0;

		for (int k = 0; k < s; k++)
		{
			requiredMoves += std::max(men[k]->requiredMovesTo(squares[k]), men[k]->requiredMoves());
			requiredCaptures += std::max(men[k]->requiredCapturesTo(squares[k]), men[k]->requiredCaptures());
		}

		xstd::minimize(minRequiredMoves, requiredMoves);
		xstd::minimize(minRequiredCaptures, requiredCaptures);

		if ((minRequiredMoves <= m_requiredMoves) && (minRequiredCaptures <= m_requiredCaptures))
			break;

	} while (std::next_permutation(men, men + m));

	/* -- Update required moves and captures -- */

	assert(minRequiredMoves >= m_requiredMoves);
	assert(minRequiredCaptures >= m_requiredCaptures);

	m_assignedRequiredMoves = 0;
	for (int k = 0; k < m; k++)
		m_assignedRequiredMoves += men[k]->requiredMoves();

	m_assignedRequiredCaptures = 0;
	for (int k = 0; k < m; k++)
		m_assignedRequiredCaptures += men[k]->requiredCaptures();

	m_unassignedRequiredMoves = minRequiredMoves - m_assignedRequiredMoves;
	m_unassignedRequiredCaptures = minRequiredCaptures - m_assignedRequiredCaptures;

	assert(m_unassignedRequiredMoves >= 0);
	assert(m_unassignedRequiredCaptures >= 0);

	m_requiredMoves = std::max(m_requiredMoves, minRequiredMoves);
	m_requiredCaptures = std::max(m_requiredCaptures, minRequiredCaptures);
}

/* -------------------------------------------------------------------------- */

bool TargetPartition::disjoint(const Pieces& pieces, int freeMoves, int freeCaptures, Targets& targets) const
{
	/* -- List men and target squares -- */

	Square squares[MaxPieces]; int s = 0;
	for (Square square : ValidSquares(m_squares))
		squares[s++] = square;

	const Piece *men[MaxPieces]; int m = 0;
	for (Man man : ValidMen(m_men))
		men[m++] = &pieces[man];

	/* -- Early exit if there are too many permutations -- */

	if (m >= 8)
		return false;

	/* -- Get available moves and captures -- */

	const int availableMoves = m_assignedRequiredMoves + m_unassignedRequiredMoves + freeMoves;
	const int availableCaptures = m_assignedRequiredCaptures + m_unassignedRequiredCaptures + freeCaptures;

	/* -- Try every men permutation -- */

	Men destinations[NumSquares];

	do
	{
		int requiredMoves = 0, requiredCaptures = 0;

		for (int k = 0; k < s; k++)
		{
			requiredMoves += std::max(men[k]->requiredMovesTo(squares[k]), men[k]->requiredMoves());
			requiredCaptures += std::max(men[k]->requiredCapturesTo(squares[k]), men[k]->requiredCaptures());
		}

		if ((requiredMoves <= availableMoves) && (requiredCaptures <= availableCaptures))
			for (int k = 0; k < s; k++)
				destinations[squares[k]] |= Man(std::distance(&pieces[0], men[k]));

	} while (std::next_permutation(men, men + m));

	/* -- Update targets -- */

	bool updated = false;
	for (Target& target : targets)
		if (m_squares[target.square()])
			if (target.updatePossibleMen(destinations[target.square()]))
				updated = true;

	return updated;
}

/* -------------------------------------------------------------------------- */
/* -- TargetPartitions                                                     -- */
/* -------------------------------------------------------------------------- */

TargetPartitions::TargetPartitions(const Pieces& pieces, const Targets& targets)
{
	reserve(targets.size());

	/* -- Label unused targets -- */

	typedef BitSet<int, NumSquares> TargetPool;
	TargetPool pool(TargetPool::mask(targets.size()));

	/* -- Build partitions -- */

	while (pool)
	{
		TargetPartition partition;

		for (bool updated = true; updated; )
		{
			updated = false;
			for (int target : TargetPool::BitSetRange(pool))
				if (partition.merge(targets[target]))
					updated = (pool.reset(target), true);
		}

		push_back(partition);
	}

	/* -- Assign required moves and captures -- */

	m_requiredMoves = 0;
	m_requiredCaptures = 0;
	m_unassignedRequiredMoves = 0;
	m_unassignedRequiredCaptures = 0;

	for (TargetPartition& partition : *this)
	{
		partition.assign(pieces);

		m_requiredMoves += partition.requiredMoves();
		m_requiredCaptures += partition.requiredCaptures();
		m_unassignedRequiredMoves += partition.unassignedRequiredMoves();
		m_unassignedRequiredCaptures += partition.unassignedRequiredCaptures();
	}

	/* -- Fill quick access map -- */

	m_map.fill(&m_null);
	for (const TargetPartition& partition : *this)
		for (Man man : ValidMen(partition.men()))
			m_map[man] = &partition;
}

/* -------------------------------------------------------------------------- */

}
