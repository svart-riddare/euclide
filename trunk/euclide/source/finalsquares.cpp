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

int FinalSquare::computeRequiredMoves(const Board& board, Man man, Color color, const Castling& castling)
{
	int distance = board.distance(man, this->man, color, square, castling);
	if (distance > requiredMoves)
		requiredMoves = distance;

	return getRequiredMoves();
}

/* -------------------------------------------------------------------------- */

int FinalSquare::getRequiredMoves() const
{
	return requiredMoves;
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

bool FinalSquares::operator=(Square square)
{	
	assert(isValidSquare(square));
	
	bool modified = false;
	int empty = 0;

	vector<FinalSquare>::iterator I = squares.begin();
	while(I != squares.end())
	{
		if (*I == square)
		{
			empty += I->empty();
			I++;
		}
		else
		{
			modified = true;
			I = squares.erase(I);
		}
	}

	if (empty == 0)
		captured = false;

	if (empty == (int)squares.size())
		captured = true;

	return modified;
}

/* -------------------------------------------------------------------------- */

int FinalSquares::computeRequiredMoves(const Board& board, Man man, Color color, const Castling& castling)
{
	int minimum = infinity;

	vector<FinalSquare>::iterator I = squares.begin(); 
	while (I != squares.end())
	{
		int requiredMoves = I->computeRequiredMoves(board, man, color, castling);
		if (requiredMoves < minimum)
			minimum = requiredMoves;

		if (requiredMoves >= infinity)
			I = squares.erase(I);
		else
			I++;
	}

	requiredMoves = minimum;

	if (requiredMoves >= infinity)
		captured = true;
	
	return getRequiredMoves();
}

/* -------------------------------------------------------------------------- */

int FinalSquares::getRequiredMoves() const
{
	if (!captured)
		return requiredMoves;

	return 0;
}

/* -------------------------------------------------------------------------- */

FinalSquares::operator const vector<FinalSquare>&() const
{
	return squares;
}

/* -------------------------------------------------------------------------- */

}
