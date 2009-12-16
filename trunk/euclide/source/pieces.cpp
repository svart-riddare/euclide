#include "pieces.h"
#include "moves.h"
#include "obstructions.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Piece::Piece(Superman superman, Color color, int moves)
	: _superman(superman), _color(color), _glyph(superman.glyph(color))
{
	assert(superman.isValid());
	assert(color.isValid());

	/* -- Initialize initial squares -- */

	_initial = superman.square(color);
	_ksquare = _initial;
	_qsquare = _initial;

	/* -- Handle castling -- */

	_castling = (superman == King) ? 1 : 0;

	if (superman == King)
		_ksquare = Square((column_t)(_initial.column() + 2), _initial.row());

	if (superman == King)
		_qsquare = Square((column_t)(_initial.column() - 2), _initial.row());

	if (superman == KingRook)
		_ksquare = Square((column_t)(_initial.column() - 2), _initial.row());

	if (superman == QueenRook)
		_qsquare = Square((column_t)(_initial.column() + 3), _initial.row());

	_kcastling = (_ksquare != _initial) ? indeterminate : tribool(false);
	_qcastling = (_qsquare != _initial) ? indeterminate : tribool(false);

	/* -- Initialize movement tables -- */

	for (Square from = FirstSquare; from <= LastSquare; from++)
		for (Square to = FirstSquare; to <= LastSquare; to++)
			_movements[from][to].initialize(from, to, superman, color, moves);

	/* -- List valid movements -- */

	for (Square from = FirstSquare; from <= LastSquare; from++)
		for (Square to = FirstSquare; to <= LastSquare; to++)
			if (_movements[from][to].possible())
				_moves.push_back(&_movements[from][to]);

	/* -- Check if some moves require captures -- */

	_hybrid = false;
	for (Moves::const_iterator move = _moves.begin(); move != _moves.end(); move++)
		if (move->capture())
			_hybrid = true;

	/* -- Fill in initial distances and required captures -- */

	computeInitialDistances();
	computeInitialCaptures();

	/* -- List possible destinations -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (_distances[square] < infinity)
			_squares[square] = true;

	/* -- Restrictions will be filled later -- */

	_possibleSquares.set();
	_possibleCaptures.set();

	_availableMoves = infinity;
	_availableCaptures = infinity;

	/* -- Optimization will be performed when movements are blocked or constraints are updated -- */

	_optimized = true;

	/* -- Initialize obstruction tables -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
	{
		_obstructions[square][NoGlyph] = new Obstructions(superman, color, square, NoGlyph, _movements);
		_validObstructions[NoGlyph] = true;

		for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
		{
			if ((superman.isKing() || glyph.isKing()) && (glyph.color() == !color))
			{
				_obstructions[square][glyph] = new Obstructions(superman, color, square, glyph, _movements);
				_validObstructions[glyph] = true;
			}
			else
			{
				_obstructions[square][glyph] = _obstructions[square][NoGlyph];
			}
		}
	}
}

/* -------------------------------------------------------------------------- */

Piece::~Piece()
{
	/* -- Delete obstruction tables -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
		for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
			if (_validObstructions[glyph])
				delete _obstructions[square][glyph];

	/* -- Delete moves -- */

	_moves.clear();
}

/* -------------------------------------------------------------------------- */

int Piece::distance(Square square) const
{
	assert(square.isValid());

	return _distances[square];
}

/* -------------------------------------------------------------------------- */

int Piece::captures(Square square) const
{
	assert(square.isValid());

	return _captures[square];
}

/* -------------------------------------------------------------------------- */

int Piece::distance(const Squares& squares) const
{
	int distance = infinity;

	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (squares[square])
			minimize(distance, _distances[square]);

	return distance;
}

/* -------------------------------------------------------------------------- */

int Piece::captures(const Squares& squares) const
{
	int captures = infinity;

	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (squares[square])
			minimize(captures, _captures[square]);

	return captures;
}

/* -------------------------------------------------------------------------- */

int Piece::distance(Square from, Square to) const
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

			if (!_movements[from][square].possible())
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

int Piece::captures(Square from, Square to) const
{
	assert(from.isValid());
	assert(to.isValid());

	/* -- Do we need to capture at all ? -- */

	if (!_hybrid)
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

			if (!_movements[from][square].possible())
				continue;

			/* -- Check if the square has been attained by a quicker path -- */

			if (distances[square] >= 0)
				continue;
			
			/* -- Add square to queue -- */

			distances[square] = distance + (_movements[from][square].capture() ? 1 : 0);
			squares.push(square);
			
			/* -- Have we reached our destination? -- */

			if (square == to)
				return distances[square];
		}
	}

	return infinity;
}

