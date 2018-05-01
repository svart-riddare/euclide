#include "targets.h"

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

void Target::updatePossibleMen(const Men& men)
{
	_men &= men;

	if (men.count() == 1)
		_man = men.first();

	if (!men)
		throw NoSolution;
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

void Targets::update()
{
	for (bool updated = true; updated; )
	{
		updated = false;
		for (const Target& target : *this)
			updated |= target.applyPigeonHolePrinciple(*this);
	}
}

/* -------------------------------------------------------------------------- */
/* -- TargetPartition                                                      -- */
/* -------------------------------------------------------------------------- */

bool TargetPartition::merge(const Target& target)
{
	if (_men && !(_men & target.men()))
		return false;

	_squares.set(target.square());
	_men |= target.men();

	return true;
}

/* -------------------------------------------------------------------------- */
/* -- TargetPartitions                                                     -- */
/* -------------------------------------------------------------------------- */

TargetPartitions::TargetPartitions(const Targets& targets)
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
					updated = pool.reset(target), true;
		}

		push_back(partition);
	}
}

/* -------------------------------------------------------------------------- */

}
