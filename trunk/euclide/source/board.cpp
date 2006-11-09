#include "board.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Obstructions::Obstructions(Superman superman, Color color, Square square, int movements[NumSquares][NumSquares])
{
	assert(superman.isValid());
	assert(color.isValid());
	assert(square.isValid());

	Glyph glyph = superman.glyph(color);

	/* -- Allocate obstruction table from precomputed table -- */

	const tables::Obstruction *_obstructions = tables::obstructions[glyph][square].obstructions;
	int numObstructions = tables::obstructions[glyph][square].numObstructions;
	
	obstructions = new int *[numObstructions];

	/* -- Soft obstructions are used for pieces captured on the obstruction square -- */

	numSoftObstructions = 0;
	for (int k = 0; k < numObstructions; k++)
		if (_obstructions[k].to != square)
			obstructions[numSoftObstructions++] = &movements[_obstructions[k].from][_obstructions[k].to];

	/* -- Hard obstructions are suitable only if the obstructing piece is not captured -- */

	numHardObstructions = numSoftObstructions;
	for (int k = 0; k < numObstructions; k++)
		if (_obstructions[k].to == square)
			obstructions[numHardObstructions++] = &movements[_obstructions[k].from][_obstructions[k].to];
}

/* -------------------------------------------------------------------------- */

Obstructions::~Obstructions()
{
	delete[] obstructions;
}

/* -------------------------------------------------------------------------- */

void Obstructions::block(bool soft) const
{
	int numObstructions = soft ? numSoftObstructions : numHardObstructions;

	for (int k = 0; k < numObstructions; k++)
		*(obstructions[k]) += 1;
}

/* -------------------------------------------------------------------------- */

void Obstructions::unblock(bool soft) const
{
	int numObstructions = soft ? numSoftObstructions : numHardObstructions;

	for (int k = 0; k < numObstructions; k++)
		*(obstructions[k]) -= 1;
}

/* -------------------------------------------------------------------------- */

