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
	assert(isValidMan(man));

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
	assert(isValidMan(man));

	if ((man == King) || (man == KingRook))
		if (kingside != false)
			return true;

	return false;
}

/* -------------------------------------------------------------------------- */

bool Castling::isQueensidePossible(Man man) const
{
	assert(isValidMan(man));

	if ((man == King) || (man == QueenRook))
		if (queenside != false)
			return true;

	return false;
}

/* -------------------------------------------------------------------------- */

Square Castling::kingsideSquare(Man man, Color color) const
{
	assert(isKingsidePossible(man));
	assert(isValidColor(color));

	if (man == KingRook)
		return (color == White) ? F1 : F8;

	assert(man == King);

	return (color == White) ? G1 : G8;
}

/* -------------------------------------------------------------------------- */

Square Castling::queensideSquare(Man man, Color color) const
{
	assert(isQueensidePossible(man));
	assert(isValidColor(color));

	if (man == QueenRook)
		return (color == White) ? D1 : D8;

	assert(man == King);

	return (color == White) ? C1 : C8;
}

/* -------------------------------------------------------------------------- */

}
