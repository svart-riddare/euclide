#ifndef __EUCLIDE_PARTITIONS_H
#define __EUCLIDE_PARTITIONS_H

#include "includes.h"
#include "targets.h"

namespace euclide
{

class Partitions;

/* -------------------------------------------------------------------------- */

class Partition : public vector<Target *>
{
	public :
		Partition();
		Partition(Targets& targets);
		
		bool update(Partitions& partitions, int maxDepth);
		bool split(Partitions& partitions, const Men& men, const int ks[]);

		bool split(Partitions& partitions, const Men& men, int k1);
		bool split(Partitions& partitions, const Men& men, int k1, int k2);
		bool split(Partitions& partitions, const Men& men, int k1, int k2, int k3);
		bool split(Partitions& partitions, const Men& men, int k1, int k2, int k3, int k4);

	public :
		inline const Men& men() const
			{ return _men; }

	protected :
		static bool lessCandidates(const Target *targetA, const Target *targetB);

	private :
		Men _men;
};

/* -------------------------------------------------------------------------- */

class Partitions : public vector<Partition>
{
	public :
		Partitions(Targets& targets);

		bool refine(bool quick = true);

	private :
};

/* -------------------------------------------------------------------------- */

}

#endif
