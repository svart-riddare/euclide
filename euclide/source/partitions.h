#ifndef __EUCLIDE_PARTITIONS_H
#define __EUCLIDE_PARTITIONS_H

#include "includes.h"
#include "targets.h"

namespace euclide
{

class Partitions;

/* -------------------------------------------------------------------------- */

class Partition : public vector_ptr<Target>
{
	public :
		Partition(const Problem& problem, Color color);
		Partition(Partition& partition, const Men& men, const bitset<NumMen>& targets);
		~Partition();
		
		bool refine(Partitions& partitions, int maxDepth);
		bool split(Partitions& partitions, const Men& men, const bitset<NumMen>& targets);

		int computeRequiredMoves(const Board& board);
		int computeRequiredCaptures(const Board& board);
		
		int updateRequiredMoves(bool recursive);
		int updateRequiredCaptures(bool recursive);
		const Men& updatePossibleMen(bool recursive);
		const Squares& updatePossibleSquares(bool recursive);

		bool setPossibleMen(const Men& men);
		bool setPossibleSquares(const Squares& squares);
		bool setAvailableMoves(int availableMoves);
		bool setAvailableCaptures(int availableCaptures);

		bool analyseAvailableMoves(int availableMoves);
		bool analyseAvailableCaptures(int availableCaptures);

	protected :
		static int analysePermutations(array<int, NumMen>& permutation, int size, int root, const matrix<int, NumMen, NumMen>& required, int maximum, matrix<int, NumMen, NumMen>& available);

	public :
		inline const Men& men() const
			{ return _men; }
		inline const Squares& square() const
			{ return _squares; }
		inline Color color() const
			{ return _color; }

	public :
		inline int getRequiredMoves() const
			{ return requiredMoves; }
		inline int getRequiredCaptures() const
			{ return requiredCaptures; }

		inline int getAssignedMoves() const
			{ return assignedMoves; }
		inline int getAssignedCaptures() const
			{ return assignedCaptures; }

	protected :
		static inline bool lessCandidates(const Target *targetA, const Target *targetB)
			{ return targetA->candidates() < targetB->candidates(); }

	private :
		Men _men;
		Squares _squares;
		Color _color;

	private :
		int requiredMoves;
		int requiredCaptures;

		int assignedMoves;
		int assignedCaptures;
};

/* -------------------------------------------------------------------------- */

class Partitions : public vector<Partition> {};

/* -------------------------------------------------------------------------- */

}

#endif
