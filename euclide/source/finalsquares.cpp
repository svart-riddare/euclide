#include "finalsquares.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

FinalSquare::FinalSquare()
{
	square = UndefinedSquare;
	man = UndefinedMan;

	requiredMoves = 0;
	requiredCaptures = 0;
}

/* -------------------------------------------------------------------------- */

FinalSquare::FinalSquare(Square square, Man man, bool captured)
{
	assert(isValidSquare(square));
	assert(isValidSuperman(man));

	this->square = square;
	this->man = man;

	this->captured = captured;

	requiredMoves = 0;
	requiredCaptures = 0;
}

/* -------------------------------------------------------------------------- */

FinalSquare::operator Square() const
{
	return square;
}

/* -------------------------------------------------------------------------- */

FinalSquare::operator Man() const
{
	return man;
}

/* -------------------------------------------------------------------------- */

bool FinalSquare::empty() const
{
	return captured;
}

/* -------------------------------------------------------------------------- */

FinalSquares::FinalSquares()
{
	captured = indeterminate;

	requiredMoves = 0;
	requiredCaptures = 0;
}

/* -------------------------------------------------------------------------- */

FinalSquares& FinalSquares::operator+=(const FinalSquare& finalSquare)
{
	assert((finalSquare.empty() == captured) || indeterminate(captured));

	squares.push_back(finalSquare);
	return *this;
}

/* -------------------------------------------------------------------------- */

FinalSquares& FinalSquares::operator=(Square square)
{
	assert(isValidSquare(square));
	
	int empty = 0;

	for (vector<FinalSquare>::iterator I = squares.begin(); I != squares.end(); )
	{
		if (*I == square)
		{
			empty += I->empty();
			I++;
		}
		else
		{
			I = squares.erase(I);
		}
	}

	if (empty == 0)
		captured = false;

	if (empty == (int)squares.size())
		captured = true;

	return *this;
}

/* -------------------------------------------------------------------------- */

FinalSquares::operator const vector<FinalSquare>&() const
{
	return squares;
}

/* -------------------------------------------------------------------------- */

}