void Obstructions::optimize()
{
	int k, n;

	/* -- Remove from the obstruction table all useless entries -- */

	for (k = 0, n = 0; k < numSoftObstructions; n += *obstructions[k++] ? 0 : 1)
		obstructions[n] = obstructions[k];

	numSoftObstructions = n;

	for ( ; k < numHardObstructions; n += *obstructions[k++] ? 0 : 1)
		obstructions[n] = obstructions[k];
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

Movements::Movements(Superman superman, Color color)
	: superman(superman), color(color), glyph(superman.glyph(color))
{
	assert(superman.isValid());
	assert(color.isValid());	

	/* -- Initialize table of allowed movements -- */

	for (Square from = FirstSquare; from <= LastSquare; from++)
		for (Square to = FirstSquare; to <= LastSquare; to++)
			movements[from][to] = (tables::movements[glyph][from][to] || tables::captures[glyph][from][to]) ? 0 : infinity;

	/* -- Count number of possible movements -- */

	possibilities = 0;
	for (Square from = FirstSquare; from <= LastSquare; from++)
		for (Square to = FirstSquare; to <= LastSquare; to++)
			if (movements[from][to] == 0)
				possibilities++;

	/* -- Check if some moves require captures -- */

	hybrid = false;
	for (Square from = FirstSquare; from <= LastSquare; from++)
		for (Square to = FirstSquare; to <= LastSquare; to++)
			if (tables::captures[glyph][from][to])
				hybrid = true;

	/* -- Fill in initial distances and required captures -- */

	distances(superman.square(color), initialDistances);
	captures(superman.square(color), initialCaptures);

	/* -- Initialize obstruction tables -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
		obstructions[square] = new Obstructions(superman, color, square, movements);
}

/* -------------------------------------------------------------------------- */

Movements::~Movements()
{
	for (Square square = FirstSquare; square <= LastSquare; square++)
		delete obstructions[square];
}

/* -------------------------------------------------------------------------- */

int Movements::distance(Square square) const
{
	assert(square.isValid());

	return initialDistances[square];
}

/* -------------------------------------------------------------------------- */

int Movements::captures(Square square) const
{
	assert(square.isValid());

	return initialCaptures[square];
}

/* -------------------------------------------------------------------------- */

int Movements::distance(Square from, Square to) const
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

	array<int, NumSquares> distances(-1);
	distances[from] = 0;

	/* -- Loop until we find the minimum distance -- */

	while (!squares.empty())
	{
		/* -- Remove first queue square -- */

		Square from = squares.front(); squares.pop();
		int distance = distances[from] + 1;

		/* -- Handle every possible immediate destination -- */

		for (Square square = FirstSquare; square <= LastSquare; square++)
		{
			/* -- Check whether this movement is possible -- */

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

int Movements::captures(Square from, Square to) const
{
	assert(from.isValid());
	assert(to.isValid());

	/* -- Do we need to capture at all ? -- */

	if (!hybrid)
		return 0;

	/* -- Handle case where no movement nor capture is required -- */

	if (from == to)
		return 0;

	/* -- Initialize distances (number of captures) -- */

	array<int, NumSquares> distances(-1);
	distances[from] = 0;

	/* -- Initialize ordered square queue -- */

	_greater<int, NumSquares> priority(distances);
	priority_queue<Square, vector<Square>, _greater<int, NumSquares> > squares(priority);
	squares.push(from);

	/* -- Loop until we find the least number of captures -- */

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

			distances[square] = distance + (tables::captures[glyph][from][square] ? 1 : 0);
			squares.push(square);
			
			/* -- Have we reached our destination? -- */

			if (square == to)
				return distances[square];
		}
	}

	return infinity;
}

/* -------------------------------------------------------------------------- */

int *Movements::distances(Square square, int distances[NumSquares]) const
{
	assert(square.isValid());

	/* -- Initialize distances -- */

	std::fill(distances, distances + NumSquares, infinity);
	distances[square] = 0;

	/* -- Initialize square queue -- */

	queue<Square> squares;
	squares.push(square);

	/* -- Loop until every reachable square has been handled -- */

	while (!squares.empty())
	{
		Square from = squares.front(); squares.pop();

		/* -- Handle every possible immediate destination -- */

		for (Square to = FirstSquare; to <= LastSquare; to++)
		{
			if (movements[from][to])
				continue;

			/* -- This square may have been attained by a quicker path -- */

			if (distances[to] < infinity)
				continue;

			/* -- Set square distance -- */

			distances[to] = distances[from] + 1;
	
			/* -- Add it to queue of reachable squares -- */

			squares.push(to);
		}
	}

	/* -- Done -- */

	return distances;
}

/* -------------------------------------------------------------------------- */

int *Movements::captures(Square square, int captures[NumSquares]) const
{
	assert(square.isValid());

	/* -- Initialize captures -- */

	std::fill(captures, captures + NumSquares, hybrid ? -1 : 0);
	captures[square] = 0;

	if (!hybrid)
		return captures;

	/* -- Initialize square priority queue -- */

	_greater<int, NumSquares> priority(captures);
	priority_queue<Square, vector<Square>, _greater<int, NumSquares> > squares(priority);
	squares.push(square);

	/* -- Loop until every reachable square has been handled -- */

	while (!squares.empty())
	{
		Square from = squares.top(); squares.pop();

		/* -- Handle every possible immediate destination -- */

		for (Square to = FirstSquare; square <= LastSquare; square++)
		{
			if (movements[from][to])
				continue;

			/* -- This square may have been attained with less captures -- */

			if (captures[to] >= 0)
				continue;

			/* -- Set square number of required captures -- */

			captures[to] = captures[from] + (tables::captures[glyph][from][to] ? 1 : 0);

			/* -- Add it to queue of reachable destinations -- */

			squares.push(to);
		}
	}

	/* -- Done -- */

	return captures;
}

/* -------------------------------------------------------------------------- */

int Movements::getCaptures(Square from, Square to, vector<Squares>& captures) const
{
	assert(from.isValid());
	assert(to.isValid());

	/* -- Some pieces does not need to capture in order to move -- */

	if (!hybrid)
		return 0;

	/* -- Initialize distances (number of captures) -- */

	array<int, NumSquares> distances(infinity);
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

			if (movements[from][square])
				continue;

			/* -- Compute distance (number of captures) -- */

			int distance = distances[from] + (tables::captures[glyph][from][square] ? 1 : 0);

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

	/* -- Fill capture list -- */

	if (distances[to] > 0)
	{
		captures.resize(distances[to]);

		/* -- Start from destination square to build all possible minimal paths -- */

		array<bool, NumSquares> visited(false);
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
				if (!tables::captures[glyph][*I][square])
					continue;

				if (!visited[*I] || !visited[square])
					continue;

				assert(distances[*I] < distances[to]);
				captures[distances[*I]].set(square);
			}
		}
	}

	/* -- Return number of captures found -- */

	return distances[to];
}

/* -------------------------------------------------------------------------- */

void Movements::block(Square square, bool captured)
{
	assert(square.isValid());

	obstructions[square]->block(captured);
}

/* -------------------------------------------------------------------------- */

void Movements::unblock(Square square, bool captured)
{
	assert(square.isValid());

	obstructions[square]->unblock(captured);
}

/* -------------------------------------------------------------------------- */

void Movements::optimize()
{
	for (Square square = FirstSquare; square <= LastSquare; square++)
		obstructions[square]->optimize();
}

/* -------------------------------------------------------------------------- */

