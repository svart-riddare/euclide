#include "captures.h"
#include "targets.h"
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

bool Capture::applyPigeonHolePrinciple(Targets& targets, Captures& captures) const
{
	bool updated = false;

	const int count = xstd::sum(captures, [=](const Capture& capture) { return (capture.men() == m_men) ? 1 : 0; });
	if (count >= m_men.count())
	{
		for (Target& target : targets)
			if (target.updatePossibleMen(~m_men))
				updated = true;

		for (Capture& capture : captures)
			if (capture.men() != m_men)
				if (capture.updatePossibleMen(~m_men, capture.xmen()))
					updated = true;
	}

	return updated;
}

/* -------------------------------------------------------------------------- */
/* -- Captures                                                             -- */
/* -------------------------------------------------------------------------- */

bool Captures::update(Targets& targets)
{
	bool reduced = false;
	for (bool updated = true; updated; reduced |= updated)
	{
		updated = false;
		for (const Capture& capture : *this)
			if (capture.applyPigeonHolePrinciple(targets, *this))
				updated = true;
	}

	return reduced;
}

/* -------------------------------------------------------------------------- */

}
