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
	requiredMovesByOccupiedSquare.assign(0);
	requiredMovesByUnoccupiedSquare.assign(0);

	requiredCapturesByMan.assign(0);
	requiredCapturesBySquare.assign(0);
	requiredCapturesByOccupiedSquare.assign(0);
	requiredCapturesByUnoccupiedSquare.assign(0);
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
	requiredMovesByOccupiedSquare.assign(infinity);
	requiredMovesByUnoccupiedSquare.assign(infinity);

	for (iterator I = begin(); I != end(); I++)
	{
		int requiredMoves = I->updateRequiredMoves(board, castling);

		minimize(requiredMovesByMan[I->man()], requiredMoves);
		minimize(requiredMovesBySquare[I->square()], requiredMoves);

		if (I->captured())
			minimize(requiredMovesByUnoccupiedSquare[I->square()], requiredMoves);
		else
			minimize(requiredMovesByOccupiedSquare[I->square()], requiredMoves);
	}
}

/* -------------------------------------------------------------------------- */

void Destinations::updateRequiredCaptures(const Board& board)
{
	requiredCapturesByMan.assign(infinity);
	requiredCapturesBySquare.assign(infinity);
	requiredCapturesByOccupiedSquare.assign(infinity);
	requiredCapturesByUnoccupiedSquare.assign(infinity);

	for (iterator I = begin(); I != end(); I++)
	{
		int requiredCaptures = I->updateRequiredCaptures(board);

		minimize(requiredCapturesByMan[I->man()], requiredCaptures);
		minimize(requiredCapturesBySquare[I->square()], requiredCaptures);

		if (I->captured())
			minimize(requiredCapturesByUnoccupiedSquare[I->square()], requiredCaptures);
		else
			minimize(requiredCapturesByOccupiedSquare[I->square()], requiredCaptures);
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
	const array<int, NumMen>& _availableMovesByMan; const array<int, NumSquares>& _availableMovesByOccupiedSquare; const array<int, NumSquares>& _availableMovesByUnoccupiedSquare;
	SetAvailableMovesPredicate(const array<int, NumMen>& availableMovesByMan, const array<int, NumSquares>& availableMovesByOccupiedSquare, const array<int, NumSquares>& availableMovesByUnoccupiedSquare)
		: _availableMovesByMan(availableMovesByMan), _availableMovesByOccupiedSquare(availableMovesByOccupiedSquare), _availableMovesByUnoccupiedSquare(availableMovesByUnoccupiedSquare) {}

	void operator=(const SetAvailableMovesPredicate&)
		{ assert(false); }

	bool operator()(const Destination& destination)
	{
		int requiredMoves = destination.getRequiredMoves();

		if (requiredMoves > _availableMovesByMan[destination.man()])
			return true;

		if (!destination.captured())
			if (requiredMoves > _availableMovesByOccupiedSquare[destination.square()])
				return true;

		if (destination.captured())
			if (requiredMoves > _availableMovesByUnoccupiedSquare[destination.square()])
				return true;

		return false;
	}
};

bool Destinations::setAvailableMoves(const array<int, NumMen>& availableMovesByMan, const array<int, NumSquares>& availableMovesByOccupiedSquare, const array<int, NumSquares>& availableMovesByUnoccupiedSquare)
{
	return remove(SetAvailableMovesPredicate(availableMovesByMan, availableMovesByOccupiedSquare, availableMovesByUnoccupiedSquare));
}

/* -------------------------------------------------------------------------- */

struct SetAvailableCapturesPredicate
{
	const array<int, NumMen>& _availableCapturesByMan; const array<int, NumSquares>& _availableCapturesByOccupiedSquare; const array<int, NumSquares>& _availableCapturesByUnoccupiedSquare;
	SetAvailableCapturesPredicate(const array<int, NumMen>& availableCapturesByMan, const array<int, NumSquares>& availableCapturesByOccupiedSquare, const array<int, NumSquares>& availableCapturesByUnoccupiedSquare)
		: _availableCapturesByMan(availableCapturesByMan), _availableCapturesByOccupiedSquare(availableCapturesByOccupiedSquare), _availableCapturesByUnoccupiedSquare(availableCapturesByUnoccupiedSquare) {}

	void operator=(const SetAvailableCapturesPredicate&)
		{ assert(false); }

	bool operator()(const Destination& destination)
	{
		int requiredCaptures = destination.getRequiredCaptures();

		if (requiredCaptures > _availableCapturesByMan[destination.man()])
			return true;

		if (!destination.captured())
			if (requiredCaptures > _availableCapturesByOccupiedSquare[destination.square()])
				return true;

		if (destination.captured())
			if (requiredCaptures > _availableCapturesByUnoccupiedSquare[destination.square()])
				return true;

		return false;
	}
};

bool Destinations::setAvailableCaptures(const array<int, NumMen>& availableCapturesByMan, const array<int, NumSquares>& availableCapturesByOccupiedSquare, const array<int, NumSquares>& availableCapturesByUnoccupiedSquare)
{
	return remove(SetAvailableCapturesPredicate(availableCapturesByMan, availableCapturesByOccupiedSquare, availableCapturesByUnoccupiedSquare));
}

/* -------------------------------------------------------------------------- */

}