bool Movements::locked() const
{
	if (!possibilities)
		return true;

	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (!movements[square][superman.square(color)])
			return false;

	return true;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

Board::Board()
{
	modified = false;

	/* -- Initialize movement tables -- */

	for (Color color = FirstColor; color <= LastColor; color++)
		for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
			movements[superman][color] = new Movements(superman, color);

	/* -- Initialize lock table -- */

	for (Color color = FirstColor; color <= LastColor; color++)
		for (Man man = FirstMan; man <= LastMan; man++)
			locks[man][color] = false;
}

/* -------------------------------------------------------------------------- */

Board::~Board()
{
	/* -- Release movement tables -- */

	for (Color color = FirstColor; color <= LastColor; color++)
		for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
			delete movements[superman][color];
}

/* -------------------------------------------------------------------------- */

int Board::distance(Man man, Superman superman, Color color, Square from, Square to) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());

	/* -- No promotion case -- */

	if (man == superman)
		return movements[man][color]->distance(from, to);

	/* -- Handle promoted men -- */

	Square square = superman.square(color);
	return movements[man][color]->distance(from, square) + movements[superman][color]->distance(square, to);
}

/* -------------------------------------------------------------------------- */

int Board::distance(Man man, Superman superman, Color color, Square to, const Castling& castling) const
{
	int minimum = infinity;

	/* -- Find minimum distance given castling rights -- */

	if (castling.isNonePossible(man))
		minimum = distance(man, superman, color, to);

	if (castling.isKingsidePossible(man))
		minimum = std::min(minimum, distance(man, superman, color, castling.kingsideSquare(man, color), to) + ((man.isKing()) ? 1 : 0));

	if (castling.isQueensidePossible(man))
		minimum = std::min(minimum, distance(man, superman, color, castling.queensideSquare(man, color), to) + ((man.isKing()) ? 1 : 0));

	return minimum;
}

/* -------------------------------------------------------------------------- */

int Board::distance(Man man, Superman superman, Color color, Square to) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());

	/* -- Use extended algorithms if board movements have been altered -- */

	if (modified)
		return distance(man, superman, color, man.square(color), to);
			
	/* -- No promotion case -- */

	if (man == superman)
		return movements[man][color]->distance(to);

	/* -- Handle promoted men -- */

	return movements[man][color]->distance(superman.square(color)) + movements[superman][color]->distance(to);
}

/* -------------------------------------------------------------------------- */

int Board::captures(Man man, Superman superman, Color color, Square from, Square to) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());

	/* -- No promotion case -- */

	if (man == superman)
		return movements[man][color]->captures(from, to);

	/* -- Handle promoted men -- */

	Square square = superman.square(color);
	return movements[man][color]->captures(from, square) + movements[superman][color]->captures(square, to);
}

/* -------------------------------------------------------------------------- */

int Board::captures(Man man, Superman superman, Color color, Square to) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());

	/* -- Use extended algorithms if board movements have been altered -- */

	if (modified)
		return captures(man, superman, color, man.square(color), to);
			
	/* -- No promotion case -- */

	if (man == superman)
		return movements[man][color]->captures(to);

	/* -- Handle promoted men -- */

	return movements[man][color]->captures(superman.square(color)) + movements[superman][color]->captures(to);
}

/* -------------------------------------------------------------------------- */

int Board::getCaptures(Man man, Superman superman, Color color, Square from, Square to, vector<Squares>& captures) const
{
	assert(man.isValid());
	assert(superman.isValid());
	assert(color.isValid());

	/* -- No promotion case -- */

	if (man == superman)
		return movements[man][color]->getCaptures(from, to, captures);

	/* -- Handle promoted men -- */

	Square square = superman.square(color);
	vector<Squares> _captures;
	
	movements[man][color]->getCaptures(from, square, captures);
	movements[superman][color]->getCaptures(square, to, _captures);

	captures.insert(captures.end(), _captures.begin(), _captures.end());
	return (int)captures.size();
}

/* -------------------------------------------------------------------------- */

void Board::block(Glyph glyph, Square square, bool captured)
{
	assert(glyph.isValid());
	assert(square.isValid());

	modified = true;

	for (Color color = FirstColor; color <= LastColor; color++)
		for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
			movements[superman][color]->block(square, captured && (color != glyph.color()));
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
	
	for (Color color = FirstColor; color <= LastColor; color++)
		for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
			movements[superman][color]->unblock(square, captured && (color != glyph.color()));
}

/* -------------------------------------------------------------------------- */

bool Board::lock(Man man, Color color)
{
	assert(man.isValid());
	assert(color.isValid());

	Square initial = man.square(color);
	Glyph glyph = man.glyph(color);

	/* -- Check if it was already locked -- */

	if (locks[man][color])
		return false;

	/* -- Check if we can reach the initial square -- */

	if (!movements[man][color]->locked())
		return false;

	/* -- Lock the square -- */

	block(glyph, initial);
	locks[man][color] = true;

	return true;
}

/* -------------------------------------------------------------------------- */

}
