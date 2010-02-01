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
			_movements[from][to].initialize(from, to, this, moves);

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

	/* -- Earliest and latest moves -- */

	_earliest = 1;
	_latest = moves;

	/* -- Restrictions will be filled later -- */

	_possibleSquares.set();
	_possibleCaptures.set();

	_availableMoves = infinity;
	_availableCaptures = infinity;

	/* -- Captured and promoted state -- */

	_captured = superman.isKing() ? false : indeterminate;
	_promoted = superman.isPawn() ? indeterminate : false;

	/* -- Fill in initial distances and required captures -- */

	computeInitialDistances();
	computeInitialCaptures();

	/* -- List possible destinations -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (_distances[square] < infinity)
			_squares[square] = true;

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

int Piece::rdistance(Square square) const
{
	assert(square.isValid());

	return _rdistances[square];
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
	computeReverseDistances(_possibleSquares, _rdistances);
}

/* -------------------------------------------------------------------------- */

void Piece::computeInitialCaptures()
{
	computeForwardCaptures(_initial, _captures);
	computeReverseCaptures(_possibleSquares, _rcaptures);
}

/* -------------------------------------------------------------------------- */

void Piece::updateInitialDistances()
{
	int distances[NumSquares];

	computeForwardDistances(_initial, distances);
	updateInitialDistances(distances, false);

	computeReverseDistances(_possibleSquares, distances);
	updateInitialDistances(distances, true);
}

/* -------------------------------------------------------------------------- */

void Piece::updateInitialCaptures()
{
	int captures[NumSquares];

	computeForwardCaptures(_initial, captures);
	updateInitialCaptures(captures, false);

	computeReverseCaptures(_possibleSquares, captures);
	updateInitialCaptures(captures, true);
}

/* -------------------------------------------------------------------------- */

void Piece::updateInitialDistances(const int distances[NumSquares], bool reverse)
{
	maximize(reverse ? _rdistances : _distances, distances, NumSquares);
}

/* -------------------------------------------------------------------------- */

void Piece::updateInitialCaptures(const int captures[NumSquares], bool reverse)
{
	maximize(reverse ? _rcaptures : _captures, captures, NumSquares);
}

/* -------------------------------------------------------------------------- */

