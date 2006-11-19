#include "partitions.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Partition::Partition(const Problem& problem, Color color)
	: _color(color)
{
	/* -- Initialize member variables -- */

	_men.set();
	_squares.set();
	
	requiredMoves = 0;
	requiredCaptures = 0;

	/* -- Create list of targets -- */

	reserve(NumMen);

	/* -- Occupied squares are obvious targets -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (problem[square].isColor(color))
			push_back(new Target(problem[square], square));

	assert(size() <= NumMen);

	/* -- Other targets are captures -- */

	Squares captures;
	
	if (size() < NumMen)	
		captures.set();

	while (size() < NumMen)
		push_back(new Target(color, captures));

	/* -- This single partition groups all men -- */

	_men.set();
}

/* -------------------------------------------------------------------------- */

Partition::Partition(Partition& partition, const Men& men, const bitset<NumMen>& targets)
	: _color(partition.color())
{
	/* -- Initialize member variables -- */

	_men = men;
	_squares = partition._squares;

	requiredMoves = 0;
	requiredCaptures = 0;

	/* -- Move some targets from the given partition to the new one -- */

	reserve(targets.count());

	for (int source = 0, destination = 0; source < (int)partition.size(); source++)
	{
		if (targets[source])
			push_back(partition[source]);
		else
			partition[destination++] = partition[source];
	}

	partition.resize(partition.size() - targets.count());

	/* -- Update state -- */

	updatePossibleSquares(true);
	updateRequiredMoves(true);
	updateRequiredCaptures(true);
}

/* -------------------------------------------------------------------------- */

Partition::~Partition()
{
#if 0
	for (iterator target = begin(); target != end(); target++)
		delete (Target *)(*target);
#endif
}

/* -------------------------------------------------------------------------- */

bool Partition::refine(Partitions& partitions, int maxDepth)
{
	if (maxDepth >= (int)size())
		maxDepth = (int)size() - 1;

	/* -- Sort targets by number of possible men -- */

	std::sort(begin(), end(), lessCandidates);

	/* -- Look for targets with a single candidate -- */

	if (maxDepth < 1)
		return false;

	if (front()->candidates() <= 1)
		return split(partitions, front()->men(), 1);

	/* -- Look for pair of targets with same two candidates -- */

	if (maxDepth < 2)
		return false;

	int numTargets = 0;
	while ((numTargets < (int)size()) && (at(numTargets)->candidates() <= 2))
		numTargets++;

	for (int k1 = 0; k1 < numTargets; k1++)
		for (int k2 = k1 + 1; k2 < numTargets; k2++)
			if (at(k1)->men() == at(k2)->men())
				return split(partitions, at(k1)->men(), (1 << k1) | (1 << k2));

	/* -- Look for targets trios with three different candidates -- */

	if (maxDepth < 3)
		return false;

	while ((numTargets < (int)size()) && (at(numTargets)->candidates() <= 3))
		numTargets++;

	for (int k1 = 0; k1 < (numTargets - 2); k1++)
	{
		Men men1 = at(k1)->men();

		for (int k2 = k1 + 1; k2 < (numTargets - 1); k2++)
		{
			Men men2 = men1 | at(k2)->men();
			if (men2.count() > 3)
				continue;

			for (int k3 = k2 + 1; k3 < numTargets; k3++)
			{
				Men men3 = men2 | at(k3)->men();

				if (men3.count() == 3)
					return split(partitions, men3, (1 << k1) | (1 << k2) | (1 << k3));
			}
		}
	}

	/* -- General case -- */

	array<Men, NumMen> men;
	array<int, NumMen> k;

	for (int depth = 4; depth < maxDepth; depth++)
	{
		/* -- Count number of suitable targets -- */

		while ((numTargets < (int)size()) && (at(numTargets)->candidates() <= depth))
			numTargets++;

		if (numTargets < depth)
			continue;

		/* -- Initialize indices and men sets -- */

		men[0] = at(k[0] = 0)->men();
		for (int n = 1; n < depth; n++)
			men[n] = men[n - 1] | at(k[n] = n)->men();

		/* -- Loop until there are no more combinations -- */

		for ( ; ; )
		{
			int n = depth - 1;

			/* -- Is it possible to split the current subset ? -- */

			if ((int)men[n].count() == depth)
			{
				bitset<NumMen> targets;
				for (int i = 0; i < depth; i++)
					targets.set(k[i]);

				return split(partitions, men[n], targets);
			}

			/* -- Move on to the next subset having the same size -- */

			for ( ; n >= 0; n--)
				if (k[n] <= numTargets - (depth - n))
					break;

			if (n < 0)
				break;

			if (n == 0)
				men[n] = at(++k[n])->men();
			else
				men[n] = men[n - 1] | at(++k[n])->men();

			while (++n < depth)
				men[n] = men[n - 1] | at(k[n] = k[n - 1] + 1)->men();
		}
	}

	/* -- No partitions updated -- */

	return false;
}

