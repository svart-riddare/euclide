#include "board.h"
#include "position.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Obstructions::Obstructions(Superman superman, Color color, Square square, Glyph glyph, int movements[NumSquares][NumSquares])
{
	assert(superman.isValid());
	assert(color.isValid());
	assert(square.isValid());

	/* -- The blocking glyph is unimportant if of the same color than the blocked man -- */

	if (glyph.color() == color)
		glyph = NoGlyph;

	/* -- Allocate obstruction table from precomputed table -- */

	const tables::Obstruction *_obstructions = tables::obstructions[superman.glyph(color)][square].obstructions;
	int numObstructions = tables::obstructions[superman.glyph(color)][square].numObstructions;
	
	obstructions = new int *[numObstructions];

	/* -- Soft obstructions are used for pieces captured on the obstruction square -- */

	numSoftObstructions = 0;
	for (int k = 0; k < numObstructions; k++)
		if (_obstructions[k].to != square)
			if (!_obstructions[k].royal || glyph.isKing())
				if (!_obstructions[k].check || tables::checks[_obstructions[k].to][glyph][square])
					obstructions[numSoftObstructions++] = &movements[_obstructions[k].from][_obstructions[k].to];

	/* -- Hard obstructions are suitable only if the obstructing piece is not captured -- */

	numHardObstructions = numSoftObstructions;
	for (int k = 0; k < numObstructions; k++)
		if (_obstructions[k].to == square)
			obstructions[numHardObstructions++] = &movements[_obstructions[k].from][_obstructions[k].to];
}

/* -------------------------------------------------------------------------- */

Obstructions::Obstructions(const Obstructions& obstructions)
{
	numSoftObstructions = obstructions.numSoftObstructions;
	numHardObstructions = obstructions.numHardObstructions;

	this->obstructions = new int *[numHardObstructions];
	std::copy(obstructions.obstructions, obstructions.obstructions + numHardObstructions, this->obstructions);
}

/* -------------------------------------------------------------------------- */

Obstructions::~Obstructions()
{
	delete[] obstructions;
}

/* -------------------------------------------------------------------------- */

Obstructions& Obstructions::operator&=(const Obstructions& obstructions)
{
	int k = 0;
		
	if (this->numSoftObstructions && obstructions.numSoftObstructions)
	{
		for (int m = 0, n = 0; ; )
		{
			if (this->obstructions[m] < obstructions.obstructions[n])
			{
				if (++m >= this->numSoftObstructions)
					break;
			}
			else
			if (this->obstructions[m] > obstructions.obstructions[n])
			{
				if (++n >= obstructions.numSoftObstructions)
					break;
			}
			else
			{
				this->obstructions[k++] = this->obstructions[n++, m++];
			}
		}
	}

	numSoftObstructions = k;
	numHardObstructions = k;
	return *this;
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

	/* -- Initialize initial squares -- */

	initial = superman.square(color);
	ksquare = initial;
	qsquare = initial;

	/* -- Handle castling -- */

	castling = (superman == King) ? 1 : 0;

	if (superman == King)
		ksquare = Square((column_t)(initial.column() + 2), initial.row());

	if (superman == King)
		qsquare = Square((column_t)(initial.column() - 2), initial.row());

	if (superman == KingRook)
		ksquare = Square((column_t)(initial.column() - 2), initial.row());

	if (superman == QueenRook)
		qsquare = Square((column_t)(initial.column() + 3), initial.row());

	_ksquare = ksquare;
	_qsquare = qsquare;

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
			if (tables::captures[glyph][from][to] && !tables::movements[glyph][from][to])
				hybrid = true;

	/* -- Fill in initial distances and required captures -- */

	computeInitialDistances();
	computeInitialCaptures();

	/* -- Initialize obstruction tables -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
	{
		obstructions[square][NoGlyph] = new Obstructions(superman, color, square, NoGlyph, movements);
		validObstructions[NoGlyph] = true;

		for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
		{
			if ((superman.isKing() || glyph.isKing()) && (glyph.color() == !color))
			{
				obstructions[square][glyph] = new Obstructions(superman, color, square, glyph, movements);
				validObstructions[glyph] = true;
			}
			else
			{
				obstructions[square][glyph] = obstructions[square][NoGlyph];
			}
		}

	}
}

/* -------------------------------------------------------------------------- */

Movements::~Movements()
{
	/* -- Delete obstruction tables -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
		for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
			if (validObstructions[glyph])
				delete obstructions[square][glyph];
}

/* -------------------------------------------------------------------------- */

int Movements::distance(Square square) const
{
	assert(square.isValid());

	return distances[square];
}

/* -------------------------------------------------------------------------- */

int Movements::captures(Square square) const
{
	assert(square.isValid());

	return _captures[square];
}

/* -------------------------------------------------------------------------- */

int Movements::distance(const Squares& squares) const
{
	int distance = infinity;

	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (squares[square])
			minimize(distance, distances[square]);

	return distance;
}

/* -------------------------------------------------------------------------- */

int Movements::captures(const Squares& squares) const
{
	int captures = infinity;

	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (squares[square])
			minimize(captures, _captures[square]);

	return captures;
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

			distances[square] = distance + ((tables::captures[glyph][from][square] && !tables::movements[glyph][from][square]) ? 1 : 0);
			squares.push(square);
			
			/* -- Have we reached our destination? -- */

			if (square == to)
				return distances[square];
		}
	}

	return infinity;
}