/* -------------------------------------------------------------------------- */

void Piece::computeInitialDistances()
{
	computeForwardDistances(_initial, _distances);
}

/* -------------------------------------------------------------------------- */

void Piece::computeInitialCaptures()
{
	computeForwardCaptures(_initial, _captures);
}

/* -------------------------------------------------------------------------- */

void Piece::updateInitialDistances()
{
	int distances[NumSquares];

	computeForwardDistances(_initial, distances);
	updateInitialDistances(distances);
}

/* -------------------------------------------------------------------------- */

void Piece::updateInitialCaptures()
{
	int captures[NumSquares];

	computeForwardCaptures(_initial, captures);
	updateInitialCaptures(captures);
}

/* -------------------------------------------------------------------------- */

void Piece::updateInitialDistances(const int distances[NumSquares])
{
	for (Square square = FirstSquare; square <= LastSquare; square++)
		maximize(_distances[square], distances[square]);
}

/* -------------------------------------------------------------------------- */

void Piece::updateInitialCaptures(const int captures[NumSquares])
{
	for (Square square = FirstSquare; square <= LastSquare; square++)
		maximize(_captures[square], captures[square]);
}

/* -------------------------------------------------------------------------- */

bool Piece::obstrusive(const vector<Square>& squares, Glyph glyph) const
{
	bool obstrusive = false;

	for (vector<Square>::const_iterator square = squares.begin(); square != squares.end(); square++)
		if (_obstructions[*square][glyph]->obstructions())
			obstrusive = true;

	return obstrusive;
}

/* -------------------------------------------------------------------------- */

void Piece::computeForwardDistances(Square square, const vector<Square>& obstructions, Glyph glyph, int distances[NumSquares])
{
	/* -- Start by blocking first square -- */

	if (obstructions.size() > 0)
		block(obstructions[0], glyph, false);

	/* -- Compute initial distances -- */

	computeForwardDistances(square, distances);

	/* -- Unblock first square -- */

	if (obstructions.size() > 0)
		unblock(obstructions[0], glyph, false);

	/* -- Perhaps we are done ? -- */

	if ((obstructions.size() < 2) || !obstrusive(obstructions, glyph))
		return;

	/* -- List of squares, ordered by distance -- */

	array<Square, NumSquares> squares;
	for (Square square = FirstSquare; square < NumSquares; square++)
		squares[square] = square;

	std::sort(squares.begin(), squares.end(), _smaller<int, NumSquares>(distances));

	/* -- Block each square of the obstructing path and refine distances -- */

	for (int obstruction = 1; obstruction < (int)obstructions.size(); obstruction++)
	{
		block(obstructions[obstruction], glyph, false);

		/* -- Handle each square, by increasing distance -- */

		for (int s = 0; s < NumSquares; s++)
		{
			Square from = squares[s];
			bool modified = false;

			/* -- Handle every possible immediate destination -- */

			for (Square to = FirstSquare; to <= LastSquare; to++)
			{
				if (!_movements[from][to].possible())
					continue;

				int distance = distances[from] + 1;
			
				/* -- Is it a quicker path ? -- */

				if (distance > distances[to])
					continue;

				distances[to] = distance;
				modified = true;
			}

			/* -- Sort array of squares given new distances -- */

			if (modified)
				std::sort(squares.begin() + s, squares.end(), _smaller<int, NumSquares>(distances));
		}

		/* -- Prepare for next iteration -- */

		unblock(obstructions[obstruction], glyph, false);
	}
}

/* -------------------------------------------------------------------------- */