/* -------------------------------------------------------------------------- */

bool Partition::split(Partitions& partitions, const Men& men, const bitset<NumMen>& targets)
{
	/* -- Create new partition -- */

	partitions.push_back(Partition(*this, men, targets));

	/* -- Update some state variable -- */

	requiredMoves = 0;
	requiredCaptures = 0;

	/* -- Set possible men -- */

	Men xmen = men;
	_men &= xmen.flip();

	for (iterator target = begin(); target != end(); target++)
		target->setPossibleMen(xmen);

	/* -- Update state -- */

	updatePossibleSquares(true);
	updateRequiredMoves(true);
	updateRequiredCaptures(true);

	return true;
}

/* -------------------------------------------------------------------------- */

int Partition::computeRequiredMoves(const Board& board)
{
	int requiredMoves = 0;

	for (iterator target = begin(); target != end(); target++)
		requiredMoves += target->computeRequiredMoves(board);

	return maximize(this->requiredMoves, requiredMoves);
}

/* -------------------------------------------------------------------------- */

int Partition::computeRequiredCaptures(const Board& board)
{
	int requiredCaptures = 0;

	for (iterator target = begin(); target != end(); target++)
		requiredCaptures += target->computeRequiredCaptures(board);

	return maximize(this->requiredCaptures, requiredCaptures);
}

/* -------------------------------------------------------------------------- */

int Partition::updateRequiredMoves(bool recursive)
{
	int requiredMoves = 0;

	for (iterator target = begin(); target != end(); target++)
		requiredMoves += recursive ? target->updateRequiredMoves() : target->getRequiredMoves();

	return maximize(this->requiredMoves, requiredMoves);
}

/* -------------------------------------------------------------------------- */

int Partition::updateRequiredCaptures(bool recursive)
{
	int requiredCaptures = 0;

	for (iterator target = begin(); target != end(); target++)
		requiredCaptures += recursive ? target->updateRequiredCaptures() : target->getRequiredCaptures();

	return maximize(this->requiredCaptures, requiredCaptures);
}

/* -------------------------------------------------------------------------- */

const Men& Partition::updatePossibleMen(bool recursive)
{
	_men.reset();

	for (iterator target = begin(); target != end(); target++)
		_men |= recursive ? target->updatePossibleMen() : target->men();

	return _men;
}

/* -------------------------------------------------------------------------- */

const Squares& Partition::updatePossibleSquares(bool recursive)
{
	_squares.reset();

	for (iterator target = begin(); target != end(); target++)
		_squares |= recursive ? target->updatePossibleSquares() : target->squares();

	return _squares;
}

/* -------------------------------------------------------------------------- */

bool Partition::setPossibleMen(const Men& men)
{
	Men xmen = men;
	xmen.flip() &= _men;

	if (xmen.none())
		return false;

	_men &= men;

	bool modified = false;
	for (iterator target = begin(); target != end(); target++)
		if (target->setPossibleMen(men))
			modified = true;

	if (!modified)
		return false;

	updatePossibleSquares(false);
	updateRequiredMoves(false);
	updateRequiredCaptures(false);

	return true;
}

/* -------------------------------------------------------------------------- */

bool Partition::setPossibleSquares(const Squares& squares)
{
	Squares xsquares = squares;
	xsquares.flip() &= _squares;

	if (xsquares.none())
		return false;

	_squares &= squares;

	bool modified = false;
	for (iterator target = begin(); target != end(); target++)
		if (target->setPossibleSquares(squares))
			modified = true;

	if (modified)
		return false;

	updatePossibleMen(false);
	updateRequiredMoves(false);
	updateRequiredCaptures(false);

	return true;
}

/* -------------------------------------------------------------------------- */

bool Partition::setAvailableMoves(int availableMoves)
{
	int freeMoves = availableMoves - getRequiredMoves();

	bool modified = false;
	for (iterator target = begin(); target != end(); target++)
		if (target->setAvailableMoves(target->getRequiredMoves() + freeMoves))
			modified = true;

	if (!modified)
		return false;

	updatePossibleMen(false);
	updatePossibleSquares(false);
	updateRequiredCaptures(false);

	return true;
}

/* -------------------------------------------------------------------------- */

bool Partition::setAvailableCaptures(int availableCaptures)
{
	int freeCaptures = availableCaptures - getRequiredCaptures();

	bool modified = false;
	for (iterator target = begin(); target != end(); target++)
		if (target->setAvailableCaptures(target->getRequiredCaptures() + freeCaptures))
			modified = true;

	if (!modified)
		return false;

	updatePossibleMen(false);
	updatePossibleSquares(false);
	updateRequiredMoves(false);

	return true;
}

/* -------------------------------------------------------------------------- */

}
