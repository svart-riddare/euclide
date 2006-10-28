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
		Partition(const Problem& problem, Color color);
		Partition(Partition& partition, const Men& men, const bitset<NumMen>& targets);
		~Partition();
		
		bool refine(Partitions& partitions, int maxDepth);
		bool split(Partitions& partitions, const Men& men, const bitset<NumMen>& targets);

		int computeRequiredMoves(const Board& board, const Castling& castling);
		int computeRequiredCaptures(const Board& board);
		
		int updateRequiredMoves();
		int updateRequiredCaptures();
		const Men& updatePossibleMen();
		const Squares& updatePossibleSquares();

		bool setPossibleMen(const Men& men);
		bool setPossibleSquares(const Squares& squares);
		bool setAvailableMoves(int numAvailableMoves);
		bool setAvailableCaptures(int numAvailableCaptures);

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
};

/* -------------------------------------------------------------------------- */

class Partitions : public vector<Partition> {};

/* -------------------------------------------------------------------------- */

}

#endif