/* -------------------------------------------------------------------------- */

void Movements::computeInitialDistances()
{
	computeForwardDistances(initial, distances);
}

/* -------------------------------------------------------------------------- */

void Movements::computeInitialCaptures()
{
	computeForwardCaptures(initial, _captures);
}

/* -------------------------------------------------------------------------- */

void Movements::computeForwardDistances(Square square, int distances[NumSquares]) const
{
	/* -- Initialize distances -- */

	std::fill(distances, distances + NumSquares, infinity);
	
	if (square == initial)
	{
		distances[ksquare] = castling;
		distances[qsquare] = castling;
	}
	
	distances[square] = 0;
	
	/* -- Initialize square queue -- */

	queue<Square> squares;
	squares.push(square);

	if (square == initial)
	{
		if (ksquare != initial)
			squares.push(ksquare);
		if (qsquare != initial)
			squares.push(qsquare);
	}

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
}

/* -------------------------------------------------------------------------- */

void Movements::computeForwardCaptures(Square square, int captures[NumSquares]) const
{
	/* -- Initialize captures -- */

	std::fill(captures, captures + NumSquares, hybrid ? -1 : 0);
	captures[square] = 0;

	if (!hybrid)
		return;

	/* -- Initialize square priority queue -- */

	_greater<int, NumSquares> priority(captures);
	priority_queue<Square, vector<Square>, _greater<int, NumSquares> > squares(priority);
	squares.push(square);

	/* -- Loop until every reachable square has been handled -- */

	while (!squares.empty())
	{
		Square from = squares.top(); squares.pop();

		/* -- Handle every possible immediate destination -- */

		for (Square to = FirstSquare; to <= LastSquare; to++)
		{
			if (movements[from][to])
				continue;

			/* -- This square may have been attained with less captures -- */

			if (captures[to] >= 0)
				continue;

			/* -- Set square number of required captures -- */

			captures[to] = captures[from] + ((tables::captures[glyph][from][to] && !tables::movements[glyph][from][to]) ? 1 : 0);

			/* -- Add it to queue of reachable destinations -- */

			squares.push(to);
		}
	}

	/* -- Don't leave negative values in the table -- */

	std::replace(captures, captures + NumSquares, -1, infinity);
}

/* -------------------------------------------------------------------------- */

void Movements::computeReverseDistances(Square square, int distances[NumSquares]) const
{
	/* -- Initialize distances -- */

	std::fill(distances, distances + NumSquares, infinity);
	distances[square] = 0;
	
	/* -- Initialize square queue -- */

	queue<Square> squares;
	squares.push(square);

	/* -- Loop until every reachable square has been handled -- */

	while (!squares.empty())
	{
		Square to = squares.front(); squares.pop();

		/* -- Handle every possible source square -- */

		for (Square from = FirstSquare; from <= LastSquare; from++)
		{
			if (movements[from][to])
				continue;

			/* -- This square may have been attained by a quicker path -- */

			if (distances[from] < infinity)
				continue;

			/* -- Set square distance -- */

			distances[from] = distances[to] + 1;
	
			/* -- Add it to queue of source squares -- */

			squares.push(from);
		}
	}
}

/* -------------------------------------------------------------------------- */

