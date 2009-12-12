#include "destinations.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Destination::Destination(Square square, Color color, Man man, Superman superman, bool captured)
	: _square(square), _color(color), _man(man), _superman(superman), _captured(captured)
{
	assert(square.isValid());
	assert(color.isValid());
	assert(man.isValid());
	assert(superman.isValid());
	assert(superman.man() == man);

	_requiredMoves = 0;
	_requiredCaptures = 0;
}

/* -------------------------------------------------------------------------- */

Destination::Destination(const Destination& destination)
{
	_square = destination.square();
	_color = destination.color();
	_man = destination.man();
	_superman = destination.superman();
	_captured = destination.captured();

	_requiredMoves = destination.requiredMoves();
	_requiredCaptures = destination.requiredCaptures();
}

/* -------------------------------------------------------------------------- */

int Destination::computeRequiredMoves(const Board& board)
{
	int distance = board.distance(_man, _superman, _color, _square);
	maximize(_requiredMoves, distance);

	return _requiredMoves;
}

/* -------------------------------------------------------------------------- */

int Destination::computeRequiredCaptures(const Board& board)
{
	int captures = board.captures(_man, _superman, _color, _square);
	maximize(_requiredCaptures, captures);

	return _requiredCaptures;
}

/* -------------------------------------------------------------------------- */

}
