#include "targets.h"
#include "pieces.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */
/* -- Target                                                               -- */
/* -------------------------------------------------------------------------- */

Target::Target(Glyph glyph, Square square)
{
	m_glyph = glyph;
	m_color = Euclide::color(glyph);

	m_square = square;

	m_requiredMoves = 0;
	m_requiredCaptures = 0;

	m_men.set();
	m_man = -1;
}

/* -------------------------------------------------------------------------- */

const Men& Target::updatePossibleMen(const Men& men)
{
	m_men &= men;

	if (men.count() == 1)
		m_man = men.first();

	if (!men)
		throw NoSolution;

	return m_men;
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
				if (target.men() & m_men)
					updated = (target.updatePossibleMen(~m_men), true);

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
			requiredMoves += men[k]->requiredMoves(squares[k]);
			requiredCaptures += men[k]->requiredCaptures(squares[k]);
		}

		xstd::minimize(minRequiredMoves, requiredMoves);
		xstd::minimize(minRequiredCaptures, requiredCaptures);

		if ((minRequiredMoves <= m_requiredMoves) && (minRequiredCaptures <= m_requiredCaptures))
			break;

	} while (std::next_permutation(squares, squares + s));

	/* -- Update required moves and captures -- */

	assert(minRequiredMoves >= m_requiredMoves);
	assert(minRequiredCaptures >= m_requiredCaptures);

	int assignedMoves = 0, assignedCaptures = 0;
	for (int k = 0; k < m; k++)
	{
		assignedMoves += men[k]->requiredMoves();
		assignedCaptures += men[k]->requiredCaptures();
	}

	m_unassignedRequiredMoves = minRequiredMoves - assignedMoves;
	m_unassignedRequiredCaptures = minRequiredCaptures - assignedCaptures;

	m_requiredMoves = std::max(m_requiredMoves, minRequiredMoves);
	m_requiredCaptures = std::max(m_requiredCaptures, minRequiredCaptures);
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