void Piece::computeForwardDistances(Square square, int distances[NumSquares], bool castling) const
{
	/* -- Initialize distances -- */

	std::fill(distances, distances + NumSquares, infinity);
	distances[square] = 0;

	if ((square == _initial) && castling)
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
	
	if ((square == _initial) && castling)
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

void Piece::computeForwardCaptures(Square square, int captures[NumSquares], bool /*castling*/) const
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

void Piece::computeReverseDistances(const Squares& squares, int distances[NumSquares]) const
{
	assert(squares.any());

	/* -- Let's make detailed computation only if there is not too many possibilites -- */

	if (squares.count() < 8)
	{
		bool initialize = true;

		/* -- Compute minimum distances to reach given squares -- */

		for (Square square = FirstSquare; square <= LastSquare; square++)
		{
			if (squares[square])
			{
				if (initialize)
				{
					computeReverseDistances(square, distances);
					initialize = false;
				}
				else
				{
					int rdistances[NumSquares];
					computeReverseDistances(square, rdistances);
					minimize(distances, rdistances, NumSquares);
				}
			}
		}
	}
	else
	{
		/* -- Fallback case, assume we need one move to reach any destination square -- */

		for (Square square = FirstSquare; square <= LastSquare; square++)
			distances[square] = squares[square] ? 0 : 1;
	}
}

/* -------------------------------------------------------------------------- */

void Piece::computeReverseCaptures(const Squares& squares, int captures[NumSquares]) const
{
	assert(squares.any());

	/* -- Let's make detailed computation only if there is not too many possibilites -- */

	if ((squares.count() < 8) && _hybrid)
	{
		bool initialize = true;

		/* -- Compute minimum distances to reach given squares -- */

		for (Square square = FirstSquare; square <= LastSquare; square++)
		{
			if (squares[square])
			{
				if (initialize)
				{
					computeReverseCaptures(square, captures);
					initialize = false;
				}
				else
				{
					int rcaptures[NumSquares];
					computeReverseCaptures(square, rcaptures);
					minimize(captures, rcaptures, NumSquares);
				}
			}
		}
	}
	else
	{
		/* -- Fallback case, assume we do not need any captures to reach destination squares -- */

		std::fill(captures, captures + NumSquares, 0);
	}
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

void Piece::findMandatoryMoves()
{
	bool backward = true;

	/* -- Follow path from initial square until there is more than one possibility -- */

	if (!indeterminate(_kcastling) && !indeterminate(_qcastling))
	{
		/* -- Take castling into account -- */

		Square current = _initial;
		if (_kcastling)
			current = _ksquare;
		if (_qcastling)
			current = _qsquare;

		/* -- Loop until we have reached a possible destination square -- */

		while (_rdistances[current] > 0)
		{
			Square destination = current;
			int possibilities  = 0;

			for (Square square = FirstSquare; square <= LastSquare; square++)
			{
				if (_movements[current][square].possible())
				{
					destination = square;
					possibilities++;
				}
			}

			/* -- If there is more than one possibility, stop -- */

			if (possibilities != 1)
				break;

			/* -- Label current movement as mandatory and move on -- */
	
			_movements[current][destination].validate();
			current = destination;
		}

		/* -- If we have reached a possible destination square, no need to perform backward analysis -- */

		if (_possibleSquares[current])
			backward = false;
	}

	/* -- Follow path backward from destination square if it is unique -- */

	if (backward && (_possibleSquares.count() == 1))
	{
		/* -- Find unique destination square -- */

		Square current;
		for (current = FirstSquare; current <= LastSquare; current++)
			if (_possibleSquares[current])
				break;

		/* -- Loop until we have almost reached a departure square -- */

		while (_distances[current] > 1)
		{
			Square source = current;
			int possibilities = 0;

			for (Square square = FirstSquare; square <= LastSquare; square++)
			{
				if (_movements[square][current].possible())
				{
					source = square;
					possibilities++;
				}
			}

			/* -- If there is more than one possibility, stop -- */

			if (possibilities != 1)
				break;

			/* -- Label current movement as mandatory and move backward -- */

			_movements[source][current].validate();
			current = source;
		}
	}
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

int Piece::getMandatoryMoves(vector<Move *>& moves) const
{
	/* -- Initialize output -- */

	moves.clear();

	/* -- Scan list of moves to find mandatory ones -- */

	for (Moves::const_iterator move = _moves.begin(); move != _moves.end(); move++)
		if (move->mandatory())
			moves.push_back(*move);

	/* -- Return number of moves found -- */

	return moves.size();
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

	/* -- Castling deductions -- */

	optimizeCastling();

	/* -- Distance computations and moves optimizations are performed recursively -- */

	int moves = infinity;
	while (moves > this->moves())
	{
		moves = this->moves();

		/* -- Recompute initial distances and captures -- */

		updateInitialDistances();
		updateInitialCaptures();

		/* -- Eliminate unused movements given number of available moves and captures -- */

		for (Moves::iterator move = _moves.begin(); move != _moves.end(); move++)
			if ((_distances[move->from()] + 1 + _rdistances[move->to()]) > _availableMoves)
				move->invalidate();

		if (_hybrid)
			for (Moves::iterator move = _moves.begin(); move != _moves.end(); move++)
				if ((_captures[move->from()] + (move->capture() ? 1 : 0) + _rcaptures[move->to()]) > _availableCaptures)
					move->invalidate();

		/* -- Keep only possible moves -- */

		_moves.erase(std::remove_if(_moves.begin(), _moves.end(), isMoveImpossible), _moves.end());

		/* -- Get earliest and latest moves -- */

		int earliest = infinity;
		int latest = 0;

		for (Moves::iterator move = _moves.begin(); move != _moves.end(); move++)
			minimize(earliest, move->earliest()), maximize(latest, move->latest());

		maximize(_earliest, earliest);
		minimize(_latest, latest);

		/* -- Set obvious lower and upper bounds for move order -- */

		for (Moves::iterator move = _moves.begin(); move != _moves.end(); move++)
			move->bound(_earliest + _distances[move->from()], _latest - _rdistances[move->to()]);

		/* -- Keep only possible moves -- */

		_moves.erase(std::remove_if(_moves.begin(), _moves.end(), isMoveImpossible), _moves.end());
	}

	/* -- Find out mandatory moves -- */

	findMandatoryMoves();

	/* -- Remove all useless obstructions -- */

	for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
		if (_validObstructions[glyph])
			for (Square square = FirstSquare; square <= LastSquare; square++)
				_obstructions[square][glyph]->optimize();

	/* -- Tabulate list of possible destination squares -- */

	_squares.reset();
	_squares[_initial] = true;

	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (_distances[square] < infinity)
			_squares[square] = true;

	/* -- Sort moves -- */

	std::sort(_moves.begin(), _moves.end(), boost::bind(isMoveEarlier, _1, _2, cref(*this)));

	/* -- All done -- */

	_optimized = true;
}

/* -------------------------------------------------------------------------- */

bool Piece::constrain()
{
	bool modified = false;

	/* -- Apply constraints to every move -- */

	for (Moves::iterator move = _moves.begin(); move != _moves.end(); move++)
		if (move->constrain())
			modified = true;

	/* -- Optimize if necessary -- */

	if (modified)
		_optimized = false;

	optimize();

	/* -- Done -- */

	return modified;
}

/* -------------------------------------------------------------------------- */

void Piece::setPossibleSquares(const Squares& squares, tribool captured, int availableMoves, int availableCaptures)
{
	assert(!squares.none());
	assert(availableMoves >= 0);
	assert(availableCaptures >= 0);

	/* -- Normalize 'captured' tribool parameter -- */

	if (indeterminate(captured) && !indeterminate(_captured))
		captured = _captured;

	/* -- Return if we already have this knowledge -- */

	if (squares.count() >= _possibleSquares.count())
		if (captured == _captured)
			if (availableMoves >= _availableMoves)
				if (availableCaptures >= _availableCaptures)
					return;

	/* -- Update state -- */

	_possibleSquares = squares;
	_captured = captured;

	_availableMoves = availableMoves;
	_availableCaptures = availableCaptures;

	/* -- Handle promotion -- */

	if (indeterminate(_promoted))
	{
		bool promoted = false;
		bool unpromoted = false;

		for (Square square = FirstSquare; square <= LastSquare; square++)
		{
			if (_possibleSquares[square])
			{
				if (square.isPromotion(_color))
					promoted = true;
				else
					unpromoted = true;
			}
		}

		if (promoted && !unpromoted)
			_promoted = true;
		else
		if (!promoted)
			_promoted = false;
	}

	/* -- Schedule for optimization -- */

	_optimized = false;
}

/* -------------------------------------------------------------------------- */

void Piece::setMandatoryMoves(const Piece& piece, const Moves& moves)
{
	/* -- Don't mess with ourself -- */

	if (&piece == this)
		return;

	/* -- Don't bother if there is no mandatory moves -- */

	if (moves.empty())
		return;

	/* -- Let's put aside castling for now and start from initial square -- */

	if (!piece._kcastling && !piece._qcastling && piece.alive(false))
	{
		Square square = piece._initial;
		Obstructions obstructions(*_obstructions[square][piece.glyph()]);
		
		for (Moves::const_iterator constraint = moves.begin(); constraint != moves.end(); constraint++)
		{
			/* -- Check for end of sequence of continuous mandatory moves -- */

			if (constraint->from() != square)
				break;

			/* -- Get common obstructions for all moves up to current one -- */

			if (constraint != moves.begin())
				obstructions &= *_obstructions[constraint->from()][piece.glyph()];

			/* -- If there is no more obstructions, there is nothing mode to do -- */

			if (!obstructions.obstructions())
				break;

			/* -- These obstructions are the moves that are constrained -- */

			for (int move = 0; move < obstructions.obstructions(); move++)
				obstructions[move]->constraints()->mustFollow(constraint->piece(), *constraint);

			/* -- Update current square -- */

			square = constraint->to();
		}
	}

	/* -- Let's do the same starting from the end -- */

	if (piece.alive())
	{
		/* -- Let's find ending square -- */

		Square square = FirstSquare;
		while (!piece._possibleSquares[square])
			square++;

		Obstructions obstructions(*_obstructions[square][piece.glyph()]);

		/* -- Do the same as above, but backward -- */

		for (Moves::const_reverse_iterator constraint = moves.rbegin(); constraint != moves.rend(); constraint++)
		{
			if ((*constraint)->to() != square)
				break;

			if (constraint != moves.rbegin())
				obstructions &= *_obstructions[(*constraint)->to()][piece.glyph()];

			if (!obstructions.obstructions())
				break;

			for (int move = 0; move < obstructions.obstructions(); move++)
				obstructions[move]->constraints()->mustPreceed((*constraint)->piece(), *constraint);

			square = (*constraint)->from();
		}
	}
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

void Piece::optimizeCastling()
{
	/* -- Try possible castlings -- */

	if (indeterminate(_kcastling) || indeterminate(_qcastling))
	{
		/* -- Compute distances assuming castling state -- */

		int distances[NumSquares];
		int kdistances[NumSquares];
		int qdistances[NumSquares];

		int distance = infinity;
		int kdistance = infinity;
		int qdistance = infinity;

		computeForwardDistances(_initial, distances, false);
		for (Square square = FirstSquare; square <= LastSquare; square++)
			if (_possibleSquares[square])
				minimize(distance, distances[square]);

		if (indeterminate(_kcastling))
		{
			computeForwardDistances(_ksquare, kdistances, false);
			for (Square square = FirstSquare; square <= LastSquare; square++)
				if (_possibleSquares[square])
					minimize(kdistance, kdistances[square]);

			kdistance += _castling;
		}

		if (indeterminate(_qcastling))
		{
			computeForwardDistances(_qsquare, qdistances, false);
			for (Square square = FirstSquare; square <= LastSquare; square++)
				if (_possibleSquares[square])
					minimize(qdistance, qdistances[square]);

			qdistance += _castling;
		}

		/* -- Eliminate impossible castlings -- */

		if (indeterminate(_kcastling) && (kdistance > _availableMoves))
			_kcastling = false;

		if (indeterminate(_qcastling) && (qdistance > _availableMoves))
			_qcastling = false;

		if (indeterminate(_kcastling) && !_qcastling && (distance > _availableMoves))
			_kcastling = true;

		if (indeterminate(_qcastling) && !_kcastling && (distance > _availableMoves))
			_qcastling = true;
	}
}

/* -------------------------------------------------------------------------- */

void Piece::synchronizeCastling(Piece& krook, Piece& qrook)
{
	assert(_superman.isKing());

	/* -- Synchronize castling deductions between king and rooks -- */

	if (!indeterminate(_kcastling) && indeterminate(krook._kcastling))
	{
		krook._kcastling = _kcastling;
		krook._optimized = false;
	}

	if (!indeterminate(_qcastling) && indeterminate(qrook._qcastling))
	{
		qrook._qcastling = _qcastling;
		qrook._optimized = false;
	}
	
	if (!indeterminate(krook._kcastling) && indeterminate(_kcastling))
	{
		_kcastling = krook._kcastling;
		_optimized = false;
	}

	if (!indeterminate(qrook._qcastling) && indeterminate(_qcastling))
	{
		_qcastling = qrook._qcastling;
		_optimized = false;
	}

	/* -- Basic coherency checks -- */

	if (!indeterminate(_kcastling))
		if (_kcastling != krook._kcastling)
			abort(NoSolution);

	if (!indeterminate(_qcastling))
		if (_qcastling != qrook._qcastling)
			abort(NoSolution);
}

/* -------------------------------------------------------------------------- */

int Piece::moves() const
{
	int moves = (int)_moves.size();

	if (_superman.isKing())
	{
		moves += (indeterminate(_kcastling) ? _castling : 0);
		moves += (indeterminate(_qcastling) ? _castling : 0);
	}	

	return moves;
}

/* -------------------------------------------------------------------------- */

const Squares& Piece::squares() const
{
	return _squares;
}

/* -------------------------------------------------------------------------- */

tribool Piece::alive(bool final) const
{
	if (!final)
		return !_superman.isPromoted();

	if (!_captured && !_promoted)
		return true;

	if (_captured || _promoted)
		return false;

	return indeterminate;
}

/* -------------------------------------------------------------------------- */

}