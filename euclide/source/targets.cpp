#include "targets.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */

Target::Target(Glyph glyph, Square square)
{
	_glyph = glyph;
	_color = Euclide::color(glyph);

	_square = square;

	_requiredMoves = 0;
	_requiredCaptures = 0;

	_men.set();
	_man = -1;
}

/* -------------------------------------------------------------------------- */

void Target::updatePossibleMen(const Men& men)
{
	_men &= men;

	if (men.count() == 1)
		_man = men.first();

	if (!men)
		throw NoSolution;
}

/* -------------------------------------------------------------------------- */

}