void Movements::computeReverseCaptures(Square square, int captures[NumSquares]) const
{
	/* -- Initialize captures -- */

	std::fill(captures, captures + NumSquares, hybrid ? -1 : 0);
	captures[square] = 0;

	if (!hybrid)
		return;

	/* -- Initialize square priority queue -- */

	_greater<int, NumSquares> priority(captures);
	priority_queue<Square, vector<Square>, _greater<int, NumSquares> > squares(priority);
	squares.push(square);

	/* -- Loop until every reachable square has been handled -- */

	while (!squares.empty())
	{
		Square to = squares.top(); squares.pop();

		/* -- Handle every possible source square -- */

		for (Square from = FirstSquare; from <= LastSquare; from++)
		{
			if (movements[from][to])
				continue;

			/* -- This square may have been attained with less captures -- */

			if (captures[from] >= 0)
				continue;

			/* -- Set square number of required captures -- */

			captures[from] = captures[to] + ((tables::captures[glyph][from][to] && !tables::movements[glyph][from][to]) ? 1 : 0);

			/* -- Add it to queue of source squares -- */

			squares.push(from);
		}
	}

	/* -- Don't leave negative values in the table -- */

	std::replace(captures, captures + NumSquares, -1, infinity);
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

			int distance = distances[from] + ((tables::captures[glyph][from][square] && !tables::movements[glyph][from][square]) ? 1 : 0);

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
		assert((int)captures.size() >= distances[to]);

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

void Movements::block(Squares squares, Glyph glyph)
{
	assert(!squares.none());
	assert(glyph.isValid());

	Square square = FirstSquare;
	while (!squares[square])
		square++;

	if (squares.count() == 1)
		return block(square, glyph, false);

	/* -- Find common obstructions -- */

	Obstructions obstructions(*this->obstructions[square++][glyph]);

	for ( ; square <= LastSquare; square++)
		if (squares[square])
			obstructions &= *this->obstructions[square][glyph];

	/* -- Apply them -- */

	obstructions.block(false);
}

/* -------------------------------------------------------------------------- */

void Movements::block(Square square, Glyph glyph, bool captured)
{
	assert(square.isValid());
	assert(glyph.isValid());

	obstructions[square][glyph]->block(captured);

	/* -- Handle castling -- */

	if (distances[ksquare] > (castling + 1))
		ksquare = initial;

	if (distances[qsquare] > (castling + 1))
		qsquare = initial;
}

/* -------------------------------------------------------------------------- */

void Movements::unblock(Square square, Glyph glyph, bool captured)
{
	assert(square.isValid());
	assert(glyph.isValid());

	obstructions[square][glyph]->unblock(captured);

	/* -- Handle castling -- */

	if (distances[_ksquare] == (castling + 1))
		ksquare = _ksquare;

	if (distances[_qsquare] == (castling + 1))
		qsquare = _qsquare;
}

/* -------------------------------------------------------------------------- */

void Movements::optimize()
{
	/* -- Remove all useless obstructions -- */

	for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
		if (validObstructions[glyph])
			for (Square square = FirstSquare; square <= LastSquare; square++)
				obstructions[square][glyph]->optimize();

	/* -- Count number of possible moves -- */

	possibilities = 0;
	for (Square from = FirstSquare; from <= LastSquare; from++)
		for (Square to = FirstSquare; to <= LastSquare; to++)
			if (movements[from][to] == 0)
				possibilities++;

	/* -- Recompute initial distances and captures -- */

	computeInitialDistances();
	computeInitialCaptures();
}

/* -------------------------------------------------------------------------- */

void Movements::reduce(Square square, int availableMoves, int availableCaptures)
{
	assert(square.isValid());
	assert(availableMoves >= 0);
	assert(availableCaptures >= 0);

	/* -- Compute distances and required captures to given square -- */

	int rdistances[NumSquares];
	int rcaptures[NumSquares];

	computeReverseDistances(square, rdistances);
	computeReverseCaptures(square, rcaptures);

	/* -- Eliminate unused movements given number of available moves -- */

	for (Square from = FirstSquare; from <= LastSquare; from++)
		for (Square to = FirstSquare; to <= LastSquare; to++)
			if (movements[from][to] == 0)
				if ((distances[from] + 1 + rdistances[to]) > availableMoves)
					movements[from][to] = infinity;

	/* -- Eliminate unused movements given number of available captures -- */

	if (hybrid)
		for (Square from = FirstSquare; from <= LastSquare; from++)
			for (Square to = FirstSquare; to <= LastSquare; to++)
				if (movements[from][to] == 0)
					if ((_captures[from] + ((tables::captures[glyph][from][to] && !tables::movements[glyph][from][to]) ? 1 : 0) + rcaptures[to]) > availableCaptures)
						movements[from][to] = infinity;
}

/* -------------------------------------------------------------------------- */

void Movements::reduce(Squares squares, int availableMoves, int availableCaptures)
{
	assert(!squares.none());
	assert(availableMoves >= 0);
	assert(availableCaptures >= 0);

	/* -- Let's not loose ourselves with too many computations -- */

	if (squares.count() > 8)
		return;

	/* -- Compute distances and required captures to given set of squares -- */

	int rdistances[NumSquares];
	int rcaptures[NumSquares];

	Square square = FirstSquare;
	while (!squares[square])
		square++;

	computeReverseDistances(square, rdistances);
	computeReverseCaptures(square, rcaptures);

	for (square++; square <= LastSquare; square++)
	{
		if (!squares[square])
			continue;

		int _rdistances[NumSquares];
		int _rcaptures[NumSquares];

		computeReverseDistances(square, _rdistances);
		computeReverseCaptures(square, _rcaptures);

		for (Square square = FirstSquare; square <= LastSquare; square++)
			rdistances[square] = std::min(rdistances[square], _rdistances[square]);
		
		for (Square square = FirstSquare; square <= LastSquare; square++)
			rcaptures[square] = std::min(rcaptures[square], _rcaptures[square]);
	}

	/* -- Eliminate unused movements given number of available moves -- */

	for (Square from = FirstSquare; from <= LastSquare; from++)
		for (Square to = FirstSquare; to <= LastSquare; to++)
			if (movements[from][to] == 0)
				if ((distances[from] + 1 + rdistances[to]) > availableMoves)
					movements[from][to] = infinity;

	/* -- Eliminate unused movements given number of available captures -- */

	if (hybrid)
		for (Square from = FirstSquare; from <= LastSquare; from++)
			for (Square to = FirstSquare; to <= LastSquare; to++)
				if (movements[from][to] == 0)
					if ((_captures[from] + ((tables::captures[glyph][from][to] && !tables::movements[glyph][from][to]) ? 1 : 0) + rcaptures[to]) > availableCaptures)
						movements[from][to] = infinity;
}

/* -------------------------------------------------------------------------- */

int Movements::moves() const
{
	return possibilities;
}

/* -------------------------------------------------------------------------- */

Squares Movements::squares() const
{
	int distances[NumSquares];
	computeForwardDistances(initial, distances);

	Squares squares;

	for (Square square = FirstSquare; square < LastSquare; square++)
		if (distances[square] < infinity)
			squares[square] = true;

	return squares;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

Board::Board()
{
	optimized = true;

	/* -- Initialize movement tables -- */

	for (Color color = FirstColor; color <= LastColor; color++)
		for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
			movements[color][superman] = new Movements(superman, color);
}

/* -------------------------------------------------------------------------- */

Board::~Board()
{
	/* -- Release movement tables -- */

	for (Color color = FirstColor; color <= LastColor; color++)
		for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
			delete movements[color][superman];
}

/* -------------------------------------------------------------------------- */

int Board::moves(Superman superman, Color color) const
{
	assert(superman.isValid());
	assert(color.isValid());

	return movements[color][superman]->moves();
}

/* -------------------------------------------------------------------------- */

int Board::moves(Man man, Superman superman, Color color) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());

	int moves = movements[color][man]->moves();

	if (superman != man)
		moves += movements[color][superman]->moves();

	return moves;
}

