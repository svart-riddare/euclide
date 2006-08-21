#include "targets.h"
#include "position.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Target::Target(Glyph glyph, Square square)
	: _glyph(glyph), _square(square)
{
	assert(glyph.isValid());
	assert(square.isValid());

	_squares.set(square);
	_men.set();

	requiredMoves = 0;
	requiredCaptures = 0;
}

/* -------------------------------------------------------------------------- */

Target::Target(Glyph glyph, const bitset<NumSquares>& squares)
	: _glyph(glyph), _square(UndefinedSquare)
{
	assert(glyph.isValid());

	_squares = squares;
	_men.set();

	requiredMoves = 0;
	requiredCaptures = 0;
}

/* -------------------------------------------------------------------------- */

void Target::updateMen(const bitset<NumMen>& men)
{
	_men &= men;
	
	if (_men.none())
		abort(NoSolution);
}

/* -------------------------------------------------------------------------- */

void Target::updateMen(const array<bitset<NumMen>, NumSquares>& men)
{
	bitset<NumMen> group;

	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (_squares.test(square))
			group |= men[square];

	updateMen(group);
}

/* -------------------------------------------------------------------------- */

void Target::updateSquares(const bitset<NumSquares>& squares)
{
	_squares &= squares;

	if (_squares.none())
		abort(NoSolution);
}

/* -------------------------------------------------------------------------- */

int Target::updateRequiredMoves(const array<int, NumSquares>& requiredMoves)
{
	this->requiredMoves = infinity;

	if (_square.isValid())
		return this->requiredMoves = requiredMoves[_square];

	/* -- Find minimum requirement -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
	{
		if (_squares[square])
		{
			minimize(this->requiredMoves, requiredMoves[square]);

			if (requiredMoves[square] >= infinity)
				_squares[square] = false;
		}
	}

	return this->requiredMoves;
}

/* -------------------------------------------------------------------------- */

int Target::updateRequiredCaptures(const array<int, NumSquares>& requiredCaptures)
{
	this->requiredCaptures = infinity;

	if (_square.isValid())
		return this->requiredCaptures = requiredCaptures[_square];

	/* -- Find minimum requirement -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
	{
		if (_squares[square])
		{
			minimize(this->requiredCaptures, requiredCaptures[square]);

			if (requiredCaptures[square] >= infinity)
				_squares[square] = false;
		}
	}

	if (_squares.none())
		abort(NoSolution);

	return this->requiredCaptures;
}

/* -------------------------------------------------------------------------- */

bool Target::isTargetOf(const Destination& destination) const
{
	if (!_men[destination.man()])
		return false;

	if (!_squares[destination.square()])
		return false;

	if (isOccupied() == destination.captured())
		return false;

	return true;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

Targets::Targets(const Problem& problem, Color color)
{
	this->color = color;

	/* -- Occupied squares are obvious targets -- */

	reserve(NumMen);
	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (problem[square].isColor(color))
			push_back(Target(problem[square], square));

	requiredMoves = 0;
	requiredCaptures = 0;

	if ((int)size() < NumMen)
		captures.set();
}

/* -------------------------------------------------------------------------- */

void Targets::update(const Destinations& destinations)
{
	array<bitset<NumMen>, NumSquares> men;
	array<bitset<NumMen>, NumSquares> ghosts;

	std::for_each(men.begin(), men.end(), std::mem_fun_ref(&bitset<NumMen>::set));
	std::for_each(ghosts.begin(), ghosts.end(), std::mem_fun_ref(&bitset<NumMen>::set));

	/* -- Scan destinations to update possible men for each target -- */
	
	for (Destinations::const_iterator destination = destinations.begin(); destination != destinations.end(); destination++)
		if (!destination->captured())
			men[destination->square()].set(destination->man());
		else
			ghosts[destination->square()].set(destination->man());

	for (Targets::iterator target = begin(); target != end(); target++)
		if (target->isOccupied())
			target->updateMen(men[target->square()]);
		else
			target->updateMen(ghosts);

	/* -- Update required moves -- */

	requiredMoves = 0;
	for (Targets::iterator target = begin(); target != end(); target++)
		requiredMoves += target->updateRequiredMoves(destinations.getRequiredMovesBySquare(!target->isOccupied()));

	/* -- Update required captures -- */

	requiredCaptures = 0;
	for (Targets::iterator target = begin(); target != end(); target++)
		requiredCaptures += target->updateRequiredCaptures(destinations.getRequiredCapturesBySquare(!target->isOccupied()));

	/* -- Update possible squares for captures -- */

	captures.reset();
	for (Targets::const_iterator target = begin(); target != end(); target++)
		if (!target->isOccupied())
			captures |= target->squares();
}

/* -------------------------------------------------------------------------- */

void Targets::reset(const Board& board, const Pieces& pieces)
{
	const Destinations& destinations = pieces.getDestinations();
	const Targets& targets = pieces.getTargets();

	/* -- Scan opponent targets for required captures -- */

	for (Targets::const_iterator target = targets.begin(); target != targets.end(); target++)
	{
		if (!target->getRequiredCaptures())
			continue;

		vector<bitset<NumSquares>> captures(target->getRequiredCaptures());

		/* -- Find all destinations compatibles with target -- */

		for (Destinations::const_iterator destination = destinations.begin(); destination != destinations.end(); destination++)
		{
			if (!target->isTargetOf(*destination))
				continue;

			assert(destination->getRequiredCaptures() >= target->getRequiredCaptures());

			/* -- We seek minimal number of captures -- */

			if (destination->getRequiredCaptures() > target->getRequiredCaptures())
				continue;

			/* -- Find squares for required captures -- */

			board.captures(destination->man(), destination->superman(), destination->color(), tables::initialSquares[destination->man()][destination->color()], destination->square(), captures);
		}

		/* -- Add these required captures as targets -- */

		for (int k = 0; k < (int)captures.size(); k++)
			push_back(Target(NoGlyph, captures[k]));
	}

	/* -- Complete target list -- */

	bitset<NumSquares> squares;
	squares.set();

	while (size() < NumMen)
		push_back(Target(NoGlyph, squares));

	/* -- There should not be more than NumMen targets -- */

	assert(size() <= NumMen);
}

/* -------------------------------------------------------------------------- */

}
