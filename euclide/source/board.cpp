#include "board.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Board::Board()
{
	for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
		for (Square from = FirstSquare; from <= LastSquare; from++)
			for (Square to = FirstSquare; to <= LastSquare; to++)
				blockedMovements[glyph][from][to] = tables::validMovements[glyph][from][to] ? 0 : infinity;
}

/* -------------------------------------------------------------------------- */

int Board::distance(const int blockedMovements[NumSquares][NumSquares], Square from, Square to)
{
	assert(isValidSquare(from));
	assert(isValidSquare(to));

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

			if (blockedMovements[from][square])
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
	assert(isValidGlyph(glyph));
	return distance(blockedMovements[glyph], from, to);
}

/* -------------------------------------------------------------------------- */

int Board::distance(Man man, Color color, Square square, Square to) const
{
	assert(isValidSuperman(man));
	assert(isValidColor(color));

	return distance(tables::manToGlyph[man][color], square, to);
}

/* -------------------------------------------------------------------------- */

int Board::distance(Man man, Man superman, Color color, Square from, Square to) const
{
	assert(isValidMan(man));
	assert(isValidSuperman(man));
	assert(isValidColor(color));

	if (man == superman)
		return distance(man, color, from, to);

	/* -- Handle promoted men -- */

	Square square = tables::initialSquares[superman][color];

	return distance(man, color, from, square) + distance(superman, color, square, to);
}

/* -------------------------------------------------------------------------- */

int Board::distance(Man man, Man superman, Color color, Square to, const Castling& castling) const
{
	int minimum = infinity;

	if (castling.isNonePossible(man))
		minimum = distance(man, superman, color, tables::initialSquares[man][color], to);

	if (castling.isKingsidePossible(man))
		minimum = std::min(minimum, distance(man, superman, color, castling.kingsideSquare(man, color), to) + ((man == King) ? 1 : 0));

	if (castling.isQueensidePossible(man))
		minimum = std::min(minimum, distance(man, superman, color, castling.queensideSquare(man, color), to) + ((man == King) ? 1 : 0));

	return minimum;
}

/* -------------------------------------------------------------------------- */

}
