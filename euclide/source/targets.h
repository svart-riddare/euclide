#ifndef __EUCLIDE_TARGETS_H
#define __EUCLIDE_TARGETS_H

#include "includes.h"

namespace Euclide
{

class Targets;

/* -------------------------------------------------------------------------- */
/* -- Target                                                               -- */
/* -------------------------------------------------------------------------- */

class Target
{
	public :
		Target(Glyph glyph, Square square);

		int updateRequiredMoves(int requiredMoves)
			{ return xstd::maximize(_requiredMoves, requiredMoves); }
		int updateRequiredCaptures(int requiredCaptures)
			{ return xstd::maximize(_requiredCaptures, requiredCaptures); }

		void updatePossibleMen(const Men& men);		
		
		bool applyPigeonHolePrinciple(Targets& targets) const;

	public :
		inline Glyph glyph() const
			{ return _glyph; }
		inline Color color() const
			{ return _color; }
		inline Square square() const
			{ return _square; }

		inline int requiredMoves() const
			{ return _requiredMoves; }
		inline int requiredCaptures() const
			{ return _requiredCaptures; }

		inline const Men& men() const
			{ return _men; }
		inline Man man() const
			{ return _man; }

	private :
		Glyph _glyph;             /**< Target glyph. */
		Color _color;             /**< Glyph color. */
		Square _square;           /**< Target square. */

		int _requiredMoves;       /**< Minimum number of moves required to reach this target. */
		int _requiredCaptures;    /**< Minimum number of captures required to reach this target. */

		Men _men;                 /**< Possible pieces that may reach this target. */
		Man _man;                 /**< Piece that reaches this target, if known, -1 otherwise. */
};

/* -------------------------------------------------------------------------- */
/* -- Targets                                                              -- */
/* -------------------------------------------------------------------------- */

class Targets : public std::vector<Target>
{
	public :
		void update();
};

/* -------------------------------------------------------------------------- */
/* -- TargetPartition                                                      -- */
/* -------------------------------------------------------------------------- */

class TargetPartition
{
	public :
		bool merge(const Target& target);

	public :
		inline const Squares& squares() const
			{ return _squares; }
		inline const Men& men() const
			{ return _men; }

	private :
		Squares _squares;    /**< Target squares for these targets. */
		Men _men;            /**< Possible men for these targets. */
};

/* -------------------------------------------------------------------------- */
/* -- TargetPartitions                                                     -- */
/* -------------------------------------------------------------------------- */

class TargetPartitions : public std::vector<TargetPartition>
{
	public :
		TargetPartitions(const Targets& targets);
};

/* -------------------------------------------------------------------------- */

}

#endif
