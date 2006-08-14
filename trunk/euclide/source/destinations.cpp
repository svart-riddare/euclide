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

	requiredMoves = 0;
	requiredCaptures = 0;
}

/* -------------------------------------------------------------------------- */

int Destination::updateRequiredMoves(const Board& board, const Castling& castling)
{
	int distance = board.distance(_man, _superman, _color, _square, castling);
	if (distance > requiredMoves)
		requiredMoves = distance;

	return requiredMoves;
}

/* -------------------------------------------------------------------------- */

int Destination::updateRequiredCaptures(const Board& board)
{
	int captures = board.captures(_man, _superman, _color, _square);
	if (captures > requiredCaptures)
		requiredCaptures = captures;

	return requiredCaptures;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

Destinations::Destinations()
{
	reserve(NumSupermen * (NumMen + NumSquares));

	requiredMovesByMan.assign(0);
	requiredMovesBySquare.assign(0);
	requiredMovesByLiveSquare.assign(0);
	requiredMovesByDeadSquare.assign(0);

	requiredCapturesByMan.assign(0);
	requiredCapturesBySquare.assign(0);
	requiredCapturesByLiveSquare.assign(0);
	requiredCapturesByDeadSquare.assign(0);
}

/* -------------------------------------------------------------------------- */

Destinations& Destinations::operator+=(const Destination& destination)
{
	push_back(destination);
	return *this;
}

/* -------------------------------------------------------------------------- */

void Destinations::updateRequiredMoves(const Board& board, const Castling& castling)
{
	requiredMovesByMan.assign(infinity);
	requiredMovesBySquare.assign(infinity);
	requiredMovesByLiveSquare.assign(infinity);
	requiredMovesByDeadSquare.assign(infinity);

	for (iterator I = begin(); I != end(); I++)
	{
		int requiredMoves = I->updateRequiredMoves(board, castling);

		minimize(requiredMovesByMan[I->man()], requiredMoves);
		minimize(requiredMovesBySquare[I->square()], requiredMoves);

		if (I->captured())
			minimize(requiredMovesByDeadSquare[I->square()], requiredMoves);
		else
			minimize(requiredMovesByLiveSquare[I->square()], requiredMoves);
	}
}

/* -------------------------------------------------------------------------- */

void Destinations::updateRequiredCaptures(const Board& board)
{
	requiredCapturesByMan.assign(infinity);
	requiredCapturesBySquare.assign(infinity);
	requiredCapturesByLiveSquare.assign(infinity);
	requiredCapturesByDeadSquare.assign(infinity);

	for (iterator I = begin(); I != end(); I++)
	{
		int requiredCaptures = I->updateRequiredCaptures(board);

		minimize(requiredCapturesByMan[I->man()], requiredCaptures);
		minimize(requiredCapturesBySquare[I->square()], requiredCaptures);

		if (I->captured())
			minimize(requiredCapturesByDeadSquare[I->square()], requiredCaptures);
		else
			minimize(requiredCapturesByLiveSquare[I->square()], requiredCaptures);
	}

	std::replace(requiredCapturesByMan.begin(), requiredCapturesByMan.end(), infinity, 0);
}

/* -------------------------------------------------------------------------- */

struct SetManSquarePredicate
{
	Man _man; Square _square; bool _captured;
	SetManSquarePredicate(Man man, Square square, bool captured)
		: _man(man), _square(square), _captured(captured) {}

	bool operator()(const Destination& destination)
	{
		if (destination.man() == _man)
			if ((destination.square() != _square) || (destination.captured() != _captured))
				return true;

		return false;
	}
};

bool Destinations::setManSquare(Man man, Square square, bool captured)
{
	return remove(SetManSquarePredicate(man, square, captured));
}

/* -------------------------------------------------------------------------- */

struct SetAvailableMovesPredicate
{
	const Destinations *_destinations; int _availableMovesForMen; int _availableMovesForSquares;
	SetAvailableMovesPredicate(const Destinations *destinations, int availableMovesForMen, int availableMovesForSquares)
		: _destinations(destinations), _availableMovesForMen(availableMovesForMen), _availableMovesForSquares(availableMovesForSquares) {}

	bool operator()(const Destination& destination)
	{
		int availableMovesForMan = _availableMovesForMen + _destinations->getRequiredMovesByMan()[destination.man()];
		int availableMovesForSquare = _availableMovesForSquares + _destinations->getRequiredMovesBySquare(false)[destination.square()];
		int availableMoves = std::min(availableMovesForMan, availableMovesForSquare);

		return (availableMoves < destination.getRequiredMoves());
	}
};

bool Destinations::setAvailableMoves(int availableMovesForMen, int availableMovesForSquares)
{
	return remove(SetAvailableMovesPredicate(this, availableMovesForMen, availableMovesForSquares));
}

/* -------------------------------------------------------------------------- */

struct SetAvailableCapturesPredicate
{
	const Destinations *_destinations; int _availableCapturesForMen; int _availableCapturesForSquares;
	SetAvailableCapturesPredicate(const Destinations *destinations, int availableCapturesForMen, int availableCapturesForSquares)
		: _destinations(destinations), _availableCapturesForMen(availableCapturesForMen), _availableCapturesForSquares(availableCapturesForSquares) {}

	bool operator()(const Destination& destination)
	{
		int availableCapturesForMan = _availableCapturesForMen + _destinations->getRequiredCapturesByMan()[destination.man()];
		int availableCapturesForSquare = _availableCapturesForSquares + _destinations->getRequiredCapturesBySquare(false)[destination.square()];
		int availableCaptures = std::min(availableCapturesForMan, availableCapturesForSquare);

		return (availableCaptures < destination.getRequiredCaptures());
	}
};

bool Destinations::setAvailableCaptures(int availableCapturesForMen, int availableCapturesForSquares)
{
	return remove(SetAvailableCapturesPredicate(this, availableCapturesForMen, availableCapturesForSquares));
}

/* -------------------------------------------------------------------------- */

}
