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

	requiredMoves = 0;
	requiredCaptures = 0;
}

/* -------------------------------------------------------------------------- */

Destination::Destination(const Destination& destination)
{
	_square = destination.square();
	_color = destination.color();
	_man = destination.man();
	_superman = destination.superman();
	_captured = destination.captured();

	requiredMoves = destination.getRequiredMoves();
	requiredCaptures = destination.getRequiredCaptures();
}

/* -------------------------------------------------------------------------- */

int Destination::computeRequiredMoves(const Board& board)
{
	int distance = board.distance(_man, _superman, _color, _square);
	if (distance > requiredMoves)
		requiredMoves = distance;

	return requiredMoves;
}

/* -------------------------------------------------------------------------- */

int Destination::computeRequiredCaptures(const Board& board)
{
	int captures = board.captures(_man, _superman, _color, _square);
	if (captures > requiredCaptures)
		requiredCaptures = captures;

	return requiredCaptures;
}

/* -------------------------------------------------------------------------- */

}
