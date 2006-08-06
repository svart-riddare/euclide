#include "board.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Board::Board()
{
	empty = true;

	/* -- Initialize table of allowed movements -- */

	for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
		for (Square from = FirstSquare; from <= LastSquare; from++)
			for (Square to = FirstSquare; to <= LastSquare; to++)
				movements[glyph][from][to] = (tables::validMovements[glyph][from][to] || tables::validCaptures[glyph][from][to]) ? 0 : infinity;

	/* -- Allocate memory for obstruction tables -- */

	obstructions[NoGlyph][0] = new int *[tables::numObstructions];
	obstructions[WhiteKing][0] = new int *[tables::numWhiteObstructions];
	obstructions[BlackKing][0] = new int *[tables::numBlackObstructions];

	/* -- Fill obstruction tables, hacker's style -- */

	int n = 0;
	for (Square square = FirstSquare; square <= LastSquare; square++, n++)
	{
		obstructions[NoGlyph][square] = &obstructions[NoGlyph][0][n];

		for ( ; tables::obstructions[n]; n++)
			obstructions[NoGlyph][0][n] = (int *)movements + (tables::obstructions[n] - &tables::validMovements[0][0][0]);

		obstructions[NoGlyph][0][n] = NULL;
	}

	n = 0;
	for (Square square = FirstSquare; square <= LastSquare; square++, n++)
	{
		obstructions[WhiteKing][square] = &obstructions[WhiteKing][0][n];

		for ( ; tables::whiteObstructions[n]; n++)
			obstructions[WhiteKing][0][n] = (int *)movements + (tables::whiteObstructions[n] - &tables::validMovements[0][0][0]);

		obstructions[WhiteKing][0][n] = NULL;
	}

	n = 0;
	for (Square square = FirstSquare; square <= LastSquare; square++, n++)
	{
		obstructions[BlackKing][square] = &obstructions[BlackKing][0][n];

		for ( ; tables::blackObstructions[n]; n++)
			obstructions[BlackKing][0][n] = (int *)movements + (tables::blackObstructions[n] - &tables::validMovements[0][0][0]);

		obstructions[BlackKing][0][n] = NULL;
	}

	for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
	{
		if (glyph.isWhite())
			std::copy(obstructions[WhiteKing], obstructions[WhiteKing] + NumSquares, obstructions[glyph]);

		if (glyph.isBlack())
			std::copy(obstructions[BlackKing], obstructions[BlackKing] + NumSquares, obstructions[glyph]);
	}
}

/* -------------------------------------------------------------------------- */

Board::~Board()
{
	delete[] obstructions[NoGlyph][0];
	delete[] obstructions[WhiteKing][0];
	delete[] obstructions[BlackKing][0];
}

/* -------------------------------------------------------------------------- */

int Board::distance(const int movements[NumSquares][NumSquares], Square from, Square to)
{
	assert(from.isValid());
	assert(to.isValid());

	/* -- Handle case where no movement is required -- */

	if (from == to)
		return 0;

	/* -- Initialize square queue -- */

	queue<Square> squares;
	squares.push(from);

	/* -- Initialize distances -- */

	array<int, NumSquares> distances;
	distances.assign(-1);
	distances[from] = 0;

	/* -- Loop while there is still an intermediate square -- */

	while (!squares.empty())
	{
		/* -- Remove first queue square -- */

		Square from = squares.front(); squares.pop();
		int distance = distances[from] + 1;

		/* -- Handle every possible immediate destination -- */

		for (Square square = FirstSquare; square <= LastSquare; square++)
		{
			/* -- Check whether this movement is forbiden -- */

			if (movements[from][square])
				continue;

			/* -- Check if the square has been attained by a quicker path -- */

			if (distances[square] >= 0)
				continue;

			/* -- Have we reached our destination? -- */

			if (square == to)
				return distance;

			/* -- If not, add this square to the queue -- */

			distances[square] = distance;
			squares.push(square);
		}
	}

	return infinity;
}

/* -------------------------------------------------------------------------- */

int Board::distance(Glyph glyph, Square from, Square to) const
{
	assert(glyph.isValid());
	return distance(movements[glyph], from, to);
}

/* -------------------------------------------------------------------------- */

int Board::distance(Superman superman, Color color, Square square, Square to) const
{
	assert(superman.isValid());
	assert(color.isValid());

	return distance(tables::supermanToGlyph[superman][color], square, to);
}

/* -------------------------------------------------------------------------- */

int Board::distance(Man man, Superman superman, Color color, Square from, Square to) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());

	/* -- No promotion case -- */

	if (man == superman)
		return distance(man, color, from, to);

	/* -- Handle promoted men -- */

	Square square = tables::initialSquares[superman][color];
	return distance(man, color, from, square) + distance(superman, color, square, to);
}

/* -------------------------------------------------------------------------- */

int Board::distance(Man man, Superman superman, Color color, Square to, const Castling& castling) const
{
	int minimum = infinity;

	if (castling.isNonePossible(man))
		if (!empty)
			minimum = distance(man, superman, color, tables::initialSquares[man][color], to);
		else
			minimum = idistance(man, superman, color, to);

	if (castling.isKingsidePossible(man))
		minimum = std::min(minimum, distance(man, superman, color, castling.kingsideSquare(man, color), to) + ((man == King) ? 1 : 0));

	if (castling.isQueensidePossible(man))
		minimum = std::min(minimum, distance(man, superman, color, castling.queensideSquare(man, color), to) + ((man == King) ? 1 : 0));

	return minimum;
}

/* -------------------------------------------------------------------------- */

int Board::idistance(Man man, Superman superman, Color color, Square to) const
{
	assert(to.isValid());
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());

	/* -- Return distance in original position on empty board -- */

	if (man == superman)
		return tables::initialDistances[man][to][color];

	/* -- Handle promoted men -- */

	Square square = tables::initialSquares[superman][color];
	return tables::initialDistances[man][square][color] + tables::initialDistances[superman][to][color];
}

/* -------------------------------------------------------------------------- */

int Board::icaptures(Man man, Superman superman, Color color, Square to) const
{
	assert(to.isValid());
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());

	/* -- Return number of required captures in original position on empty board -- */

	if (man == superman)
		return tables::initialCaptures[man][to][color];

	/* -- Handle promoted men -- */

	Square square = tables::initialSquares[superman][color];
	return tables::initialCaptures[man][square][color] + tables::initialCaptures[superman][to][color];
}

/* -------------------------------------------------------------------------- */

void Board::block(Glyph glyph, Square square, bool captured)
{
	assert(glyph.isValid());
	assert(square.isValid());

	empty = false;

	for (int **obstructions = this->obstructions[captured ? glyph : NoGlyph][square]; *obstructions; obstructions++)
		**obstructions += 1;
}

/* -------------------------------------------------------------------------- */

void Board::block(Glyph glyph, Square from, Square to, bool captured)
{
	unblock(glyph, from);
	block(glyph, to, captured);
}

/* -------------------------------------------------------------------------- */

void Board::unblock(Glyph glyph, Square square, bool captured)
{
	assert(glyph.isValid());
	assert(square.isValid());

	for (int **obstructions = this->obstructions[square][captured ? glyph : NoGlyph]; *obstructions; obstructions++)
		**obstructions -= 1;
}

/* -------------------------------------------------------------------------- */

}