void Piece::computeForwardDistances(Square square, int distances[NumSquares]) const
{
	/* -- Initialize distances -- */

	std::fill(distances, distances + NumSquares, infinity);
	distances[square] = 0;

	if (square == _initial)
	{
		if (_kcastling || _qcastling)
			distances[_initial] = infinity;
		if (_kcastling || indeterminate(_kcastling))
			distances[_ksquare] = _castling;
		if (_qcastling || indeterminate(_qcastling))
			distances[_qsquare] = _castling;
	}		
	
	/* -- Initialize square queue -- */

	queue<Square> squares;
	if (!distances[square])
		squares.push(square);
	
	if (square == _initial)
	{
		if (_kcastling || indeterminate(_kcastling))
			squares.push(_ksquare);
		if (_qcastling || indeterminate(_qcastling))
			squares.push(_qsquare);
	}

	/* -- Loop until every reachable square has been handled -- */

	while (!squares.empty())
	{
		Square from = squares.front(); squares.pop();

		/* -- Handle every possible immediate destination -- */

		for (Square to = FirstSquare; to <= LastSquare; to++)
		{
			if (!_movements[from][to].possible())
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

void Piece::computeForwardCaptures(Square square, int captures[NumSquares]) const
{
	/* -- Initialize captures -- */

	std::fill(captures, captures + NumSquares, _hybrid ? -1 : 0);
	captures[square] = 0;

	if (!_hybrid)
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
			if (!_movements[from][to].possible())
				continue;

			/* -- This square may have been attained with less captures -- */

			if (captures[to] >= 0)
				continue;

			/* -- Set square number of required captures -- */

			captures[to] = captures[from] + (_movements[from][to].capture() ? 1 : 0);

			/* -- Add it to queue of reachable destinations -- */

			squares.push(to);
		}
	}

	/* -- Don't leave negative values in the table -- */

	std::replace(captures, captures + NumSquares, -1, infinity);
}

/* -------------------------------------------------------------------------- */

void Piece::computeReverseDistances(Square square, int distances[NumSquares]) const
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
			if (!_movements[from][to].possible())
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

void Piece::computeReverseCaptures(Square square, int captures[NumSquares]) const
{
	/* -- Initialize captures -- */

	std::fill(captures, captures + NumSquares, _hybrid ? -1 : 0);
	captures[square] = 0;

	if (!_hybrid)
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
			if (!_movements[from][to].possible())
				continue;

			/* -- This square may have been attained with less captures -- */

			if (captures[from] >= 0)
				continue;

			/* -- Set square number of required captures -- */

			captures[from] = captures[to] + (_movements[from][to].capture() ? 1 : 0);

			/* -- Add it to queue of source squares -- */

			squares.push(from);
		}
	}

	/* -- Don't leave negative values in the table -- */

	std::replace(captures, captures + NumSquares, -1, infinity);
}

/* -------------------------------------------------------------------------- */

bool Piece::mayLeave(Square square) const
{
	for (Square s = FirstSquare; s <= LastSquare; s++)
		if (_movements[square][s].possible())
			return true;

	return false;
}

/* -------------------------------------------------------------------------- */

bool Piece::mayReach(Square square) const
{
	for (Square s = FirstSquare; s <= LastSquare; s++)
		if (_movements[s][square].possible())
			return true;

	return false;
}

/* -------------------------------------------------------------------------- */

