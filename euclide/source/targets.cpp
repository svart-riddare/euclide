#include "targets.h"
#include "pieces.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */
/* -- Target                                                               -- */
/* -------------------------------------------------------------------------- */

Target::Target(Glyph glyph, Square square)
{
	_glyph = glyph;
	_color = Euclide::color(glyph);

	_square = square;

	_requiredMoves = 0;
	_requiredCaptures = 0;

	_men.set();
	_man = -1;
}

/* -------------------------------------------------------------------------- */

const Men& Target::updatePossibleMen(const Men& men)
{
	_men &= men;

	if (men.count() == 1)
		_man = men.first();

	if (!men)
		throw NoSolution;

	return _men;
}

/* -------------------------------------------------------------------------- */

bool Target::applyPigeonHolePrinciple(Targets& targets) const
{
	bool updated = false;

	Squares squares;
	for (const Target& target : targets)
		if (target.men() == _men)
			squares.set(target.square());

	if (squares.count() >= _men.count())
		for (Target& target : targets)
			if (target.men() != _men)
				if (target.men() & _men)
					updated = (target.updatePossibleMen(~_men), true);

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
	_requiredMoves = 0;
	_requiredCaptures = 0;

	_unassignedRequiredMoves = 0;
	_unassignedRequiredCaptures = 0;
}

/* -------------------------------------------------------------------------- */

bool TargetPartition::merge(const Target& target)
{
	if (_men && !(_men & target.men()))
		return false;

	_squares.set(target.square());
	_men |= target.men();

	_requiredMoves += target.requiredMoves();
	_requiredCaptures += target.requiredCaptures();

	return true;
}

/* -------------------------------------------------------------------------- */

void TargetPartition::assign(const Pieces& pieces)
{
	/* -- List men and target squares -- */

	Square squares[MaxPieces]; int s = 0;
	for (Square square : ValidSquares(_squares))
		squares[s++] = square;

	const Piece *men[MaxPieces]; int m = 0;
	for (Man man : ValidMen(_men))
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

		if ((minRequiredMoves <= _requiredMoves) && (minRequiredCaptures <= _requiredCaptures))
			break;		

	} while (std::next_permutation(squares, squares + s));

	/* -- Update required moves and captures -- */

	assert(minRequiredMoves >= _requiredMoves);
	assert(minRequiredCaptures >= _requiredCaptures);

	int assignedMoves = 0, assignedCaptures = 0;
	for (int k = 0; k < m; k++)
	{
		assignedMoves += men[k]->requiredMoves();
		assignedCaptures += men[k]->requiredCaptures();
	}

	_unassignedRequiredMoves = minRequiredMoves - assignedMoves;
	_unassignedRequiredCaptures = minRequiredCaptures - assignedCaptures;

	_requiredMoves = std::max(_requiredMoves, minRequiredMoves);
	_requiredCaptures = std::max(_requiredCaptures, minRequiredCaptures);
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

	_requiredMoves = 0;
	_requiredCaptures = 0;
	_unassignedRequiredMoves = 0;
	_unassignedRequiredCaptures = 0;

	for (TargetPartition& partition : *this)
	{
		partition.assign(pieces);

		_requiredMoves += partition.requiredMoves();
		_requiredCaptures += partition.requiredCaptures();
		_unassignedRequiredMoves += partition.unassignedRequiredMoves();
		_unassignedRequiredCaptures += partition.unassignedRequiredCaptures();
	}

	/* -- Fill quick access map -- */

	_map.fill(&_null);
	for (const TargetPartition& partition : *this)
		for (Man man : ValidMen(partition.men()))
			_map[man] = &partition;
}

/* -------------------------------------------------------------------------- */

}
