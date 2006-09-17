#include "targets.h"
#include "position.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Target::Target(Glyph glyph, Square square)
	: _glyph(glyph), _square(square), _cause(NULL, 0)
{
	assert(glyph.isValid());
	assert(square.isValid());

	_squares.set(square);
	_men.set();

	requiredMoves = 0;
	requiredCaptures = 0;
}

/* -------------------------------------------------------------------------- */

Target::Target(Glyph glyph, const Squares& squares)
	: _glyph(glyph), _square(UndefinedSquare), _cause(NULL, 0)
{
	assert(glyph == NoGlyph);

	_squares = squares;
	_men.set();

	requiredMoves = 0;
	requiredCaptures = 0;
}

/* -------------------------------------------------------------------------- */

void Target::setCause(const Cause& cause)
{
	if (cause != _cause)
	{
		assert(_cause == Cause(NULL, 0));
		assert(_glyph == NoGlyph);
		
		_cause = cause;
	}
}

/* -------------------------------------------------------------------------- */

void Target::updateMen(const Men& men)
{
	_men &= men;
	
	if (_men.none())
		abort(NoSolution);
}

/* -------------------------------------------------------------------------- */

void Target::updateMen(const array<Men, NumSquares>& men)
{
	Men group;

	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (_squares.test(square))
			group |= men[square];

	updateMen(group);
}

/* -------------------------------------------------------------------------- */

void Target::updateSquares(const Squares& squares)
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

bool Target::isCausedBy(const Cause& cause) const
{
	if (cause == _cause)
		return true;

	return false;
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

	/* -- Other targets are captures -- */

	if ((int)size() < NumMen)
		captures.set();

	while ((int)size() < NumMen)
		push_back(Target(NoGlyph, captures));

	/* -- Initialize member variables -- */

	requiredMoves = 0;
	requiredCaptures = 0;
}

/* -------------------------------------------------------------------------- */

void Targets::update(const Destinations& destinations)
{
	Men empty;
	array<Men, NumSquares> men(empty);
	array<Men, NumSquares> ghosts(empty);

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

void Targets::refine(const Board& board, const Pieces& pieces)
{
	const Destinations& destinations = pieces.getDestinations();
	const Targets& targets = pieces.getTargets();

	/* -- Scan opponent targets for required captures -- */

	for (Targets::const_iterator target = targets.begin(); target != targets.end(); target++)
	{
		if (!target->getRequiredCaptures())
			continue;

		vector<Squares> captures(target->getRequiredCaptures());

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
		{
			Target::Cause cause(&*target, k);

			/* -- Find which target is dedicated to this capture if any -- */

			Targets::iterator target = std::find_if(begin(), end(), boost::bind(&Target::isCausedBy, _1, cause));
			if (target == end())
				for (target = begin(); target != end(); target++)
					if (!target->isOccupied())
						if (target->isCausedBy(Target::Cause(NULL, 0)))
							break;

			if (target == end())
				abort(NoSolution);

			target->setCause(cause);
			target->updateSquares(captures[k]);

		}
	}
}

/* -------------------------------------------------------------------------- */

}
