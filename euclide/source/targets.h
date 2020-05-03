#ifndef __EUCLIDE_TARGETS_H
#define __EUCLIDE_TARGETS_H

#include "includes.h"

namespace Euclide
{

class Targets;
class Pieces;

/* -------------------------------------------------------------------------- */
/* -- Target                                                               -- */
/* -------------------------------------------------------------------------- */

class Target
{
	public :
		Target(Glyph glyph, Square square);

		int updateRequiredMoves(int requiredMoves)
			{ return xstd::maximize(m_requiredMoves, requiredMoves); }
		int updateRequiredCaptures(int requiredCaptures)
			{ return xstd::maximize(m_requiredCaptures, requiredCaptures); }

		const Men& updatePossibleMen(const Men& men);

		bool applyPigeonHolePrinciple(Targets& targets) const;

	public :
		inline Glyph glyph() const
			{ return m_glyph; }
		inline Color color() const
			{ return m_color; }
		inline Square square() const
			{ return m_square; }

		inline int requiredMoves() const
			{ return m_requiredMoves; }
		inline int requiredCaptures() const
			{ return m_requiredCaptures; }

		inline const Men& men() const
			{ return m_men; }
		inline Man man() const
			{ return m_man; }

	private :
		Glyph m_glyph;             /**< Target glyph. */
		Color m_color;             /**< Glyph color. */
		Square m_square;           /**< Target square. */

		int m_requiredMoves;       /**< Minimum number of moves required to reach this target. */
		int m_requiredCaptures;    /**< Minimum number of captures required to reach this target. */

		Men m_men;                 /**< Possible pieces that may reach this target. */
		Man m_man;                 /**< Piece that reaches this target, if known, -1 otherwise. */
};

/* -------------------------------------------------------------------------- */
/* -- Targets                                                              -- */
/* -------------------------------------------------------------------------- */

class Targets : public std::vector<Target>
{
	public :
		bool update();
};

/* -------------------------------------------------------------------------- */
/* -- TargetPartition                                                      -- */
/* -------------------------------------------------------------------------- */

class TargetPartition
{
	public :
		TargetPartition();

		bool merge(const Target& target);
		void assign(const Pieces& pieces);

	public :
		inline const Squares& squares() const
			{ return m_squares; }
		inline const Men& men() const
			{ return m_men; }

		inline int requiredMoves() const
			{ return m_requiredMoves; }
		inline int requiredCaptures() const
			{ return m_requiredCaptures; }

		inline int unassignedRequiredMoves() const
			{ return m_unassignedRequiredMoves; }
		inline int unassignedRequiredCaptures() const
			{ return m_unassignedRequiredCaptures; }

	private :
		Squares m_squares;                   /**< Target squares for these targets. */
		Men m_men;                           /**< Possible men for these targets. */

		int m_requiredMoves;                 /**< Required moves. */
		int m_requiredCaptures;              /**< Required captures. */

		int m_unassignedRequiredMoves;       /**< Required moves unassigned to a specific piece. */
		int m_unassignedRequiredCaptures;    /**< Required captures unassigned to a specific piece. */
};

/* -------------------------------------------------------------------------- */
/* -- TargetPartitions                                                     -- */
/* -------------------------------------------------------------------------- */

class TargetPartitions : public std::vector<TargetPartition>
{
	public :
		TargetPartitions(const Pieces& pieces, const Targets& targets);

	public :
		inline int requiredMoves() const
			{ return m_requiredMoves; }
		inline int requiredCaptures() const
			{ return m_requiredCaptures; }

		inline int unassignedRequiredMoves() const
			{ return m_unassignedRequiredMoves; }
		inline int unassignedRequiredCaptures() const
			{ return m_unassignedRequiredCaptures; }

		inline int requiredMoves(Man man) const
			{ return m_map[man]->requiredMoves(); }
		inline int requiredCaptures(Man man) const
			{ return m_map[man]->requiredCaptures(); }

		inline int unassignedRequiredMoves(Man man) const
			{ return m_map[man]->unassignedRequiredMoves(); }
		inline int unassignedRequiredCaptures(Man man) const
			{ return m_map[man]->unassignedRequiredCaptures(); }

	private :
		int m_requiredMoves;                                /**< Required moves. */
		int m_requiredCaptures;                             /**< Required capture. */

		int m_unassignedRequiredMoves;                      /**< Required moves unassigned to a specific piece. */
		int m_unassignedRequiredCaptures;                   /**< Required captures unassigned to a specific piece. */

		array<const TargetPartition *, MaxPieces> m_map;    /**< Quick map between men and partitions. */
		TargetPartition m_null;                             /**< Null partition, used in above map. */
};

/* -------------------------------------------------------------------------- */

}

#endif
