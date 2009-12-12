#ifndef __EUCLIDE_PARTITIONS_H
#define __EUCLIDE_PARTITIONS_H

#include "includes.h"
#include "targets.h"

namespace euclide
{

class Partitions;

/* -------------------------------------------------------------------------- */

/**
 * \class Partition
 * A partition is a list of targets that may be fullfilled by the same subset of
 * pieces. At initialization, there is a single partition made of sixteen (the 
 * initial number of pieces on the board) targets. As solving goes on, this 
 * partition is split into distinct partitions with non-intersecting subsets
 * of pieces. The ultimate goal is to reach sixteen partitions composed of a
 * single target.
 */

/**
 * \class Partitions
 * Partition set.
 */

/* -------------------------------------------------------------------------- */

class Partition : public Targets
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
		const Supermen& updatePossibleSupermen(bool recursive);

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
		inline const Supermen& supermen() const
			{ return _supermen; }
		inline const Squares& square() const
			{ return _squares; }
		inline Color color() const
			{ return _color; }

	public :
		inline int requiredMoves() const
			{ return _requiredMoves; }
		inline int requiredCaptures() const
			{ return _requiredCaptures; }

		inline int assignedMoves() const
			{ return _assignedMoves; }
		inline int assignedCaptures() const
			{ return _assignedCaptures; }

	protected :
		static inline bool lessCandidates(const Target *targetA, const Target *targetB)
			{ return targetA->candidates() < targetB->candidates(); }

	private :
		Men _men;                 /**< Pieces that may fullfill partition targets. The number of pieces must equal the number of targets. */
		Supermen _supermen;       /**< Promotion pieces involved by partition targets. */
		Squares _squares;         /**< Final destination squares for partition targets. */
		Color _color;             /**< Partition color. */

	private :
		int _requiredMoves;       /**< Required numner of moves to fullfill all partition targets. */
		int _requiredCaptures;    /**< Required number of captures to fullfill all partition targets. */

		int _assignedMoves;       /**< Number of required moves that were assigned to a specific target within the partition. */
		int _assignedCaptures;    /**< Number of required captures that were assigned to a specific target within the partition. */
};

/* -------------------------------------------------------------------------- */

class Partitions : public vector_ptr<Partition> {};

/* -------------------------------------------------------------------------- */

}

#endif
