#include "captures.h"
#include "pieces.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */
/* -- Capture                                                              -- */
/* -------------------------------------------------------------------------- */

Capture::Capture(Color color, Men men, Men xmen)
{
	m_glyphs.set([=](Glyph glyph) { return Euclide::color(glyph) == color; });
	m_glyph = Empty;
	m_color = color;

	m_squares.set();
	m_square = Nowhere;

	m_requiredMoves = 0;
	m_requiredCaptures = 0;

	m_men = men;
	m_man = -1;

	m_xmen = xmen;
	m_xman = -1;
}

/* -------------------------------------------------------------------------- */

bool Capture::updatePossibleSquares(Squares squares)
{
	if ((m_squares & squares) == m_squares)
		return false;

	m_squares &= squares;
	if (m_squares.count() == 1)
		m_square = m_squares.first();

	if (!m_squares)
		throw NoSolution;

	return true;
}

/* -------------------------------------------------------------------------- */

bool Capture::updatePossibleMen(Men men, Men xmen)
{
	if (((m_men & men) == m_men) && ((m_xmen & xmen) == m_xmen))
		return false;

	m_men &= men;
	if (m_men.count() == 1)
		m_man = m_men.first();

	m_xmen &= xmen;
	if (m_xmen.count() == 1)
		m_xman = m_xmen.first();

	if (!m_men || !m_xmen)
		throw NoSolution;

	return true;
}

/* -------------------------------------------------------------------------- */
/* -- Captures                                                             -- */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */

}
