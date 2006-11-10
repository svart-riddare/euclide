#include "castling.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Castling::Castling()
{
	kingside = indeterminate;
	queenside = indeterminate;
}

/* -------------------------------------------------------------------------- */

bool Castling::isNonePossible(Man man) const
{
	assert(man.isValid());

	if (kingside)
		if ((man == King) || (man == KingRook))
			return false;

	if (queenside)
		if ((man == King) || (man == QueenRook))
			return false;

	return true;
}

/* -------------------------------------------------------------------------- */

bool Castling::isKingsidePossible(Man man) const
{
	assert(man.isValid());

	if ((man == King) || (man == KingRook))
		if (kingside || indeterminate(kingside))
			return true;

	return false;
}

/* -------------------------------------------------------------------------- */

bool Castling::isQueensidePossible(Man man) const
{
	assert(man.isValid());

	if ((man == King) || (man == QueenRook))
		if (queenside || indeterminate(queenside))
			return true;

	return false;
}

/* -------------------------------------------------------------------------- */

Square Castling::kingsideSquare(Man man, Color color) const
{
	assert(isKingsidePossible(man));
	assert(color.isValid());

	if (man == KingRook)
		return (color == White) ? F1 : F8;

	assert(man == King);

	return (color == White) ? G1 : G8;
}

/* -------------------------------------------------------------------------- */

Square Castling::queensideSquare(Man man, Color color) const
{
	assert(isQueensidePossible(man));
	assert(color.isValid());

	if (man == QueenRook)
		return (color == White) ? D1 : D8;

	assert(man == King);

	return (color == White) ? C1 : C8;
}

/* -------------------------------------------------------------------------- */

void Castling::setKingsidePossible(bool possible)
{
	kingside = possible;
}

/* -------------------------------------------------------------------------- */

void Castling::setQueensidePossible(bool possible)
{
	queenside = possible;
}

/* -------------------------------------------------------------------------- */

}
