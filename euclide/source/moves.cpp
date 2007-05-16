#include "moves.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Move::Move(Square from, Square to, Color color, Man man, Superman superman)
	: _from(from), _to(to), _color(color), _man(man), _superman(superman)
{
	assert(from.isValid() || (from == UndefinedSquare));
	assert(to.isValid() || (to == UndefinedSquare));
	assert(from.isValid() || to.isValid());

	assert(color.isValid());
	assert(man.isValid());
	assert(superman.isValid());
}

/* -------------------------------------------------------------------------- */

}