/* -------------------------------------------------------------------------- */

int Board::distance(Man man, Superman superman, Color color, Square from, Square to) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());

	/* -- No promotion case -- */

	if (man == superman)
		return movements[color][man]->distance(from, to);

	/* -- Handle promotions -- */

	Square square = superman.square(color);
	return movements[color][man]->distance(from, square) + movements[color][superman]->distance(square, to);
}

/* -------------------------------------------------------------------------- */

int Board::distance(Man man, Superman superman, Color color, Square to) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());

	/* -- Use extended algorithms if board movements have been altered -- */

	if (!optimized)
		return distance(man, superman, color, man.square(color), to);
			
	/* -- No promotion case -- */

	if (man == superman)
		return movements[color][man]->distance(to);

	/* -- Handle promotions -- */

	return movements[color][man]->distance(superman.square(color)) + movements[color][superman]->distance(to);
}

/* -------------------------------------------------------------------------- */

int Board::captures(Man man, Superman superman, Color color, Square from, Square to) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());

	/* -- No promotion case -- */

	if (man == superman)
		return movements[color][man]->captures(from, to);

	/* -- Handle promotions -- */

	Square square = superman.square(color);
	return movements[color][man]->captures(from, square) + movements[color][superman]->captures(square, to);
}

/* -------------------------------------------------------------------------- */

