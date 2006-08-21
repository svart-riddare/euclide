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

int Destination::computeRequiredMoves(const Board& board, const Castling& castling)
{
	int distance = board.distance(_man, _superman, _color, _square, castling);
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
/* -------------------------------------------------------------------------- */

Destinations::Destinations(const Problem& problem, Color color)
{
	/* -- Initialize move and capture requirements -- */

	requiredMoves = 0;
	requiredMovesByMan.assign(0);
	requiredMovesBySquare.assign(0);
	requiredMovesByShrine.assign(0);

	requiredCaptures = 0;
	requiredCapturesByMan.assign(0);
	requiredCapturesBySquare.assign(0);
	requiredCapturesByShrine.assign(0);

	/* -- Associate with each man a possible destination -- */

	reserve(NumSupermen * (NumMen + NumSquares));

	for (Square square = FirstSquare; square <= LastSquare; square++)
	{
		Glyph glyph = problem[square];

		/* -- Associate glyphs to men -- */

		for (Man man = FirstMan; man <= LastMan; man++)
			if (tables::supermanToGlyph[man][color] == glyph)
				push_back(Destination(square, color, man, man, false));

		/* -- Handle promoted pieces -- */

		for (Superman superman = FirstPromotedMan; superman <= LastPromotedMan; superman++)
			if (tables::supermanToGlyph[superman][color] == glyph)
				for (Man man = FirstPawn; man <= LastPawn; man++)
					push_back(Destination(square, color, man, superman, false));

		/* -- Each man also have been captured on any square -- */

		for (Man man = FirstMan; man <= LastMan; man++)
			push_back(Destination(square, color, man, man, true));

		/* -- Pawns may also have been captured as promoted pieces -- */

		for (Man man = FirstPawn; man <= LastPawn; man++)
			for (Superman superman = FirstPromotedMan; superman <= LastPromotedMan; superman++)
				push_back(Destination(square, color, man, superman, true));
	}
}

/* -------------------------------------------------------------------------- */

void Destinations::computeRequiredMoves(const Board& board, const Castling& castling)
{
	std::for_each(begin(), end(), boost::bind(&Destination::computeRequiredMoves, _1, cref(board), cref(castling)));
	updateRequiredMoves();
}

/* -------------------------------------------------------------------------- */

void Destinations::computeRequiredCaptures(const Board &board)
{
	std::for_each(begin(), end(), boost::bind(&Destination::computeRequiredCaptures, _1, cref(board)));
	updateRequiredCaptures();
}

/* -------------------------------------------------------------------------- */

void Destinations::updateRequiredMoves()
{
	requiredMovesByMan.assign(infinity);
	requiredMovesBySquare.assign(infinity);
	requiredMovesByShrine.assign(infinity);

	for (iterator destination = begin(); destination != end(); destination++)
	{
		int requiredMoves = destination->getRequiredMoves();

		minimize(requiredMovesByMan[destination->man()], requiredMoves);

		if (destination->captured())
			minimize(requiredMovesByShrine[destination->square()], requiredMoves);
		else
			minimize(requiredMovesBySquare[destination->square()], requiredMoves);
	}

	requiredMoves = std::accumulate(requiredMovesByMan.begin(), requiredMovesByMan.end(), 0);
}

/* -------------------------------------------------------------------------- */

void Destinations::updateRequiredCaptures()
{
	requiredCapturesByMan.assign(infinity);
	requiredCapturesBySquare.assign(infinity);
	requiredCapturesByShrine.assign(infinity);

	for (iterator destination = begin(); destination != end(); destination++)
	{
		int requiredCaptures = destination->getRequiredCaptures();

		minimize(requiredCapturesByMan[destination->man()], requiredCaptures);

		if (destination->captured())
			minimize(requiredCapturesByShrine[destination->square()], requiredCaptures);
		else
			minimize(requiredCapturesBySquare[destination->square()], requiredCaptures);
	}

	std::replace(requiredCapturesByMan.begin(), requiredCapturesByMan.end(), infinity, 0);
	requiredCaptures = std::accumulate(requiredCapturesByMan.begin(), requiredCapturesByMan.end(), 0);
}

/* -------------------------------------------------------------------------- */

bool Destinations::setShrines(const bitset<NumSquares>& shrines)
{
	bitset<NumMen> men;
	bitset<NumSquares> squares;

	men.set();
	squares.set();

	return remove(boost::bind(&isDestinationCompatible, _1, cref(men), cref(squares), cref(shrines)));
}

/* -------------------------------------------------------------------------- */

bool Destinations::setManSquare(Man man, Square square, bool captured)
{
	bitset<NumMen> men;
	bitset<NumSquares> squares;
	bitset<NumSquares> shrines;

	men[man] = true;
	squares[square] = !captured;
	shrines[square] = captured;

	return remove(boost::bind(&isDestinationCompatible, _1, cref(men), cref(squares), cref(shrines)));
}

/* -------------------------------------------------------------------------- */

bool Destinations::setAvailableMoves(const array<int, NumMen>& availableMovesByMan, const array<int, NumSquares>& availableMovesBySquare, const array<int, NumSquares>& availableMovesByShrine)
{
	return remove(boost::bind(&isEnoughMovesForDestination, _1, cref(availableMovesByMan), cref(availableMovesBySquare), cref(availableMovesByShrine)));
}

/* -------------------------------------------------------------------------- */

bool Destinations::setAvailableCaptures(const array<int, NumMen>& availableCapturesByMan, const array<int, NumSquares>& availableCapturesBySquare, const array<int, NumSquares>& availableCapturesByShrine)
{
	return remove(boost::bind(&isEnoughCapturesForDestination, _1, cref(availableCapturesByMan), cref(availableCapturesBySquare), cref(availableCapturesByShrine)));
}

/* -------------------------------------------------------------------------- */

bool Destinations::isDestinationCompatible(const Destination& destination, const bitset<NumMen>& men, const bitset<NumSquares>& squares, const bitset<NumSquares>& shrines)
{
	if (!men[destination.man()])
		return true;

	if ((destination.captured() ? shrines : squares)[destination.square()])
		return true;

	return false;
}

/* -------------------------------------------------------------------------- */

bool Destinations::isEnoughMovesForDestination(const Destination& destination, const array<int, NumMen>& availableMovesByMan, const array<int, NumSquares>& availableMovesBySquare, const array<int, NumSquares>& availableMovesByShrine)
{
	int requiredMoves = destination.getRequiredMoves();

	if (requiredMoves > availableMovesByMan[destination.man()])
		return false;

	if (requiredMoves > (destination.captured() ? availableMovesByShrine : availableMovesBySquare)[destination.square()])
		return false;

	return true;
}

/* -------------------------------------------------------------------------- */

bool Destinations::isEnoughCapturesForDestination(const Destination& destination, const array<int, NumMen>& availableCapturesByMan, const array<int, NumSquares>& availableCapturesBySquare, const array<int, NumSquares>& availableCapturesByShrine)
{
	int requiredCaptures = destination.getRequiredCaptures();

	if (requiredCaptures > availableCapturesByMan[destination.man()])
		return false;

	if (requiredCaptures > (destination.captured() ? availableCapturesByShrine : availableCapturesBySquare)[destination.square()])
		return false;

	return true;
}

/* -------------------------------------------------------------------------- */

}