int Piece::getCaptures(Square from, Square to, vector<Squares>& captures) const
{
	assert(from.isValid());
	assert(to.isValid());

	/* -- Some pieces does not need to capture in order to move -- */

	if (!_hybrid)
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

			if (!_movements[from][square].possible())
				continue;

			/* -- Compute distance (number of captures) -- */

			int distance = distances[from] + (_movements[from][square].capture() ? 1 : 0);

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
				if (!_movements[*I][square].capture())
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

bool Piece::getUniquePath(Square from, Square to, vector<Square>& squares) const
{
	assert(from.isValid());
	assert(to.isValid());

	/* -- Check castling -- */

	if (from == _initial)
		if (indeterminate(_kcastling) || indeterminate(_qcastling))
			return false;
		
	/* -- Initialize list of squares -- */

	squares.clear();

	if (from == _initial) 
	{
		if (_kcastling)
			squares.push_back(_ksquare);
		else
		if (_qcastling)
			squares.push_back(_qsquare);
	}

	if (squares.empty())
		squares.push_back(from);

	/* -- Follow path(s) and build square lite -- */

	while (squares.size() <= NumSquares)
	{
		Square current = squares.back();
		Square next = UndefinedSquare;

		/* -- Find immediate destinations from current square -- */

		for (Square square = FirstSquare; square <= LastSquare; square++)
		{
			if (!_movements[current][square].possible())
				continue;

			/* -- Return if path is not unique -- */

			if (next != UndefinedSquare)
				return false;

			next = square;
		}

		/* -- Return if we can go elsewere than our target -- */

		if (current == to)
			return (next == UndefinedSquare) ? true : false;

		/* -- Add square to path -- */

		assert(next != UndefinedSquare);
		squares.push_back(next);		
	}

	/* -- We should not be here -- */

	assert(squares.back() == to);
	return false;
}

/* -------------------------------------------------------------------------- */

void Piece::block(Squares squares, Glyph glyph, bool definitive)
{
	assert(!squares.none());
	assert(glyph.isValid());

	Square square = FirstSquare;
	while (!squares[square])
		square++;

	if (squares.count() == 1)
		return block(square, glyph, false, definitive);

	/* -- Find common obstructions -- */

	Obstructions obstructions(*_obstructions[square++][glyph]);

	for ( ; square <= LastSquare; square++)
		if (squares[square])
			obstructions &= *_obstructions[square][glyph];

	/* -- Apply them -- */

	int blocked = obstructions.block(false);

	/* -- Have any movements been definitively blocked ? -- */

	if (definitive && blocked)
		_optimized = false;
}

/* -------------------------------------------------------------------------- */

void Piece::block(Square square, Glyph glyph, bool captured, bool definitive)
{
	assert(square.isValid());
	assert(glyph.isValid());

	/* -- Apply obstructions -- */

	int blocked = _obstructions[square][glyph]->block(captured);

	/* -- Have any movements been definitively blocked ? -- */

	if (definitive && blocked)
		_optimized = false;
}

/* -------------------------------------------------------------------------- */

void Piece::unblock(Square square, Glyph glyph, bool captured)
{
	assert(square.isValid());
	assert(glyph.isValid());

	/* -- Undo obstructions -- */

	_obstructions[square][glyph]->unblock(captured);
}

/* -------------------------------------------------------------------------- */

void Piece::optimize()
{
	/* -- Don't bother if there is nothing to do -- */

	if (_optimized)
		return;

	/* -- Eliminate impossible captures given possible capture squares -- */

	if (_hybrid && (_possibleCaptures.count() < NumSquares))
		for (Moves::iterator move = _moves.begin(); move != _moves.end(); move++)
			if (move->capture() && !_possibleCaptures[move->to()])
				move->invalidate();

	/* -- Remove obvious possibilities given number of moves and captures -- */

	for (Moves::iterator move = _moves.begin(); move != _moves.end(); move++)
		if ((_distances[move->to()] > _availableMoves) || (_captures[move->to()] > _availableCaptures))
			move->invalidate();

	/* -- Keep only possible moves -- */

	_moves.erase(std::remove_if(_moves.begin(), _moves.end(), isMoveImpossible), _moves.end());

	/* -- Obvious castling deductions -- */

	if (!moves() && indeterminate(_kcastling))
		if ((!_possibleSquares[_initial] && !_possibleSquares[_qsquare]) || !_possibleSquares[_ksquare])
			_kcastling = _possibleSquares[_ksquare] ? true : false;

	if (!moves() && indeterminate(_qcastling))
		if ((!_possibleSquares[_initial] && !_possibleSquares[_ksquare]) || !_possibleSquares[_qsquare])
			_qcastling = _possibleSquares[_qsquare] ? true : false;

	/* -- Recompute initial distances and captures -- */

	updateInitialDistances();
	updateInitialCaptures();

	/* -- Tabulate list of possible destination squares -- */

	_squares.reset();
	_squares[_initial] = true;

	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (_distances[square] < infinity)
			_squares[square] = true;

	/* -- Eliminate unused movements without making too much computations -- */

	if (_possibleSquares.count() < 8)
	{
		/* -- Compute distances and required captures to given set of possible destination squares -- */

		int rdistances[NumSquares];
		int rcaptures[NumSquares];

		Square square = FirstSquare;
		while (!_possibleSquares[square])
			square++;

		computeReverseDistances(square, rdistances);
		computeReverseCaptures(square, rcaptures);

		for (square++; square <= LastSquare; square++)
		{
			if (_possibleSquares[square])
			{
				int trdistances[NumSquares];
				int trcaptures[NumSquares];
	
				computeReverseDistances(square, trdistances);
				computeReverseCaptures(square, trcaptures);

				minimize(rdistances, trdistances, NumSquares);
				minimize(rcaptures, trcaptures, NumSquares);
			}
		}

		/* -- Eliminate unused movements given number of available moves and captures -- */

		for (Moves::iterator move = _moves.begin(); move != _moves.end(); move++)
			if ((_distances[move->from()] + 1 + rdistances[move->to()]) > _availableMoves)
				move->invalidate();

		if (_hybrid)
			for (Moves::iterator move = _moves.begin(); move != _moves.end(); move++)
				if ((_captures[move->from()] + (move->capture() ? 1 : 0) + rcaptures[move->to()]) > _availableCaptures)
					move->invalidate();

		/* -- Keep only possible moves -- */

		_moves.erase(std::remove_if(_moves.begin(), _moves.end(), isMoveImpossible), _moves.end());
	}

	/* -- Further castling deductions -- */

	if (indeterminate(_kcastling) || indeterminate(_qcastling))
	{
		if (!_possibleSquares[_initial] && !mayLeave(_initial))
		{
			if (!_possibleSquares[_qsquare] && !mayLeave(_qsquare))
				_kcastling = true;

			if (!_possibleSquares[_ksquare] && !mayLeave(_ksquare))
				_qcastling = true;
		}
	}

	/* -- Remove all useless obstructions -- */

	for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
		if (_validObstructions[glyph])
			for (Square square = FirstSquare; square <= LastSquare; square++)
				_obstructions[square][glyph]->optimize();

	/* -- All done -- */

	_optimized = true;
}

/* -------------------------------------------------------------------------- */

void Piece::optimize(const vector<tuple<Man, Color, vector<Square> > >& paths)
{
	if (!moves())
		return;

	/* -- For each given path -- */

	for (vector<tuple<Man, Color, vector<Square> > >::const_iterator path = paths.begin(); path != paths.end(); path++)
	{
		Man man = get<0>(*path);
		Color color = get<1>(*path);
		const vector<Square>& squares = get<2>(*path);

		/* -- Let's not block ourself -- */

		if ((man == _superman) && (color == _color))
			continue;

		/* -- Compute distances taking into account the path of another piece -- */

		int distances[NumSquares];
		computeForwardDistances(_initial, squares, man.glyph(color), distances);
		updateInitialDistances(distances);
	}

	/* -- Finish optimization -- */

	optimize();
}

/* -------------------------------------------------------------------------- */

void Piece::setPossibleSquares(const Squares& squares, int availableMoves, int availableCaptures)
{
	assert(!squares.none());
	assert(availableMoves >= 0);
	assert(availableCaptures >= 0);

	/* -- Return if we already have this knowledge -- */

	if (squares.count() >= _possibleSquares.count())
		if (availableMoves >= _availableMoves)
			if (availableCaptures >= _availableCaptures)
				return;

	/* -- Update state -- */

	_possibleSquares = squares;

	_availableMoves = availableMoves;
	_availableCaptures = availableCaptures;

	/* -- Schedule for optimization -- */

	_optimized = false;
}

/* -------------------------------------------------------------------------- */

void Piece::setPossibleCaptures(const Squares& captures)
{
	/* -- Return if we already have this knowledge -- */

	if (captures.count() >= _possibleCaptures.count())
		return;

	/* -- Update state -- */

	_possibleCaptures = captures;

	/* -- Schedule for optimisation -- */

	if (_hybrid)
		_optimized = false;
}

/* -------------------------------------------------------------------------- */

void Piece::synchronizeCastling(Piece& krook, Piece& qrook)
{
	assert(_superman.isKing());

	/* -- Synchronize castling deductions between king and rooks -- */

	if (!indeterminate(_kcastling))
		krook._kcastling = _kcastling;
	if (!indeterminate(_qcastling))
		qrook._qcastling = _qcastling;
	
	if (!indeterminate(krook._kcastling))
		_kcastling = krook._kcastling;
	if (!indeterminate(qrook._qcastling))
		_qcastling = qrook._qcastling;
}

/* -------------------------------------------------------------------------- */

int Piece::moves() const
{
	return (int)_moves.size();
}

/* -------------------------------------------------------------------------- */

const Squares& Piece::squares() const
{
	return _squares;
}

/* -------------------------------------------------------------------------- */

}
