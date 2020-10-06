#ifndef __EUCLIDE_CAPTURES_H
#define __EUCLIDE_CAPTURES_H

#include "includes.h"

namespace Euclide
{

class Targets;
class Pieces;

/* -------------------------------------------------------------------------- */
/* -- Capture                                                              -- */
/* -------------------------------------------------------------------------- */

class Capture
{
	public:
		Capture(Color color, Men men, Men xmen);

		int updateRequiredMoves(int requiredMoves)
			{ return xstd::maximize(m_requiredMoves, requiredMoves); }
		int updateRequiredCaptures(int requiredCaptures)
			{ return xstd::maximize(m_requiredCaptures, requiredCaptures); }

		bool updatePossibleSquares(Squares squares);
		bool updatePossibleMen(Men men, Men xmen);

	public:
		inline Glyphs glyphs() const
			{ return m_glyphs; }
		inline Glyph glyph() const
			{ return m_glyph; }
		inline Color color() const
			{ return m_color; }

		inline Squares squares() const
			{ return m_squares; }
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

		inline Men xmen() const
			{ return m_xmen; }
		inline Man xman() const
			{ return m_xman; }

	private:
		Glyphs m_glyphs;           /**< Possible glyphs captured. */
		Glyph m_glyph;             /**< Glyph captured, if known. */
		Color m_color;             /**< Captured piece color. */

		Squares m_squares;         /**< Possible capture squares. */
		Square m_square;           /**< Capture square, if known. */

		int m_requiredMoves;       /**< Required moves to reach capture squares. */
		int m_requiredCaptures;    /**< Required captures to reach capture square. */

		Men m_men;                 /**< Possible captured pieces. */
		Man m_man;                 /**< Captured piece, if known. */

		Men m_xmen;                /**< Possible pieces that may have performed this capture. */
		Man m_xman;                /**< Capturer piece, if known. */
};

/* -------------------------------------------------------------------------- */
/* -- Captures                                                             -- */
/* -------------------------------------------------------------------------- */

class Captures : public std::vector<Capture>
{
};

/* -------------------------------------------------------------------------- */

}

#endif
