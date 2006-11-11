#include "targets.h"
#include "position.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Target::Target(Glyph glyph, Square square)
	: _man(UndefinedMan), _glyph(glyph), _color(glyph.color()), _square(square), _cause(NULL, 0)
{
	assert(glyph.isValid());
	assert(square.isValid());

	_squares.set(square);
	_men.set();

	requiredMoves = 0;
	requiredCaptures = 0;

	/* -- List possible destinations -- */

	for (Man man = FirstMan; man <= LastMan; man++)
		if (man.glyph(_color) == glyph)
			push_back(Destination(square, _color, man, man, false));

	/* -- Handle promoted pieces -- */

	for (Superman superman = FirstPromotedMan; superman <= LastPromotedMan; superman++)
		if (superman.glyph(_color) == glyph)
			for (Man man = FirstPawn; man <= LastPawn; man++)
				push_back(Destination(square, _color, man, superman, false));
}

/* -------------------------------------------------------------------------- */

Target::Target(Color color, const Squares& squares)
	: _man(UndefinedMan), _glyph(NoGlyph), _color(color), _square(UndefinedSquare), _cause(NULL, 0)
{
	_squares = squares;
	_men.set();

	requiredMoves = 0;
	requiredCaptures = 0;

	/* -- List possible destinations -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
	{
		if (!squares[square])
			continue;

		/* -- Each man may have been captured on this square -- */

		for (Man man = FirstMan; man <= LastMan; man++)
			push_back(Destination(square, color, man, man, true));

		/* -- Pawns may also have been captured as promoted pieces -- */

		for (Man man = FirstPawn; man <= LastPawn; man++)
			for (Superman superman = FirstPromotedMan; superman <= LastPromotedMan; superman++)
				push_back(Destination(square, color, man, superman, true));
	}
}

/* -------------------------------------------------------------------------- */

int Target::computeRequiredMoves(const Board& board)
{
	std::for_each(begin(), end(), boost::bind(&Destination::computeRequiredMoves, _1, cref(board)));
	return updateRequiredMoves();
}

/* -------------------------------------------------------------------------- */

int Target::computeRequiredCaptures(const Board& board)
{
	std::for_each(begin(), end(), boost::bind(&Destination::computeRequiredCaptures, _1, cref(board)));
	return updateRequiredCaptures();
}

/* -------------------------------------------------------------------------- */

int Target::updateRequiredMoves()
{
	return requiredMoves = std::min_element(begin(), end(), Destination::lessMoves)->getRequiredMoves();
}

/* -------------------------------------------------------------------------- */

int Target::updateRequiredCaptures()
{
	return requiredCaptures = std::min_element(begin(), end(), Destination::lessCaptures)->getRequiredCaptures();
}

/* -------------------------------------------------------------------------- */

const Men& Target::updatePossibleMen()
{
	if (_man != UndefinedMan)
		return _men;

	_men.reset();
	for (vector<Destination>::const_iterator destination = begin(); destination != end(); destination++)
		_men.set(destination->man());

	if (_men.count() == 1)
		for (Man man = FirstMan; man <= LastMan; man++)
			if (_men[man])
				_man = man;

	return _men;
}

/* -------------------------------------------------------------------------- */

const Squares& Target::updatePossibleSquares()
{
	if (_square != UndefinedSquare)
		return _squares;

	_squares.reset();
	for (vector<Destination>::const_iterator destination = begin(); destination != end(); destination++)
		_squares.set(destination->square());

	if (_squares.count() == 1)
		for (Square square = FirstSquare; square <= LastSquare; square++)
			if (_squares[square])
				_square = square;

	return _squares;
}

/* -------------------------------------------------------------------------- */

bool Target::setPossibleMen(const Men& men)
{
	Men xmen = men;
	xmen.flip() &= _men;

	if (xmen.none())
		return false;

	_men &= men;

	iterator last = std::remove_if(begin(), end(), boost::bind(&Destination::isInMen, _1, cref(xmen)))	;
	if (last == end())
		return false;

	erase(last, end());
	if (empty())
		abort(NoSolution);

	updatePossibleSquares();
	updateRequiredMoves();
	updateRequiredCaptures();

	return true;
}

/* -------------------------------------------------------------------------- */

bool Target::setPossibleSquares(const Squares& squares)
{
	Squares xsquares = squares;
	xsquares.flip() &= _squares;

	if (xsquares.none())
		return false;

	_squares &= squares;

	iterator last = std::remove_if(begin(), end(), boost::bind(&Destination::isInSquares, _1, cref(xsquares)));
	if (last == end())
		return false;

	erase(last, end());
	if (empty())
		abort(NoSolution);

	updatePossibleMen();
	updateRequiredMoves();
	updateRequiredCaptures();

	return true;
}

/* -------------------------------------------------------------------------- */

bool Target::setAvailableMoves(int numAvailableMoves)
{
	iterator last = std::remove_if(begin(), end(), !boost::bind(&Destination::isInMoves, _1, numAvailableMoves));
	if (last == end())
		return false;

	erase(last, end());
	if (empty())
		abort(NoSolution);

	updatePossibleMen();
	updatePossibleSquares();
	updateRequiredCaptures();

	return true;
}

/* -------------------------------------------------------------------------- */

bool Target::setAvailableCaptures(int numAvailableCaptures)
{
	iterator last = std::remove_if(begin(), end(), !boost::bind(&Destination::isInCaptures, _1, numAvailableCaptures));
	if (last == end())
		return false;

	erase(last, end());
	if (empty())
		abort(NoSolution);

	updatePossibleMen();
	updatePossibleSquares();
	updateRequiredMoves();

	return true;
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

}