int Board::captures(Man man, Superman superman, Color color, Square to) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());

	/* -- Use extended algorithms if board movements have been altered -- */

	if (!optimized)
		return captures(man, superman, color, man.square(color), to);
			
	/* -- No promotion case -- */

	if (man == superman)
		return movements[color][man]->captures(to);

	/* -- Handle promotions -- */

	return movements[color][man]->captures(superman.square(color)) + movements[color][superman]->captures(to);
}

/* -------------------------------------------------------------------------- */

int Board::getCaptures(Man man, Superman superman, Color color, Square from, Square to, vector<Squares>& captures) const
{
	assert(man.isValid());
	assert(superman.isValid());
	assert(color.isValid());

	/* -- No promotion case -- */

	if (man == superman)
		return movements[color][man]->getCaptures(from, to, captures);

	/* -- Handle promotions -- */

	Square square = superman.square(color);
	vector<Squares> _captures;
	
	movements[color][man]->getCaptures(from, square, captures);
	movements[color][superman]->getCaptures(square, to, _captures);

	captures.insert(captures.end(), _captures.begin(), _captures.end());
	return (int)captures.size();
}

/* -------------------------------------------------------------------------- */

void Board::block(Superman man, Color color, Square square, bool captured)
{
	assert(man.isValid());
	assert(color.isValid());
	assert(square.isValid());

	Glyph glyph = man.glyph(color);
	optimized = false;

	/* -- Block movements for our side -- */

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		if (superman != man)
			movements[color][superman]->block(square, glyph, false);

	/* -- Block movements for opponent pieces -- */

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		movements[!color][superman]->block(square, glyph, captured);

}

/* -------------------------------------------------------------------------- */

void Board::unblock(Superman man, Color color, Square square, bool captured)
{
	assert(man.isValid());
	assert(color.isValid());
	assert(square.isValid());

	Glyph glyph = man.glyph(color);
	optimized = false;

	/* -- Unblock movements for our side -- */

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		if (superman != man)
			movements[color][superman]->unblock(square, glyph, false);

	/* -- Unblock movements for opponent pieces -- */

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		movements[!color][superman]->unblock(square, glyph, captured);
}

/* -------------------------------------------------------------------------- */

void Board::transblock(Superman superman, Color color, Square from, Square to, bool captured)
{
	unblock(superman, color, from);
	block(superman, color, to, captured);
}

/* -------------------------------------------------------------------------- */

void Board::block(Man man, Superman superman, Color color)
{
	assert(man.isValid());
	assert(superman.isValid());
	assert(color.isValid());

	/* -- Find squares on which the piece must lie -- */

	Squares squares = movements[color][man]->squares();

	if (man != superman)
		squares |= movements[color][superman]->squares();

	block(man, superman, color, squares);
}

/* -------------------------------------------------------------------------- */

void Board::block(Man man, Superman _superman, Color color, const Squares& squares)
{
	assert(man.isValid());
	assert(_superman.isValid());

	Glyph glyph = (man == _superman) ? man.glyph(color) : NoGlyph;
	optimized = false;
	
	/* -- Block movements for our side -- */

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		if ((superman != man) && (superman != _superman))
			movements[color][superman]->block(squares, glyph);

	/* -- Block movements for opponent pieces -- */

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		movements[!color][superman]->block(squares, glyph);
}

/* -------------------------------------------------------------------------- */

void Board::reduce(Man man, Superman superman, Color color, Square square, int availableMoves, int availableCaptures)
{
	assert(man.isValid());
	assert(superman.isValid());
	assert(color.isValid());

	/* -- No promotion case -- */

	if (man == superman)
		movements[color][man]->reduce(square, availableMoves, availableCaptures);

	/* -- Handle promotions -- */

	else
	{
		Square promotion = superman.square(color);

		int requiredMoves = movements[color][man]->distance(promotion);
		int requiredCaptures = movements[color][man]->captures(promotion);

		int promotionMoves = movements[color][superman]->distance(square);
		int promotionCaptures = movements[color][superman]->captures(square);

		movements[color][man]->reduce(promotion, availableMoves - promotionMoves, availableCaptures - promotionCaptures);
		movements[color][superman]->reduce(square, availableMoves - requiredMoves, availableCaptures - requiredCaptures);
	}
}

