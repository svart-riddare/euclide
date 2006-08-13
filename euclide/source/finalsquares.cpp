#include "finalsquares.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

FinalSquare::FinalSquare(Square square, Man man, Superman superman, bool captured)
{
	assert(square.isValid());
	assert(man.isValid());
	assert(superman.isValid());

	this->square = square;
	this->man = man;
	this->superman = superman;

	this->captured = captured;

	requiredMoves = 0;
	requiredCaptures = 0;
}

/* -------------------------------------------------------------------------- */

int FinalSquare::computeRequiredMoves(const Board& board, Color color, const Castling& castling)
{
	int distance = board.distance(man, superman, color, square, castling);
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

int FinalSquare::computeRequiredCaptures(const Board& board, Color color)
{
	int captures = board.captures(man, superman, color, square);
	if (captures > requiredCaptures)
		requiredCaptures = captures;

	return getRequiredCaptures();
}

/* -------------------------------------------------------------------------- */

int FinalSquare::getRequiredCaptures() const
{
	return requiredCaptures;
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

FinalSquare::operator Superman() const
{
	return superman;
}

/* -------------------------------------------------------------------------- */

bool FinalSquare::isSquare(Square square, bool captured) const
{
	return (this->square == square) && (this->captured == captured);
}

/* -------------------------------------------------------------------------- */

bool FinalSquare::isSquare(Square square) const
{
	return (this->square == square);
}

/* -------------------------------------------------------------------------- */

bool FinalSquare::isEmpty() const
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
	assert(!requiredMoves);

	squares.push_back(finalSquare);
	return *this;
}

/* -------------------------------------------------------------------------- */

bool FinalSquares::applyDeduction(Square square, bool captured)
{	
	bool modified = false;

	for (finalsquares_t::iterator I = squares.begin(); I != squares.end(); )
	{
		if (I->isSquare(square, captured))
		{
			I++;
		}
		else
		{
			modified = true;
			I = squares.erase(I);
		}
	}

	if (modified)
		applyDeduction();

	return modified;
}

/* -------------------------------------------------------------------------- */

bool FinalSquares::applyDeduction(const array<int, NumSquares>& availableMoves, const array<int, NumSquares>& availableCaptures)
{
	bool modified = false;

	for (finalsquares_t::iterator I = squares.begin(); I != squares.end(); )
	{
		if ((I->getRequiredMoves() <= availableMoves[(Square)*I]) && (I->getRequiredCaptures() <= availableCaptures[(Square)*I]))
		{
			I++;
		}
		else
		{
			modified = true;
			I = squares.erase(I);
		}
	}

	if (modified)
		applyDeduction();

	return modified;
}

/* -------------------------------------------------------------------------- */

bool FinalSquares::applyDeduction(int availableMoves, int availableCaptures)
{
	bool modified = false;

	for (finalsquares_t::iterator I = squares.begin(); I != squares.end(); )
	{
		if ((I->getRequiredMoves() <= availableMoves) && (I->getRequiredCaptures() <= availableCaptures))
		{
			I++;
		}
		else
		{
			modified = true;
			I = squares.erase(I);
		}
	}

	if (modified)
		applyDeduction();

	return modified;
}

/* -------------------------------------------------------------------------- */

bool FinalSquares::applyDeduction()
{
	/* -- If there is no possible squares, the problem has no solutions -- */

	if (squares.empty())
		abort(NoSolution);

	/* -- Check capture state -- */

	if (!indeterminate(captured))
		return false;

	/* -- Determine capture state if possible -- */

	int numEmptySquares = (int)std::count_if(squares.begin(), squares.end(), std::mem_fun_ref<bool, FinalSquare>(&FinalSquare::isEmpty));

	if (numEmptySquares == (int)squares.size())
		captured = true;

	if (numEmptySquares == 0)
		captured = false;

	return true;
}

/* -------------------------------------------------------------------------- */

int FinalSquares::computeRequiredMoves(const Board& board, Color color, const Castling& castling)
{
	int minimum = infinity;

	/* -- Compute required moves for each possible final square -- */

	for (finalsquares_t::iterator I = squares.begin(); I != squares.end(); I++)
	{
		/* -- Keep minimum value -- */

		int requiredMoves = I->computeRequiredMoves(board, color, castling);
		if (requiredMoves < minimum)
			minimum = requiredMoves;
	}

	/* -- Update required moves for current man -- */

	if (minimum > requiredMoves)
		requiredMoves = minimum;

	return getRequiredMoves();
}

/* -------------------------------------------------------------------------- */

int FinalSquares::getRequiredMoves(array<int, NumSquares>& squares) const
{
	/* -- Find minimum number of moves to reach occupied squares -- */

	for (finalsquares_t::const_iterator I = this->squares.begin(); I != this->squares.end(); I++)
	{
		int requiredMoves = I->getRequiredMoves();
		Square square = *I;

		if (requiredMoves < squares[square])
			if (!I->isEmpty())
				squares[square] = requiredMoves;
	}

	return getRequiredMoves();
}

/* -------------------------------------------------------------------------- */

int FinalSquares::getRequiredMoves() const
{
	return requiredMoves;
}

/* -------------------------------------------------------------------------- */

int FinalSquares::computeRequiredCaptures(const Board& board, Color color)
{
	int minimum = infinity;

	/* -- Compute required captures for each possible final square -- */

	for (finalsquares_t::iterator I = squares.begin(); I != squares.end(); I++)
	{
		/* -- Keep minimum value -- */

		int requiredCaptures = I->computeRequiredCaptures(board, color);
		if (requiredCaptures < minimum)
			minimum = requiredCaptures;
	}

	/* -- Update required moves for current man -- */

	if (minimum > requiredCaptures)
		requiredCaptures = minimum;

	return getRequiredCaptures();
}

/* -------------------------------------------------------------------------- */

int FinalSquares::getRequiredCaptures(array<int, NumSquares>& squares) const
{
	/* -- Find minimum number of captures to reach occupied squares -- */

	for (finalsquares_t::const_iterator I = this->squares.begin(); I != this->squares.end(); I++)
	{
		int requiredCaptures = I->getRequiredCaptures();
		Square square = *I;

		if (requiredCaptures < squares[square])
			if (!I->isEmpty())
				squares[square] = requiredCaptures;
	}

	return getRequiredCaptures();
}

/* -------------------------------------------------------------------------- */

int FinalSquares::getRequiredCaptures() const
{
	return requiredCaptures;
}

/* -------------------------------------------------------------------------- */

FinalSquares::operator const finalsquares_t&() const
{
	return squares;
}

/* -------------------------------------------------------------------------- */

}
