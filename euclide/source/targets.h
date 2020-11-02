#ifndef __EUCLIDE_TARGETS_H
#define __EUCLIDE_TARGETS_H

#include "includes.h"

namespace Euclide
{

class Captures;
class Targets;
class Pieces;

/* -------------------------------------------------------------------------- */
/* -- Target                                                               -- */
/* -------------------------------------------------------------------------- */

class Target
{
	public:
		Target(Glyph glyph, Square square, Men men);

		int updateRequiredMoves(int requiredMoves)
			{ return xstd::maximize(m_requiredMoves, requiredMoves); }
		int updateRequiredCaptures(int requiredCaptures)
			{ return xstd::maximize(m_requiredCaptures, requiredCaptures); }

		bool updatePossibleMen(Men men);

		bool applyPigeonHolePrinciple(Targets& targets, Captures& captures) const;

	public:
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

		inline Men men() const
			{ return m_men; }
		inline Man man() const
			{ return m_man; }

	private:
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
	public:
		bool update(Captures& captures);
};

/* -------------------------------------------------------------------------- */

}

#endif
