#include "partitions.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Partition::Partition()
{
	reserve(NumMen);
}

/* -------------------------------------------------------------------------- */

Partition::Partition(Targets& targets)
{
	assert(targets.size() == NumMen);

	reserve(NumMen);
	for (vector<Target>::iterator target = targets.begin(); target != targets.end(); target++)
		push_back(&*target);

	_men.set();
}

/* -------------------------------------------------------------------------- */

bool Partition::update(Partitions& partitions, int maxDepth)
{
	if (maxDepth >= (int)size())
		maxDepth = (int)size() - 1;

	/* -- Sort targets by number of possible men -- */

	std::sort(begin(), end(), lessCandidates);

	/* -- Look for targets with a single candidate -- */

	if (maxDepth < 1)
		return false;

	if (front()->candidates() <= 1)
		if (split(partitions, front()->men(), 0))
			return true;

	/* -- Look for pair of targets with same two candidates -- */

	if (maxDepth < 2)
		return false;

	int numTargets = 0;
	while ((numTargets < (int)size()) && (at(numTargets)->candidates() <= 2))
		numTargets++;

	for (int k1 = 0; k1 < numTargets; k1++)
		for (int k2 = k1 + 1; k2 < numTargets; k2++)
			if (at(k1)->men() == at(k2)->men())
				if (split(partitions, at(k1)->men(), k1, k2))
					return true;

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
					if (split(partitions, men3, k1, k2, k3))
						return true;
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
				if (split(partitions, men[n], k.data()))
					return true;

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

bool Partition::split(Partitions& partitions, const Men& men, const int ks[])
{
	array<bool, NumMen> discard(true);

	for (int k = 0; k < (int)men.count(); k++)
		discard[ks[k]] = false;

	/* -- Create a new partition -- */

	Partition partition;

	for (int source = 0, destination = 0; source < (int)size(); source++)
	{
		if (discard[source])
			partition.push_back(at(source));
		else
			at(destination++) = at(source);
	}

	resize(men.count());

	/* -- Add new partition to partition list -- */

	assert(partition.size() > 0);
	partitions.push_back(partition);
			
	/* -- Update new partition targets -- */

	Men xmen = men;
	xmen.flip();

	for (vector<Target *>::const_iterator target = partition.begin(); target != partition.end(); target++)
		(*target)->updateMen(xmen);

	return true;
}

/* -------------------------------------------------------------------------- */

bool Partition::split(Partitions& partitions, const Men& men, int k1)
{
	return split(partitions, men, &k1);
}

/* -------------------------------------------------------------------------- */

bool Partition::split(Partitions& partitions, const Men& men, int k1, int k2)
{
	int ks[2] = { k1, k2 };
	return split(partitions, men, ks);
}

/* -------------------------------------------------------------------------- */

bool Partition::split(Partitions& partitions, const Men& men, int k1, int k2, int k3)
{
	int ks[3] = { k1, k2, k3 };
	return split(partitions, men, ks);
}

/* -------------------------------------------------------------------------- */

bool Partition::split(Partitions& partitions, const Men& men, int k1, int k2, int k3, int k4)
{
	int ks[4] = { k1, k2, k3, k4 };
	return split(partitions, men, ks);
}

/* -------------------------------------------------------------------------- */

bool Partition::lessCandidates(const Target *targetA, const Target *targetB)
{
	return targetA->candidates() < targetB->candidates();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

Partitions::Partitions(Targets& targets)
{
	push_back(Partition(targets));
}

/* -------------------------------------------------------------------------- */

bool Partitions::refine(bool quick)
{
	bool modified = false;

	for (int k = 0; k < (int)size(); k++)
		if (at(k).update(*this, quick ? 3 : NumMen))
			modified = true;
	
	return modified;
}

/* -------------------------------------------------------------------------- */

}


