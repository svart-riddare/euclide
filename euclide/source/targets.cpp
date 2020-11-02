#include "targets.h"
#include "captures.h"
#include "pieces.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */
/* -- Target                                                               -- */
/* -------------------------------------------------------------------------- */

Target::Target(Glyph glyph, Square square, Men men)
{
	m_glyph = glyph;
	m_color = Euclide::color(glyph);

	m_square = square;

	m_requiredMoves = 0;
	m_requiredCaptures = 0;

	m_men = men;
	m_man = -1;
}

/* -------------------------------------------------------------------------- */

bool Target::updatePossibleMen(Men men)
{
	if ((m_men & men) == m_men)
		return false;

	m_men &= men;
	if (m_men.count() == 1)
		m_man = m_men.first();

	if (!m_men)
		throw NoSolution;

	return true;
}

/* -------------------------------------------------------------------------- */

bool Target::applyPigeonHolePrinciple(Targets& targets, Captures& captures) const
{
	bool updated = false;

	Squares squares;
	for (const Target& target : targets)
		if (target.men() == m_men)
			squares.set(target.square());

	if (squares.count() >= m_men.count())
	{
		for (Target& target : targets)
			if (target.men() != m_men)
				if (target.updatePossibleMen(~m_men))
					updated = true;

		for (Capture& capture : captures)
			if (capture.updatePossibleMen(~m_men, capture.xmen()))
				updated = true;
	}

	return updated;
}

/* -------------------------------------------------------------------------- */
/* -- Targets                                                              -- */
/* -------------------------------------------------------------------------- */

bool Targets::update(Captures& captures)
{
	bool reduced = false;
	for (bool updated = true; updated; reduced |= updated)
	{
		updated = false;
		for (const Target& target : *this)
			if (target.applyPigeonHolePrinciple(*this, captures))
				updated = true;
	}

	return reduced;
}

/* -------------------------------------------------------------------------- */

}
