#include "targets.h"
#include "position.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Target::Target(Glyph glyph, Square square)
	: _man(UndefinedMan), _glyph(glyph), _color(glyph.color()), _square(square), _superman(UndefinedSuperman)
{
	assert(glyph.isValid());
	assert(square.isValid());

	_men.set();
	_supermen.set();
	_squares.set(square);

	_minRequiredMoves = 0;
	_minRequiredCaptures = 0;

	_requiredMoves.assign(0);
	_requiredCaptures.assign(0);

	/* -- Reserve some memory -- */

	reserve(glyph.isPawn() ? NumPawns : (NumMen - NumPawns + NumColumns * NumPawns));

	/* -- List possible destinations -- */

	for (Man man = FirstMan; man <= LastMan; man++)
		if (man.glyph(_color) == glyph)
			push_back(Destination(square, _color, man, man, false));

	/* -- Handle promoted pieces -- */

	for (Superman superman = FirstPromotedMan; superman <= LastPromotedMan; superman++)
		if (superman.glyph(_color) == glyph)
			for (Man man = FirstPawn; man <= LastPawn; man++)
				if (superman.man() == man)
					push_back(Destination(square, _color, man, superman, false));

	/* -- Update list of possible men -- */

	updatePossibleMen();
}

/* -------------------------------------------------------------------------- */

Target::Target(Color color, const Squares& squares)
	: _man(UndefinedMan), _glyph(NoGlyph), _color(color), _square(UndefinedSquare), _superman(UndefinedSuperman)
{
	_men.set();
	_supermen.set();
	_squares = squares;

	_minRequiredMoves = 0;
	_minRequiredCaptures = 0;

	_requiredMoves.assign(0);
	_requiredCaptures.assign(0);

	/* -- Reserve memory -- */

	reserve(squares.count() * (NumMen + NumPawns * NumPromotedMen));

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
				if (superman.man() == man)
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
	_minRequiredMoves = infinity;
	_requiredMoves.assign(infinity);

	for (const_iterator destination = begin(); destination != end(); destination++)
	{
		minimize(_minRequiredMoves, destination->requiredMoves());
		minimize(_requiredMoves[destination->man()], destination->requiredMoves());
	}

	return _minRequiredMoves;
}

/* -------------------------------------------------------------------------- */

int Target::updateRequiredCaptures()
{
	_minRequiredCaptures = infinity;
	_requiredCaptures.assign(infinity);

	for (const_iterator destination = begin(); destination != end(); destination++)
	{
		minimize(_minRequiredCaptures, destination->requiredCaptures());
		minimize(_requiredCaptures[destination->man()], destination->requiredCaptures());
	}

	return _minRequiredCaptures;
}

/* -------------------------------------------------------------------------- */

const Men& Target::updatePossibleMen()
{
	updatePossibleSupermen();

	if (_man != UndefinedMan)
		return _men;

	_men.reset();
	for (Destinations::const_iterator destination = begin(); destination != end(); destination++)
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
	for (Destinations::const_iterator destination = begin(); destination != end(); destination++)
		_squares.set(destination->square());

	if (_squares.count() == 1)
		for (Square square = FirstSquare; square <= LastSquare; square++)
			if (_squares[square])
				_square = square;

	return _squares;
}

/* -------------------------------------------------------------------------- */

const Supermen& Target::updatePossibleSupermen()
{
	if (_superman != UndefinedSuperman)
		return _supermen;

	_supermen.reset();
	for (Destinations::const_iterator destination = begin(); destination != end(); destination++)
		_supermen.set(destination->superman());

	if (_supermen.count() == 1)
		for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
			if (_supermen[superman])
				_superman = superman;

	return _supermen;
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

	updatePossibleMen();
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
	updatePossibleSquares();
	updateRequiredMoves();
	updateRequiredCaptures();

	return true;
}

/* -------------------------------------------------------------------------- */

bool Target::setAvailableMoves(int availableMoves)
{
	iterator last = std::remove_if(begin(), end(), !boost::bind(&Destination::isEnoughMoves, _1, availableMoves));
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

bool Target::setAvailableCaptures(int availableCaptures)
{
	iterator last = std::remove_if(begin(), end(), !boost::bind(&Destination::isEnoughCaptures, _1, availableCaptures));
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

bool Target::setAvailableMoves(const array<int, NumMen>& availableMoves)
{
	iterator last = std::remove_if(begin(), end(), !boost::bind(&Destination::isEnoughManMoves, _1, cref(availableMoves)));
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

bool Target::setAvailableCaptures(const array<int, NumMen>& availableCaptures)
{
	iterator last = std::remove_if(begin(), end(), !boost::bind(&Destination::isEnoughManCaptures, _1, cref(availableCaptures)));
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

bool Target::operator==(const Target& target) const
{
	if (_man != target.man())
		return false;

	if (_glyph != target.glyph())
		return false;

	if (_color != target.color())
		return false;

	if (_square != target.square())
		return false;

	if (_superman != target.superman())
		return false;

	if  (_men != target.men())
		return false;

	if (_squares != target.squares())
		return false;

	if (_supermen != target.supermen())
		return false;

	return true;
}

/* -------------------------------------------------------------------------- */

}
