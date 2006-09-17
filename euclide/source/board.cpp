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

	/* -- Initiliaze list of locked initial squares -- */

	for (Man man = FirstMan; man <= LastMan; man++)
		for (Color color = FirstColor; color <= LastColor; color++)
			locks[man][color] = false;
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

	/* -- Loop until finding the minimum distance -- */

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

int Board::distance(Superman superman, Color color, Square from, Square to) const
{
	assert(superman.isValid());
	assert(color.isValid());

	return distance(tables::supermanToGlyph[superman][color], from, to);
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

int Board::captures(const int movements[NumSquares][NumSquares], const bool captures[NumSquares][NumSquares], Square from, Square to)
{
	assert(from.isValid());
	assert(to.isValid());

	/* -- Handle case where no movement is required -- */

	if (from == to)
		return 0;

	/* -- Initialize distances (number of captures) -- */

	array<int, NumSquares> distances;
	distances.assign(-1);
	distances[from] = 0;

	/* -- Initialize ordered square queue -- */

	_greater<int, NumSquares> priority(distances);
	priority_queue<Square, vector<Square>, _greater<int, NumSquares> > squares(priority);
	squares.push(from);

	/* -- Loop until finding the least number of captures -- */

	while (!squares.empty())
	{
		/* -- Remove first queue square -- */

		Square from = squares.top(); squares.pop();
		int distance = distances[from];

		/* -- Handle every possible immediate destination -- */

		for (Square square = FirstSquare; square <= LastSquare; square++)
		{
			/* -- Check whether this movement is forbiden -- */

			if (movements[from][square])
				continue;

			/* -- Check if the square has been attained by a quicker path -- */

			if (distances[square] >= 0)
				continue;
			
			/* -- Add square to queue -- */

			distances[square] = distance + (captures[from][square] ? 1 : 0);
			squares.push(square);
			
			/* -- Have we reached our destination? -- */

			if (square == to)
				return distances[square];
		}
	}

	return infinity;
}

/* -------------------------------------------------------------------------- */

int Board::captures(Glyph glyph, Square from, Square to) const
{
	assert(glyph.isValid());

	/* -- Some pieces do not need to capture in order to move -- */

	if (!tables::maxCaptures[glyph])
		return 0;

	return captures(movements[glyph], tables::validCaptures[glyph], from, to);
}

/* -------------------------------------------------------------------------- */

int Board::captures(Superman superman, Color color, Square from, Square to) const
{
	assert(superman.isValid());
	assert(color.isValid());

	return captures(tables::supermanToGlyph[superman][color], from, to);
}

/* -------------------------------------------------------------------------- */

int Board::captures(Man man, Superman superman, Color color, Square from, Square to) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());

	/* -- No promotion case -- */

	if (man == superman)
		return captures(man, color, from, to);

	/* -- Handle promoted men -- */

	Square square = tables::initialSquares[superman][color];
	return captures(man, color, from, square) + captures(superman, color, square, to);
}

/* -------------------------------------------------------------------------- */

int Board::captures(Man man, Superman superman, Color color, Square to) const
{
	if (empty)
		return icaptures(man, superman, color, to);

	return captures(man, superman, color, tables::initialSquares[man][color], to);
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

int Board::captures(Glyph glyph, Square from, Square to, vector<Squares>& captures) const
{
	assert(glyph.isValid());
	assert(from.isValid());
	assert(to.isValid());

	/* -- Some pieces does not need to capture in order to move -- */

	if (!tables::maxCaptures[glyph])
		return 0;

	/* -- Initialize distances (number of captures) -- */

	array<int, NumSquares> distances;
	distances.assign(infinity);
	distances[from] = 0;

	/* -- Initialize ordered square queue -- */

	_greater<int, NumSquares> priority(distances);
	priority_queue<Square, vector<Square>, _greater<int, NumSquares> > squares(priority);
	squares.push(from);

	/* -- Initialize reverse move graph -- */

	array<list<Square>, NumSquares> moves;

	/* -- Loop until all moves have been tabulated -- */

	while (!squares.empty())
	{
		/* -- Remove first queue square -- */

		Square from = squares.top(); squares.pop();

		/* -- Handle every possible immediate destination -- */

		for (Square square = FirstSquare; square <= LastSquare; square++)
		{
			/* -- Check whether this movement is forbiden -- */

			if (movements[glyph][from][square])
				continue;

			/* -- Compute distance -- */

			int distance = distances[from] + (tables::validCaptures[glyph][from][square] ? 1 : 0);

			if (distance > distances[square])
				continue;

			/* -- Update distance, build move graph and insert square once -- */

			bool insert = (distances[square] == infinity);

			moves[square].push_back(from);
			distances[square] = distance;

			if (insert)
				squares.push(square);
		}
	}

	/* -- Initialize output capture list -- */

	if (distances[to] > 0)
	{
		captures.resize(distances[to]);

		/* -- Start from destination square to build all possible minimal paths -- */

		array<bool, NumSquares> visited;
		visited.assign(false);

		visited[from] = true;
		visited[to] = true;

		queue<Square> squares;
		squares.push(to);

		while (!squares.empty())
		{
			Square from = squares.front(); squares.pop();

			for (list<Square>::const_iterator I = moves[from].begin(); I != moves[from].end(); I++)
			{
				if (!visited[*I])
					squares.push(*I);

				visited[*I] = true;
			}
		}

		/* -- We are now ready to collect the captures -- */

		for (Square square = FirstSquare; square <= LastSquare; square++)
		{
			for (list<Square>::const_iterator I = moves[square].begin(); I != moves[square].end(); I++)
			{
				if (!tables::validCaptures[glyph][*I][square])
					continue;

				if (!visited[*I] || !visited[square])
					continue;

				assert(distances[*I] < distances[to]);
				captures[distances[*I]].set(square);
			}
		}
	}
	
	return distances[to];
}

/* -------------------------------------------------------------------------- */

int Board::captures(Man man, Superman superman, Color color, Square from, Square to, vector<Squares>& captures) const
{
	assert(man.isValid());
	assert(superman.isValid());
	assert(color.isValid());

	/* -- No promotion case -- */

	if (man == superman)
		return this->captures(tables::supermanToGlyph[superman][color], from, to, captures);

	/* -- Handle promoted men -- */

	Square square = tables::initialSquares[superman][color];
	return this->captures(tables::supermanToGlyph[man][color], from, square) + this->captures(tables::supermanToGlyph[superman][color], square, to);
}

/* -------------------------------------------------------------------------- */

void Board::block(Glyph glyph, Square square, bool captured)
{
	assert(glyph.isValid());
	assert(square.isValid());

	empty = false;

	for (int **obstructions = this->obstructions[captured ? glyph : Glyph(NoGlyph)][square]; *obstructions; obstructions++)
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

	for (int **obstructions = this->obstructions[square][captured ? glyph : Glyph(NoGlyph)]; *obstructions; obstructions++)
		**obstructions -= 1;
}

/* -------------------------------------------------------------------------- */

bool Board::lock(Man man, Color color)
{
	assert(man.isValid());
	assert(color.isValid());

	Square initial = tables::initialSquares[man][color];
	Glyph glyph = tables::supermanToGlyph[man][color];

	/* -- Check if it was already locked -- */

	if (locks[man][color])
		return false;

	/* -- Check if we can reach the initial square -- */

	for (Square square = FirstSquare; square < LastSquare; square++)
		if (movements[glyph][square][initial] == 0)
			return false;

	/* -- Lock the square -- */

	block(glyph, initial);
	locks[man][color] = true;

	return true;
}

/* -------------------------------------------------------------------------- */

}
