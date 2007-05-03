#include "captures.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Capture::Capture(Target *target)
	: Pointer<Target>(target), _man(UndefinedMan), _glyph(UndefinedGlyph), _color(!target->color())
{
	/* -- Initialize member variables -- */

	_men.set();

	requiredMoves = 0;
	requiredCaptures = 0;

	this->target = NULL;
	_required = false;
}

/* -------------------------------------------------------------------------- */

void Capture::assign(Target *target, bool required)
{
	assert(!this->target);

	this->target = target;
	_required = required;
}

/* -------------------------------------------------------------------------- */

}