/* -------------------------------------------------------------------------- */

void Board::reduce(Man man, Superman superman, Color color, const Squares& squares, int availableMoves, int availableCaptures)
{
	assert(man.isValid());
	assert(superman.isValid());
	assert(color.isValid());

	/* -- No promotion case -- */

	if (man == superman)
		movements[color][man]->reduce(squares, availableMoves, availableCaptures);

	/* -- Handle promotions -- */

	else
	{
		Square promotion = superman.square(color);

		int requiredMoves = movements[color][man]->distance(promotion);
		int requiredCaptures = movements[color][man]->captures(promotion);
		int promotionMoves = movements[color][superman]->distance(squares);
		int promotionCaptures = movements[color][superman]->captures(squares);

		movements[color][man]->reduce(promotion, availableMoves - promotionMoves, availableCaptures - promotionCaptures);
		movements[color][superman]->reduce(squares, availableMoves - requiredMoves, availableCaptures - requiredCaptures);
	}
}

/* -------------------------------------------------------------------------- */

void Board::optimize(const Pieces& pieces, Color color, int availableMoves, int availableCaptures)
{
	/* -- Initialize information tied with each man -- */

	struct 
	{
		int availableMoves;
		int availableCaptures;

		Squares msquares;
		Squares squares;

		Supermen supermen;
		Superman superman;

		bool block;
			
	} men[NumMen];

	for (Man man = FirstMan; man <= LastMan; man++)
	{
		men[man].availableMoves = 0;
		men[man].availableCaptures = 0;

		men[man].block = true;
	}

	/* -- Loop through partitions, targets and destinations to collect the information -- */

	for	(Partitions::const_iterator partition = pieces.begin(); partition != pieces.end(); partition++)
	{
		int unassignedMoves = availableMoves - pieces.getRequiredMoves() + partition->getRequiredMoves() - partition->getAssignedMoves();
		int unassignedCaptures = availableCaptures - pieces.getRequiredCaptures() + partition->getRequiredCaptures() - partition->getAssignedCaptures();

		for (Targets::const_iterator target = partition->begin(); target != partition->end(); target++)
		{
			for (Destinations::const_iterator destination = target->begin(); destination != target->end(); destination++)
			{
				Man man = destination->man();
				Superman superman = destination->superman();
				Square square = destination->square();

				maximize(men[man].availableMoves, target->getRequiredMoves() + unassignedMoves);
				maximize(men[man].availableCaptures, target->getRequiredCaptures() + unassignedCaptures);

				men[man].squares[square] = true;
				if (superman == man)
					men[man].msquares[square] = true;
				else
					men[man].msquares[superman.square(color)] = true;

				men[man].superman = superman;
				men[man].supermen[superman] = true;

				if (destination->captured())
					men[man].block = false;
			}
		}
	}

	/* -- Count men to supermen links -- */

	array<int, NumSupermen> supermen(0);

	for (Man man = FirstMan; man <= LastMan; man++)
		for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
			if (men[man].supermen[superman])
				supermen[superman]++;

	/* -- Apply path reductions and obstructions for each man -- */

	for (Man man = FirstMan; man <= LastMan; man++)
	{
		/* -- Handle case where the promotion piece is known and appears only once -- */

		if ((men[man].supermen.count() == 1) && (supermen[men[man].superman] == 1))
		{
			reduce(man, men[man].superman, color, men[man].squares, men[man].availableMoves, men[man].availableCaptures);

			if (men[man].block)
				block(man, men[man].superman, color);
		}

		/* -- Leave out promotion -- */

		else
			reduce(man, man, color, men[man].msquares, men[man].availableMoves, men[man].availableCaptures);
	}

	/* -- Handle supermen that never came to be -- */

	for (Superman superman = FirstPromotedMan; superman <= LastPromotedMan; superman++)
		if (!supermen[superman])
			movements[color][superman]->reduce(superman.square(color), 0, 0);

	/* -- Complete optimization -- */

	if (!optimized)
		optimize();
}

/* -------------------------------------------------------------------------- */

void Board::optimize()
{
	optimized = true;

	for (Color color = FirstColor; color <= LastColor; color++)
		for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
			movements[color][superman]->optimize();
}

/* -------------------------------------------------------------------------- */

}
