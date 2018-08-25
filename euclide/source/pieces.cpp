#include "pieces.h"
#include "problem.h"
#include "tables/tables.h"
#include "cache.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */

Piece::Piece(const Problem& problem, Square square)
{
	assert(problem.initialPosition(square));

	/* -- Piece characteristics -- */

	_glyph = problem.initialPosition(square);
	_color = Euclide::color(_glyph);
	_species = problem.piece(_glyph);

	_royal = (_species == King);

	/* -- Initial square is known, final square not -- */

	_initialSquare = square;
	_castlingSquare = square;
	_finalSquare = Nowhere;

	/* -- Has the piece been captured or promoted? -- */

	_captured = (_royal || !problem.capturedPieces(_color)) ? tribool(false) : unknown;
	_promoted = (_species == Pawn) ? unknown : tribool(false);

	_glyphs.set(_glyph);
	if (maybe(_captured))
		_glyphs.set(Empty);
	if (maybe(_promoted))
		_glyphs.set([&](Glyph glyph) { return Euclide::color(glyph) == _color; });

	/* -- Initialize number of available moves and captures -- */

	_availableMoves = problem.moves(_color);
	_availableCaptures = problem.initialPieces(!_color) - problem.diagramPieces(!_color);

	_requiredMoves = 0;
	_requiredCaptures = 0;

	/* -- Initialize possible final squares and capture squares -- */

	for (Square square : AllSquares())
		_possibleSquares.set(square, maybe(_captured) || (problem.diagramPosition(square) == _glyph) || (maybe(_promoted) && (Euclide::color(problem.diagramPosition(square)) == _color)));

	if (_availableCaptures)
		_possibleCaptures.set();

	/* -- Initialize legal moves and move tables -- */

	Tables::initializeLegalMoves(&_moves, _species, _color, problem.variant(), _availableCaptures ? unknown : tribool(false));
	_xmoves = Tables::getCaptureMoves(_species, _color, problem.variant());

	_constraints = Tables::getMoveConstraints(_species, problem.variant(), false);
	_xconstraints = Tables::getMoveConstraints(_species, problem.variant(), true);

	_checks = Tables::getUnstoppableChecks(_species, _color, problem.variant());

	/* -- Initialize occupied squares -- */

	for (Square square : AllSquares())
		_occupied[square].pieces.fill(nullptr);

	/* -- Distances will be computed later -- */

	_distances.fill(0);
	_captures.fill(0);
	_rdistances.fill(0);
	_rcaptures.fill(0);

	/* -- Squares crossed will also be filled later -- */

	_stops.set();
	_route.set();

	/* -- Handle castling -- */

	std::fill_n(_castling, NumCastlingSides, false);

	if ((_glyph == WhiteKing) || (_glyph == BlackKing))
		for (CastlingSide side : AllCastlingSides())
			if (_initialSquare == Castlings[_color][side].from)
				if (problem.castling(_color, side))
					_moves[Castlings[_color][side].from][Castlings[_color][side].to] = true, _castling[side] = unknown;

	if ((_glyph == WhiteRook) || (_glyph == BlackRook))
		for (CastlingSide side : AllCastlingSides())
			if (_initialSquare == Castlings[_color][side].rook)
				if (problem.castling(_color, side))
					_castlingSquare = Castlings[_color][side].free, _castling[side] = unknown;

	/* -- Update possible moves -- */

	_update = true;
	update();
}

/* -------------------------------------------------------------------------- */

Piece::~Piece()
{
}

/* -------------------------------------------------------------------------- */

void Piece::setCastling(CastlingSide side, bool castling)
{
	if (!unknown(_castling[side]))
		return;

	/* -- King can only castle on one side -- */

	if (castling && _royal)
		for (CastlingSide other : AllCastlingSides())
			if (side != other)
				setCastling(other, false);

	/* -- Prohibit castling moves -- */

	if (!castling)
	{
		if (_royal)
			_moves[Castlings[_color][side].from][Castlings[_color][side].to] = false;

		_castlingSquare = _initialSquare;
	}

	/* -- Update state -- */

	_castling[side] = castling;
	_update = true;
}

/* -------------------------------------------------------------------------- */

void Piece::setCaptured(bool captured)
{
	if (!unknown(_captured))
		return;

	_captured = captured;
	_update = true;
}

/* -------------------------------------------------------------------------- */

void Piece::setAvailableMoves(int availableMoves)
{
	if (availableMoves >= _availableMoves)
		return;

	_availableMoves = availableMoves;
	_update = true;
}

/* -------------------------------------------------------------------------- */

void Piece::setAvailableCaptures(int availableCaptures)
{
	if (availableCaptures >= _availableCaptures)
		return;

	_availableCaptures = availableCaptures;
	_update = true;
}

/* -------------------------------------------------------------------------- */

void Piece::setPossibleSquares(const Squares& squares)
{
	if ((_possibleSquares & squares) == _possibleSquares)
		return;

	_possibleSquares &= squares;
	_update = true;
}

/* -------------------------------------------------------------------------- */

void Piece::setPossibleCaptures(const Squares& squares)
{
	if ((_possibleCaptures & squares) == _possibleCaptures)
		return;

	_possibleCaptures &= squares;
	_update = true;
}

/* -------------------------------------------------------------------------- */

void Piece::bypassObstacles(const Piece& blocker)
{
	const Squares& obstacles = blocker.stops();

	/* -- Blocked movements -- */

	if ((obstacles & _route).any())
		for (Square from : ValidSquares(_stops))
			for (Square to : ValidSquares(_moves[from]))
				if (obstacles <= ((*_constraints)[from][to] | from))
					_moves[from][to] = false, _update = true;

	/* -- Castling -- */

	if (_castlingSquare != _initialSquare)
		for (CastlingSide side : AllCastlingSides())
			if (obstacles <= (*_constraints)[Castlings[_color][side].rook][Castlings[_color][side].free])
				setCastling(side, false);

	/* -- Checks -- */

	if (_royal && (blocker._color != _color) && (obstacles.count() == 1))
		for (Square check : ValidSquares((*blocker._checks)[obstacles.first()]))
			if (_route[check])
				for (Square from : ValidSquares(_stops))
					if (_moves[from][check])
						_moves[from][check] = false, _update = true;
}

/* -------------------------------------------------------------------------- */

int Piece::mutualInteractions(Piece& pieceA, Piece& pieceB, const array<int, NumColors>& freeMoves, bool fast)
{
	const int requiredMoves = pieceA._requiredMoves + pieceB._requiredMoves;
	const bool enemies = pieceA._color != pieceB._color;

	/* -- Don't bother if these two pieces can not interact with each other -- */

	const Squares routes[2] = { 
		pieceA._route | ((enemies && pieceB._royal) ? pieceA._threats : Squares()),
		pieceB._route | ((enemies && pieceA._royal) ? pieceB._threats : Squares())
	};

	if (!(routes[0] & routes[1]))
		return requiredMoves;

	/* -- Use fast method if the search space is too large -- */

	const int threshold = 5000;
	if (pieceA.nmoves() * pieceB.nmoves() > threshold)
		fast = true;

	/* -- Play all possible moves with these two pieces -- */

	array<State, 2> states = {
		State(pieceA, pieceA._requiredMoves + freeMoves[pieceA._color]),
		State(pieceB, pieceB._requiredMoves + freeMoves[pieceB._color])
	};

	const int availableMoves = requiredMoves + freeMoves[pieceA._color] + (enemies ? freeMoves[pieceB._color] : 0);

	TwoPieceCache cache;
	const int newRequiredMoves = fast ? fastplay(states, availableMoves, cache) : fullplay(states, availableMoves, availableMoves, cache);

	if (newRequiredMoves >= Infinity)
		throw NoSolution;

	/* -- Store required moves for each piece, if greater than the previously computed values -- */

	for (const State& state : states)
		if (state.requiredMoves > state.piece._requiredMoves)
			state.piece._requiredMoves = state.requiredMoves, state.piece._update = true;

	/* -- Early exit if we have not performed all computations -- */

	if (fast)
		return newRequiredMoves;

	/* -- Remove never played moves and keep track of occupied squares -- */

	for (const State& state : states)
	{
		for (Square square : AllSquares())
		{
			if (state.moves[square] < state.piece._moves[square])
				state.piece._moves[square] = state.moves[square], state.piece._update = true;

			if (state.squares[square].count() == 1)
			{
				const Square occupied = state.squares[square].first();
				if (!state.piece._occupied[square].squares[occupied])
				{
					state.piece._occupied[square].squares[occupied] = true;
					state.piece._occupied[square].pieces[occupied] = &states[&state == &states[0]].piece;
					state.piece._update = true;
				}
			}

			if (state.distances[square] > state.piece._distances[square])
				state.piece._distances[square] = state.distances[square], state.piece._update = true;
		}
	}

	/* -- Done -- */

	return newRequiredMoves;
}

/* -------------------------------------------------------------------------- */

bool Piece::update()
{
	if (!_update)
		return false;

	updateDeductions();
	_update = false;

	return true;
}

/* -------------------------------------------------------------------------- */

void Piece::updateDeductions()
{
	/* -- Castling for rooks -- */

	if (_castlingSquare != _initialSquare)
		if (!_moves[_castlingSquare] && !_possibleSquares[_castlingSquare])
			for (CastlingSide side : AllCastlingSides())
				setCastling(side, false);

	if (_castlingSquare != _initialSquare)
		if (!_moves[_initialSquare] && !_possibleSquares[_initialSquare])
			for (CastlingSide side : AllCastlingSides())
				setCastling(side, true);

	if (_castlingSquare != _initialSquare)
		if (_distances[_castlingSquare])
			for (CastlingSide side : AllCastlingSides())
				setCastling(side, false);

	/* -- Compute distances -- */	

	const bool castling = xstd::any_of(AllCastlingSides(), [&](CastlingSide side) { return is(_castling[side]); });

	updateDistances(castling);
	if (_xmoves)
		_captures = computeCaptures(castling ? _castlingSquare : _initialSquare, _castlingSquare);

	for (Square square : ValidSquares(_possibleSquares))
		if (_distances[square] > _availableMoves)
			_possibleSquares[square] = false;

	for (Square square : ValidSquares(_possibleCaptures))
		if (_captures[square] > _availableCaptures)
			_possibleCaptures[square] = false;

	_rdistances = computeDistancesTo(_possibleSquares);
	if (_xmoves)
		_rcaptures = computeCapturesTo(_possibleSquares);

	/* -- Are there any possible final squares left? -- */

	if (!_possibleSquares)
		throw NoSolution;

	if (_possibleSquares.count() == 1)
		_finalSquare = _possibleSquares.first();

	/* -- Compute minimum number of moves and captures performed by this piece -- */

	xstd::maximize(_requiredMoves, xstd::min(ValidSquares(_possibleSquares), [&](Square square) { return _distances[square]; }));
	xstd::maximize(_requiredCaptures, xstd::min(ValidSquares(_possibleSquares), [&](Square square) { return _captures[square]; }));

	/* -- Remove moves that will obviously never be played -- */

	for (Square from : AllSquares())
		for (Square to : ValidSquares(_moves[from]))
			if (_distances[from] + 1 + _rdistances[to] > _availableMoves)
				_moves[from][to] = false;

	if (_xmoves)
		for (Square from : AllSquares())
			for (Square to : ValidSquares(_moves[from]))
				if (_captures[from] + (*_xmoves)[from][to] + _rcaptures[to] > _availableCaptures)
					_moves[from][to] = false;

	/* -- Update castling state according to corresponding king moves -- */

	if (_royal)
	{
		for (CastlingSide side : AllCastlingSides())
		{
			if (maybe(_castling[side]))
			{
				if (!_moves[Castlings[_color][side].from][Castlings[_color][side].to])
					setCastling(side, false);

				if (_moves[Castlings[_color][side].from].count() == 1)
					if (_moves[Castlings[_color][side].from][Castlings[_color][side].to])
						setCastling(side, true);
			}
		}
	}

	/* -- Get all squares the piece may have crossed or stopped on -- */

	_stops = _possibleSquares;
	_stops.set(_initialSquare);
	_stops.set(_castlingSquare);
	for (Square from : AllSquares())
		_stops |= _moves[from];

	_route = _stops;
	for (Square from : AllSquares())
		for (Square to : ValidSquares(_moves[from]))
			_route |= (*_constraints)[from][to];

	_threats.reset();
	for (Square square : ValidSquares(_stops))
		_threats |= (*_checks)[square];

	/* -- Update occupied squares -- */

	for (Square square : AllSquares())
	{
		for (bool loop = true; loop; )
		{
			loop = false;
			for (Square occupied : ValidSquares(_occupied[square].squares))
			{
				for (Square other : ValidSquares(_occupied[square].pieces[occupied]->_occupied[occupied].squares))
				{
					if (!_occupied[square].squares[other])
					{
						_occupied[square].pieces[other] = _occupied[square].pieces[occupied]->_occupied[occupied].pieces[other];
						_occupied[square].squares[other] = true;
						loop = true;
					}
				}
			}
		}
	}

}

/* -------------------------------------------------------------------------- */

void Piece::updateDistances(bool castling)
{
	const array<int, NumSquares> distances = computeDistances(castling ? _castlingSquare : _initialSquare, _castlingSquare);

	for (Square square : AllSquares())
		xstd::maximize(_distances[square], distances[square]);
}

/* -------------------------------------------------------------------------- */

array<int, NumSquares> Piece::computeDistances(Square square, Square castling) const
{
	/* -- Initialize distances -- */

	array<int, NumSquares> distances;
	distances.fill(Infinity);
	distances[square] = 0;
	distances[castling] = 0;
	
	/* -- Initialize square queue -- */

	Queue<Square, NumSquares> squares;
	squares.push(square);
	if (castling != square)
		squares.push(castling);
	
	/* -- Loop until every reachable square has been handled -- */

	while (!squares.empty())
	{
		const Square from = squares.front(); squares.pop();

		/* -- Handle every possible immediate destination -- */

		for (Square to : ValidSquares(_moves[from]))
		{
			/* -- This square may have been attained by a quicker path -- */

			if (distances[to] < Infinity)
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

array<int, NumSquares> Piece::computeDistancesTo(Squares destinations) const
{
	/* -- Initialize distances and square queue -- */

	array<int, NumSquares> distances;
	Queue<Square, NumSquares> squares;

	distances.fill(Infinity);

	for (Square square : AllSquares())
		if ((distances[square] = destinations[square] ? 0 : Infinity) == 0)
			squares.push(square);

	/* -- Loop until every reachable square has been handled -- */

	while (!squares.empty())
	{
		const Square to = squares.front(); squares.pop();

		/* -- Handle every possible immediate destination -- */

		for (Square from : AllSquares())
		{
			/* -- Skip illegal moves -- */

			if (!_moves[from][to])
				continue;

			/* -- This square may have been attained by a quicker path -- */

			if (distances[from] < Infinity)
				continue;

			/* -- Set square distance -- */

			distances[from] = distances[to] + 1;
	
			/* -- Add it to queue of reachable squares -- */

			squares.push(from);
		}
	}

	/* -- Done -- */

	return distances;
}

/* -------------------------------------------------------------------------- */

array<int, NumSquares> Piece::computeCaptures(Square square, Square castling) const
{
	assert(_xmoves);

	/* -- Initialize captures -- */

	array<int, NumSquares> captures;
	captures.fill(Infinity);
	captures[square] = 0;
	captures[castling] = 0;

	/* -- Initialize square queue -- */

	Queue<Square, NumSquares> squares;
	squares.push(square);
	if (castling != square)
		squares.push(castling);

	/* -- Loop until every reachable square has been handled -- */

	while (!squares.empty())
	{
		const Square from = squares.front(); squares.pop();

		/* -- Handle every possible immediate destination -- */

		for (Square to : ValidSquares(_moves[from]))
		{
			/* -- This square may have been attained using less captures -- */

			const int required = captures[from] + (*_xmoves)[from][to];
			if (required >= captures[to])
				continue;

			/* -- Set required captures -- */

			captures[to] = required;

			/* -- Add it to queue of squares -- */

			squares.push(to);
		}
	}

	/* -- Done -- */

	return captures;
}

/* -------------------------------------------------------------------------- */

array<int, NumSquares> Piece::computeCapturesTo(Squares destinations) const
{
	/* -- Initialize captures and square queue -- */

	array<int, NumSquares> captures;
	Queue<Square, NumSquares> squares;

	captures.fill(Infinity);

	for (Square square : AllSquares())
		if ((captures[square] = destinations[square] ? 0 : Infinity) == 0)
			squares.push(square);

	/* -- Loop until every reachable square has been handled -- */

	while (!squares.empty())
	{
		const Square to = squares.front(); squares.pop();

		/* -- Handle every possible immediate destination -- */

		for (Square from : AllSquares())
		{
			/* -- Skip illegal moves -- */

			if (!_moves[from][to])
				continue;

			/* -- This square may have been attained using less captures -- */

			const int required = captures[to] + (*_xmoves)[from][to];
			if (required >= captures[from])
				continue;

			/* -- Set square required number of captures -- */

			captures[from] = required;

			/* -- Add it to queue of reachable squares -- */

			squares.push(from);
		}
	}

	/* -- Done -- */

	return captures;
}

/* -------------------------------------------------------------------------- */

int Piece::fastplay(array<State, 2>& states, int availableMoves, TwoPieceCache& cache)
{
	typedef TwoPieceCache::Position Position;
	Queue<Position, 8 * NumSquares * NumSquares> queue;

	int requiredMoves = Infinity;

	const bool friends = (states[0].piece._color == states[1].piece._color);
	const bool partners = friends && (states[0].piece._royal || states[1].piece._royal) && (states[0].teleportation || states[1].teleportation);

	/* -- Initial position -- */

	Position initial(states[0].piece._initialSquare, 0, states[1].piece._initialSquare, 0);
	queue.push(initial);
	cache.add(initial);

	/* -- Loop -- */

	for ( ; !queue.empty(); queue.pop())
	{
		const Position& position = queue.front();

		/* -- Check if we have reached our goal -- */

		if (states[0].piece._possibleSquares[position.squares[0]] && states[1].piece._possibleSquares[position.squares[1]])
		{
			/* -- Get required moves -- */

			xstd::minimize(states[0].requiredMoves, position.moves[0]);
			xstd::minimize(states[1].requiredMoves, position.moves[1]);
			xstd::minimize(requiredMoves, position.moves[0] + position.moves[1]);
		}

		/* -- Play all moves -- */

		for (int s = 0; s < 2; s++)
		{
			const int k = s ^ (position.moves[0] > position.moves[1]);

			State& state = states[k];
			State& xstate = states[k ^ 1];
			Piece& piece = state.piece;
			Piece& xpiece = xstate.piece;
			const Square from = position.squares[k];
			const Square other = position.squares[k ^ 1];

			/* -- Handle teleportation for rooks -- */

			if (state.teleportation && !position.moves[k] && (position.squares[k] == piece._initialSquare))
			{
				Square to = piece._castlingSquare;

				/* -- Teleportation could be blocked by other piece -- */

				const bool blocked = (*piece._constraints)[from][to][other];
				if (!blocked)
				{
					Position next(k ? other : to, position.moves[0], k ? to : other, position.moves[1]);
					if (!cache.hit(next))
					{
						/* -- Insert resulting position in front of queue -- */

						queue.pass(next, 1);
						cache.add(next);
					}
				}
			}

			/* -- Check if there are any moves left for this piece -- */

			if (state.availableMoves <= position.moves[k])
				continue;

			if ((state.requiredMoves <= position.moves[k]) && (xstate.requiredMoves <= position.moves[k ^ 1]))
				continue;

			/* -- Check that the enemy is not in check -- */

			if (xpiece._royal && !friends && (*piece._checks)[from][other])
				continue;

			/* -- Loop over all moves -- */

			for (Square to : ValidSquares(piece._moves[from]))
			{
				Position next(k ? other : to, position.moves[0] + (k ^ 1), k ? to : other, position.moves[1] + (k ^ 0));

				/* -- Take castling into account -- */

				if (piece._royal && !position.moves[k] && partners)
					for (CastlingSide side : AllCastlingSides())
						if ((to == Castlings[piece._color][side].to) && (other == Castlings[piece._color][side].rook) && !position.moves[k ^ 1])
							next.squares[k ^ 1] = Castlings[piece._color][side].free;

				/* -- Continue if position was already reached before -- */

				if (cache.hit(next))
					continue;

				/* -- Move could be blocked by other pieces -- */

				bool blocked = (*piece._constraints)[from][to][other] || xpiece._occupied[other].squares[from];
				for (Square square : ValidSquares(xpiece._occupied[other].squares))
					if ((*piece._constraints)[from][to][square])
						blocked = true;

				if (blocked)
					continue;

				/* -- Reject move if it brings us to far away -- */

				if (piece._rdistances[to] > std::min(availableMoves, state.availableMoves - next.moves[k]))
					continue;

				/* -- Reject move if we move into check -- */

				if (piece._royal && !friends && (*xpiece._checks)[other][to])
					continue;

				/* -- Castling constraints -- */

				if (piece._royal && !friends && (from == piece._initialSquare))
					for (CastlingSide side : AllCastlingSides())
						if ((Castlings[piece._color][side].from == from) && (Castlings[piece._color][side].to == to))
							if (position.moves[k] || (*xpiece._checks)[other][from] || (*xpiece._checks)[other][Castlings[piece._color][side].free])
								continue;

				/* -- Safeguard if maximum queue size is insufficient -- */

				assert(!queue.full());
				if (queue.full())
				{
					states[0].requiredMoves = states[0].piece._requiredMoves;
					states[1].requiredMoves = states[1].piece._requiredMoves;
					return states[0].requiredMoves + states[1].requiredMoves;
				}

				/* -- Play move and add it to cache -- */

				queue.push(next);
				cache.add(next);
			}
		}
	}

	/* -- Done -- */

	return requiredMoves;
}

/* -------------------------------------------------------------------------- */

int Piece::fullplay(array<State, 2>& states, int availableMoves, int maximumMoves, TwoPieceCache& cache, bool *invalidate)
{
	int requiredMoves = Infinity;

	/* -- Check if we have achieved our goal -- */

	if (states[0].piece._possibleSquares[states[0].square] && states[1].piece._possibleSquares[states[1].square])
	{
		/* -- Get required moves -- */

		xstd::minimize(states[0].requiredMoves, states[0].playedMoves);
		xstd::minimize(states[1].requiredMoves, states[1].playedMoves);
		requiredMoves = states[0].playedMoves + states[1].playedMoves;

		/* -- Label occupied squares -- */

		states[0].squares[states[0].square][states[1].square] = true;
		states[1].squares[states[1].square][states[0].square] = true;
	}

	/* -- Break recursion if there are no more moves available -- */

	if (availableMoves < 0)
		return requiredMoves;

	/* -- Check for cache hit -- */

	if (cache.hit(states[0].square, states[0].playedMoves, states[1].square, states[1].playedMoves, &requiredMoves))
		return requiredMoves;

	/* -- Loop over both pieces -- */

	for (int k = 0; k < 2; k++)
	{
		const int s = k ^ (states[0].playedMoves > states[1].playedMoves);

		State& state = states[s];
		State& xstate = states[s ^ 1];
		Piece& piece = state.piece;
		Piece& xpiece = xstate.piece;
		const Square from = state.square;
		const Square other = xstate.square;
		const bool friends = (piece._color == xpiece._color);

		/* -- Teleportation when castling -- */

		if (state.teleportation && !state.playedMoves)
		{			
			const Square king = (xpiece._royal && friends) ? other : Nowhere;
			const Square pivot = std::find_if(Castlings[piece._color], Castlings[piece._color] + NumCastlingSides, [=](const Castling& castling) { return castling.rook == from; })->to;
		
			if ((king != Nowhere) ? (king == pivot) && (xstate.playedMoves == 1) : !(*piece._constraints)[piece._initialSquare][piece._castlingSquare][other])
			{
				assert(!piece._distances[piece._castlingSquare]);
				if (!piece._distances[piece._castlingSquare])
				{
					state.square = piece._castlingSquare;
					state.teleportation = false;

					const int myRequiredMoves = fullplay(states, availableMoves, maximumMoves, cache);
					if (myRequiredMoves <= maximumMoves)
					{
						state.squares[from][other] = true;
						xstate.squares[other][from] = true;

						state.distances[piece._castlingSquare] = 0;
					}

					xstd::minimize(requiredMoves, myRequiredMoves);

					state.teleportation = true;
					state.square = piece._initialSquare;
				}
			}
		}

		/* -- Check if there are any moves left for this piece -- */

		if (state.availableMoves <= 0)
			continue;

		/* -- Check that the enemy is not in check -- */

		if (xpiece._royal && !friends && (*piece._checks)[from][other])
			continue;

		/* -- Loop over all moves -- */

		for (Square to : ValidSquares(piece._moves[from]))
		{
			/* -- Move could be blocked by other pieces -- */

			bool blocked = (*piece._constraints)[from][to][other] || xpiece._occupied[other].squares[from];
			for (Square square : ValidSquares(xpiece._occupied[other].squares))
				if ((*piece._constraints)[from][to][square])
					blocked = true;

			if (blocked)
				continue;

			/* -- Reject move if it brings us to far away -- */

			if (1 + piece._rdistances[to] > std::min(availableMoves, state.availableMoves))
				continue;

			/* -- Reject move if we have taken a shortcut -- */

			if (state.playedMoves + 1 < piece._distances[to])
				if (!invalidate || ((*invalidate = true), true))
					continue;

			/* -- Reject move if we move into check -- */

			if (piece._royal && !friends && (*xpiece._checks)[other][to])
				continue;

			/* -- Castling constraints -- */

			if (piece._royal && !friends && (from == piece._initialSquare))
				for (CastlingSide side : AllCastlingSides())
					if ((Castlings[piece._color][side].from == from) && (Castlings[piece._color][side].to == to))
						if (state.playedMoves || (*xpiece._checks)[other][from] || (*xpiece._checks)[other][Castlings[piece._color][side].free])
							continue;

			/* -- Play move -- */

			state.availableMoves -= 1;
			state.playedMoves += 1;
			state.square = to;

			/* -- Recursion -- */

			bool shortcuts = false;
			const int myRequiredMoves = fullplay(states, availableMoves - 1, maximumMoves, cache, &shortcuts);

			/* -- Cache this position, for tremendous speedups -- */

			cache.add(states[0].square, states[0].playedMoves, states[1].square, states[1].playedMoves, myRequiredMoves, shortcuts);

			/* -- Label all valid moves that can be used to reach our goals and squares that were occupied -- */

			if (myRequiredMoves <= maximumMoves)
			{
				state.moves[from][to] = true;
				state.squares[from][other] = true;
				xstate.squares[other][from] = true;

				xstd::minimize(state.distances[to], state.playedMoves);
			}

			/* -- Undo move -- */

			state.availableMoves += 1;
			state.playedMoves -= 1;
			state.square = from;

			/* -- Update required moves -- */

			xstd::minimize(requiredMoves, myRequiredMoves);
		}
	}

	/* -- Done -- */

	return requiredMoves;
}

/* -------------------------------------------------------------------------- */


#if 0

/* -------------------------------------------------------------------------- */

Piece::Piece(Superman superman, Color color, int moves)
	: _superman(superman), _glyph(superman.glyph(color)), _color(color)
{
	assert(superman.isValid());
	assert(color.isValid());

	/* -- Initialize initial squares -- */

	_initial = superman.square(color);
	_xinitial = _initial;

	/* -- Handle castling -- */

	if (superman == KingRook)
		_xinitial = _initial.make(-2, 0);

	if (superman == QueenRook)
		_xinitial = _initial.make(3, 0);

	_kcastling = superman.isKing() ? &_movements[_initial][_initial.make(2, 0)] : NULL;
	_qcastling = superman.isKing() ? &_movements[_initial][_initial.make(-2, 0)] : NULL;

	/* -- Initialize movement tables -- */

	for (Square from = FirstSquare; from <= LastSquare; from++)
		for (Square to = FirstSquare; to <= LastSquare; to++)
			_movements[from][to].initialize(from, to, this, moves);

	for (Square from = FirstSquare; from <= LastSquare; from++)
		_imovements[from][0].initialize(from, UndefinedSquare, this, moves);

	for (Square to = FirstSquare; to <= LastSquare; to++)
		_imovements[to][1].initialize(UndefinedSquare, to, this, moves);

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

	_teleported = (_initial != _xinitial) ? indeterminate : (tribool)false;
	_captured = superman.isKing() ? (tribool)false : indeterminate;
	_promoted = superman.isPawn() ? indeterminate : (tribool)false;

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
	
	/* -- Initialize square queue -- */

	queue<Square> squares;
	squares.push(square);

	/* -- Handle castling -- */

	if (castling && (square == _initial))
	{
		/* -- Castling must be the king's first move -- */

		if (_superman.isKing() && (bool)_kcastling->mandatory())
		{
			squares.pop();

			distances[_kcastling->to()] = 1;
			squares.push(_kcastling->to());
		}

		if (_superman.isKing() && (bool)_qcastling->mandatory())
		{
			squares.pop();

			distances[_qcastling->to()] = 1;
			squares.push(_qcastling->to());
		}

		/* -- Castling must be the rook's first move -- */

		if (_teleported)
			squares.pop();

		/* -- Castling allows the rook to reach it's teleportation square freely -- */

		if (_teleported || indeterminate(_teleported))
		{
			assert(_xinitial != _initial);

			distances[_xinitial] = 0;
			squares.push(_xinitial);
		}

		/* -- If castling is mandatory, the initial rook square distance is non zero -- */

		if (_teleported)
			distances[_initial] = infinity;
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

	if (!indeterminate(_teleported))
	{
		/* -- Take castling into account -- */

		Square current = _xinitial;

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

			/* -- Label incomplete move as mandatory -- */

			if (possibilities > 1)
				_imovements[current][0].validate();

			/* -- If there is more than one possibility, stop -- */

			if (possibilities != 1)
				break;

			/* -- Label current movement as mandatory and move on -- */
	
			_movements[current][destination].validate();
			_imovements[destination][1].validate();

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

		while (_distances[current] > 0)
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

			/* -- Label incomplete move as mandatory -- */

			if (possibilities > 1)
				_imovements[current][1].validate();

			/* -- If there is more than one possibility, stop -- */

			if (possibilities != 1)
				break;

			/* -- Label current movement as mandatory and move backward -- */

			_movements[source][current].validate();
			_movements[source][0].validate();

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

int Piece::getMandatoryMoves(Moves& moves, bool incomplete) /*const*/
{
	/* -- Find initial square if known -- */

	Square square = indeterminate(_teleported) ? Square(UndefinedSquare) : _xinitial;

	/* -- Initialize output -- */

	moves.clear();

	/* -- Scan list of moves to find mandatory ones -- */

	for (Moves::const_iterator move = _moves.begin(); move != _moves.end(); move++)
	{
		if (move->mandatory())
		{
			/* -- Add incomplete moves if required -- */

			if (incomplete && (move->from() != square))
			{
				if (square != UndefinedSquare)
					if (_imovements[square][0].mandatory())
						moves.push_back(&_imovements[square][0]);
	
				if (_imovements[move->from()][1].mandatory())
					moves.push_back(&_imovements[move->from()][1]);
			}
			
			/* -- Add mandatory move, if there is no possible switchbacks -- */

			bool switchbacks = false;

			for (Square to = FirstSquare; to <= LastSquare; to++)
				if ((to != move->to()) && _movements[move->from()][to].possible())
					switchbacks = true;

			for (Square from = FirstSquare; from <= LastSquare; from++)
				if ((from != move->from()) && _movements[from][move->to()].possible())
					switchbacks = true;

			if (!switchbacks)
			{
				moves.push_back(*move);			
				square = move->to();
			}
		}
	}

	/* -- Add final incomplete moves if required -- */

	if (incomplete && (square != UndefinedSquare))
	{	
		if (!_possibleSquares[square])
		{
			if (_imovements[square][0].mandatory())
				moves.push_back(&_imovements[square][0]);

			if (_possibleSquares.count() == 1)
			{
				Square square = FirstSquare;
				while (!_possibleSquares[square])
					square++;

				if (_imovements[square][1].mandatory())
					moves.push_back(&_imovements[square][1]);
			}
		}
	}

	/* -- Save constraints associated to these mandatory moves -- */

	if (!moves.empty())
	{
		for (Moves::iterator move = moves.begin(); move + 1 != moves.end(); move++)
		{
			Move *first = *(move + 0);
			Move *second = *(move + 1);

			if (!first->incomplete() || !second->incomplete())
			{
				first->constraints()->mustPreceed(this, second);
				second->constraints()->mustFollow(this, first);
			}
		}
	}

	/* -- Return number of moves found -- */

	return moves.size();
}

/* -------------------------------------------------------------------------- */

int Piece::getMovesFrom(Square square, Moves& moves) const
{
	/* -- Initialize output -- */

	moves.clear();

	/* -- Scan moves to find moves leaving the given square -- */

	for (Square to = FirstSquare; to <= LastSquare; to++)
		if (_movements[square][to].possible())
			moves.push_back(const_cast<Move *>(&_movements[square][to]));

	/* -- Return number of moves found -- */

	return moves.size();
}

/* -------------------------------------------------------------------------- */

int Piece::getMovesTo(Square square, Moves& moves) const
{
	/* -- Initialize output -- */

	moves.clear();

	/* -- Scan moves to find moves reaching the given square -- */

	for (Square from = FirstSquare; from <= LastSquare; from++)
		if (_movements[from][square].possible())
			moves.push_back(const_cast<Move *>(&_movements[from][square]));

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

		/* -- Remove impossible optional moves -- */

		for (Moves::iterator move = _moves.begin(); move != _moves.end(); move++)
			if (!_imovements[move->from()][0].possible() || !_imovements[move->to()][1].possible())
				move->invalidate();

		/* -- Remove optional moves that are out of bounds -- */

		for (Moves::iterator move = _moves.begin(); move != _moves.end(); move++)
			move->bound(_imovements[move->from()][0].earliest(), move->latest());

		for (Moves::iterator move = _moves.begin(); move != _moves.end(); move++)
			move->bound(move->earliest(), _imovements[move->to()][1].latest());

		/* -- Keep only possible moves -- */

		_moves.erase(std::remove_if(_moves.begin(), _moves.end(), isMoveImpossible), _moves.end());
	}

	/* -- Update incomplete moves -- */

	for (Square from = FirstSquare; from <= LastSquare; from++)
	{
		if (_imovements[from][0].possible())
		{
			int earliest = infinity;
			int latest = 0;

			for (Square to = FirstSquare; to <= LastSquare; to++)
			{
				if (_movements[from][to].possible())
				{
					minimize(earliest, _movements[from][to].earliest());
					maximize(latest, _movements[from][to].latest());
				}
			}

			_imovements[from][0].bound(earliest, latest);
		}
	}

	for (Square to = FirstSquare; to <= LastSquare; to++)
	{
		if (_imovements[to][1].possible())
		{
			int earliest = infinity;
			int latest = 0;

			for (Square from = FirstSquare; from <= LastSquare; from++)
			{
				if (_movements[from][to].possible())
				{
					minimize(earliest, _movements[from][to].earliest());
					maximize(latest, _movements[from][to].latest());
				}
			}

			_imovements[to][1].bound(earliest, latest);
		}
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

	std::sort(_moves.begin(), _moves.end(), [=](const Move *moveA, const Move *moveB) { return isMoveEarlier(moveA, moveB, *this); });

	/* -- All done -- */

	_optimized = true;
}

/* -------------------------------------------------------------------------- */

bool Piece::constrain()
{
	bool modified = false;
	if (!moves())
		return false;

	/* -- Propagate incomplete move constraints to complete moves -- */

	std::pair<Move *, int> candidates[NumSquares][2];
	for (Square square = FirstSquare; square <= LastSquare; square++)
		candidates[square][0].second = candidates[square][1].second = 0;

	for (Moves::iterator move = _moves.begin(); move != _moves.end(); move++)
	{
		candidates[move->from()][0].first = *move;
		candidates[move->from()][0].second += 1;
		candidates[move->to()][1].first = *move;
		candidates[move->to()][1].second += 1;
	}

	for (Square square = FirstSquare; square <= LastSquare; square++)
	{
		for (int direction = 0; direction < 2; direction++)
		{
			Move *move = candidates[square][direction].first;
			if (candidates[square][direction].second != 1)
				continue;

			if (!move->mandatory())
				continue;

			if (!_imovements[square][direction].constraints())
				continue;

			for (Man man = FirstMan; man <= LastMan; man++)
			{
				for (Color color = FirstColor; color <= LastColor; color++)
				{
					if (_imovements[square][direction].constraints()->follows(color, man))
						if (!move->constraints()->follows(color, man) || move->constraints()->follows(color, man))
							move->constraints()->mustFollow(_imovements[square][direction].constraints()->follows(color, man)->piece(), _imovements[square][direction].constraints()->follows(color, man));

					if (_imovements[square][direction].constraints()->precedes(color, man))
						if (!move->constraints()->precedes(color, man) || move->constraints()->precedes(color, man))
							move->constraints()->mustPreceed(_imovements[square][direction].constraints()->precedes(color, man)->piece(), _imovements[square][direction].constraints()->precedes(color, man));
				}
			}
		}
	}

	/* -- Apply constraints to every move -- */

	for (Moves::iterator move = _moves.begin(); move != _moves.end(); move++)
		if (move->constrain())
			modified = true;

	/* -- Apply contraints to partial moves also -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
		for (int direction = 0; direction < 2; direction++)
			if (_imovements[square][direction].possible())
				if (_imovements[square][direction].constrain())
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

void Piece::setMandatoryMoveConstraints(const Piece& piece, const Moves& moves)
{
	/* -- Don't mess with ourself -- */

	if (&piece == this)
		return;

	/* -- Don't bother if there is no mandatory moves or the piece has not moved -- */

	if (moves.empty() || _moves.empty())
		return;

	/* -- The first rook move after castling occured after castling took place -- */

	if (_teleported && piece.glyph().isKing() && (piece.color() == _color))
	{
		for (Moves::iterator move = _moves.begin(); move != _moves.end(); move++)
			if (move->possible() && (move->from() == _xinitial))
				move->constraints()->mustFollow(&piece, moves[0]);

		if (_imovements[_xinitial][0].possible())
			_imovements[_xinitial][0].constraints()->mustFollow(&piece, moves[0]);
	}

	/* -- Let's put aside castling for now and start from initial square -- */

	if (!_teleported && piece.alive(false))
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

			/* -- If there is no more obstructions, there is nothing more to do -- */

			if (!obstructions.obstructions())
				break;

			/* -- These obstructions are the moves that are constrained -- */

			for (int move = 0; move < obstructions.obstructions(); move++)
				obstructions[move]->constraints()->mustFollow(constraint->piece(), *constraint);

			/* -- Constrain also generic moves -- */

			int movesFrom[NumSquares];
			int movesTo[NumSquares];

			std::fill(movesFrom, movesFrom + NumSquares, 0);
			std::fill(movesTo, movesTo + NumSquares, 0);

			for (Moves::const_iterator move = _moves.begin(); move != _moves.end(); move++)
			{
				if (move->possible())
				{
					movesFrom[move->from()] += 1;
					movesTo[move->to()] += 1;
				}
			}

			for (int move = 0; move < obstructions.obstructions(); move++)
			{
				if (obstructions[move]->possible())
				{
					movesFrom[obstructions[move]->from()] -= 1;
					movesTo[obstructions[move]->to()] -= 1;
				}
			}

			for (Square from = FirstSquare; from <= LastSquare; from++)
				if (_imovements[from][0].possible() && !movesFrom[from])
					_imovements[from][0].constraints()->mustFollow(constraint->piece(), *constraint);

			for (Square to = FirstSquare; to <= LastSquare; to++)
				if (_imovements[to][1].possible() && !movesTo[to])
					_imovements[to][1].constraints()->mustFollow(constraint->piece(), *constraint);

			/* -- Update current square -- */

			square = constraint->to();

			/* -- Break on incomplete move -- */

			if (square == UndefinedSquare)
				break;
		}
	}

	/* -- Let's do the same starting from the end -- */

	if (piece.alive() && (piece._possibleSquares.count() == 1))
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

			int movesFrom[NumSquares];
			int movesTo[NumSquares];

			std::fill(movesFrom, movesFrom + NumSquares, 0);
			std::fill(movesTo, movesTo + NumSquares, 0);

			for (Moves::const_iterator move = _moves.begin(); move != _moves.end(); move++)
			{
				if (move->possible())
				{
					movesFrom[move->from()] += 1;
					movesTo[move->to()] += 1;
				}
			}

			for (int move = 0; move < obstructions.obstructions(); move++)
			{
				if (obstructions[move]->possible())
				{
					movesFrom[obstructions[move]->from()] -= 1;
					movesTo[obstructions[move]->to()] -= 1;
				}
			}

			for (Square from = FirstSquare; from <= LastSquare; from++)
				if (_imovements[from][0].possible() && !movesFrom[from])
					_imovements[from][0].constraints()->mustPreceed((*constraint)->piece(), *constraint);

			for (Square to = FirstSquare; to <= LastSquare; to++)
				if (_imovements[to][1].possible() && !movesTo[to])
					_imovements[to][1].constraints()->mustPreceed((*constraint)->piece(), *constraint);

			square = (*constraint)->from();

			if (square == UndefinedSquare)
				break;
		}
	}
}

/* -------------------------------------------------------------------------- */

int Piece::findMutualObstructions(Piece *pieces[2], Square squares[2], int nmoves[2], Moves& moves, MiniHash& processed, int availableMoves, int assignedMoves, int maximumMoves, int rmoves[2])
{
	int requiredMoves = infinity;

	/* -- Check if we have achieved our goal -- */

	if (pieces[0]->_possibleSquares[squares[0]] && pieces[1]->_possibleSquares[squares[1]])
	{
		requiredMoves = moves.size();
		minimize(rmoves, nmoves, 2);
	}

	/* -- Return if performing more computations is useless -- */

	if (requiredMoves <= assignedMoves)
		return requiredMoves;

	/* -- Return if there is no more available moves -- */

	if (availableMoves <= 0)
		return requiredMoves;

	/* -- If we do not know the result, we must perform calculations -- */

	if (!processed[squares[0]][squares[1]].visited(nmoves, &requiredMoves))
	{
		/* -- Try all moves for both pieces -- */

		for (int k = 0; k < 2; k++)
		{
			/* -- Try teleportation (castling) if we are a rook -- */

			if (!nmoves[k] && (pieces[k]->_teleported || indeterminate(pieces[k]->_teleported)) && (squares[k] == pieces[k]->_initial))
			{
				bool possible = true;

				/* -- Check if intermediate squares are free -- */

				int delta = (pieces[k]->_initial < pieces[k]->_xinitial) ? 1 : -1;
				for (Column column = (column_t)(pieces[k]->_initial.column() + delta); column != E; column += delta)
					if ((pieces[k ^ 1]->man() != King) || (column != E - 2 * delta))
						if (squares[k ^ 1] == Square(column, pieces[k]->_initial.row()))
							possible = false;

				/* -- Teleportation -- */

				if (possible)
				{
					squares[k] = pieces[k]->_xinitial;
					minimize(requiredMoves, findMutualObstructions(pieces, squares, nmoves, moves, processed, availableMoves, assignedMoves, maximumMoves, rmoves));
					squares[k] = pieces[k]->_initial;
				}
			}

			/* -- Check that we can still play -- */

			if (nmoves[k] >= pieces[k]->_availableMoves)
				continue;

			/* -- Try all moves, unless castling is mandatory -- */

			if ((nmoves[k] > 0) || !pieces[k]->_teleported || indeterminate(pieces[k]->_teleported) || (squares[k] != pieces[k]->_initial))
			{
				for (Moves::iterator move = pieces[k]->_moves.begin(); move != pieces[k]->_moves.end(); move++)
				{
					/* -- Check if we can play this move -- */

					if (move->from() != squares[k])
						continue;

					if (move->isObstruction(squares[k ^ 1]))
						continue;

					if (move->to() == squares[k ^ 1])
						continue;

					/* -- Check ordering constraints -- */

					if (move->constraints() && move->constraints()->follows(pieces[k ^ 1]->color(), pieces[k ^ 1]->man()))
						if (!move->constraints()->follows(pieces[k ^ 1]->color(), pieces[k ^ 1]->man())->tags())
							continue;

					/* -- Check for illegal moves if we are a king -- */

					if ((pieces[k]->man() == King) && (pieces[0]->color() != pieces[1]->color()))
						if (tables::checks[move->to()][pieces[k ^ 1]->glyph()][squares[k ^ 1]])
							continue;

					if ((pieces[k ^ 1]->man() == King) && (pieces[0]->color() != pieces[1]->color()))
						if (tables::checks[squares[k ^ 1]][pieces[k]->glyph()][move->from()])
							continue;

					/* -- Check that playing this move makes sense -- */

					if (pieces[k]->_rdistances[move->to()] > pieces[k]->_availableMoves - nmoves[k] - 1)
						continue;

					/* -- Play the move -- */

					squares[k] = move->to();
					moves.push_back(*move);
					nmoves[k] += 1;
					
					pieces[k]->move(move->from(), UndefinedSquare)->tags() += 1;
					pieces[k]->move(UndefinedSquare, move->to())->tags() += 1;
					move->tags() += 1;

					/* -- Recursive call -- */

					int result = findMutualObstructions(pieces, squares, nmoves, moves, processed, availableMoves - 1, assignedMoves, maximumMoves, rmoves);
					
					/* -- Update hash table and tag successful moves -- */

					processed[squares[0]][squares[1]].visited(nmoves, result);
					if (result <= maximumMoves)
					{
						if (move->tag().used)
						{
							minimize(move->tag().min, nmoves[0] + nmoves[1] - 1);
							maximize(move->tag().max, nmoves[0] + nmoves[1] - 1);
						}
						else
						{
							move->tag().min = nmoves[0] + nmoves[1] - 1;
							move->tag().max = nmoves[0] + nmoves[1] - 1;
							move->tag().used = true;
						}
					}

					/* -- Update number of required moves -- */

					minimize(requiredMoves, result);

					/* -- Unplay the move -- */

					pieces[k]->move(move->from(), UndefinedSquare)->tags() -= 1;
					pieces[k]->move(UndefinedSquare, move->to())->tags() -= 1;
					move->tags() -= 1;

					nmoves[k] -= 1;
					moves.pop_back();
					squares[k] = move->from();

					/* -- Stop if performing more computations is useless -- */

					if (requiredMoves <= assignedMoves)
						return requiredMoves;
				}
			}
		}
	}

	/* -- Return required moves -- */

	return requiredMoves;
}

/* -------------------------------------------------------------------------- */

bool Piece::setMutualObstructions(Piece& piece, int availableMoves, int assignedMoves, int *requiredMoves, int *requiredMovesA, int *requiredMovesB, bool fast)
{
	if (requiredMoves)
		*requiredMoves = 0;

	if (requiredMovesA)
		*requiredMovesA = 0;

	if (requiredMovesB)
		*requiredMovesB = 0;

	if (&piece == this)
		return false;

	/* -- Skip useless computations -- */

	if (!moves() || !piece.moves())
		return false;

	/* -- This function does not implement captures yet -- */

	if (_captured || indeterminate(_captured) || piece._captured || indeterminate(piece._captured))
		return false;

	/* -- Initialization -- */

	Piece *pieces[2] = { this, &piece };

	Square squares[2] = { pieces[0]->_initial, pieces[1]->_initial };
	int rmoves[2] = { infinity, infinity };
	int nmoves[2] = { 0, 0 };
	Moves moves;

	/* -- Initialize hash table of positions already processed -- */

	MiniHash processed;

	/* -- Tag all moves as unused and unplayed -- */

	for (int k = 0; k < 2; k++)
	{
		for (Moves::const_iterator move = pieces[k]->_moves.begin(); move != pieces[k]->_moves.end(); move++)
		{
			move->tag().used = false;
			move->tags() = 0;
		}

		for (Square square = FirstSquare; square <= LastSquare; square++)
		{
			pieces[k]->move(square, UndefinedSquare)->tags() = 0;
			pieces[k]->move(UndefinedSquare, square)->tags() = 0;
		}
	}

	/* -- Recursively find the number of required moves for these two pieces -- */

	int minimumRequiredMoves = findMutualObstructions(pieces, squares, nmoves, moves, processed, availableMoves, fast ? assignedMoves : -1, availableMoves, rmoves);

	/* -- Save result -- */

	if (requiredMoves)
		*requiredMoves = minimumRequiredMoves;

	if (requiredMovesA && !fast)
		*requiredMovesA = rmoves[0];

	if (requiredMovesB && !fast)
		*requiredMovesB = rmoves[1];

	/* -- Mark as impossible all moves that have not been played -- */

	bool modified = false;

	if (!fast)
	{
		for (int k = 0; k < 2; k++)
		{
			for (Moves::iterator move = pieces[k]->_moves.begin(); move != pieces[k]->_moves.end(); move++)
			{
				if (!move->tag().used)
				{
					pieces[k]->_optimized = false;
					move->invalidate();
					modified = true;
				}
			}
		}
	}

	/* -- Set constraints on mandatory moves -- */

	Moves mandatoryMoves[2];

	for (int k = 0; k < 2; k++)
		for (Moves::iterator move = pieces[k]->_moves.begin(); move != pieces[k]->_moves.end(); move++)
			if (move->mandatory() && move->tag().used)
				mandatoryMoves[k].push_back(*move);

	if (mandatoryMoves[0].size() && mandatoryMoves[1].size())
	{
		for (int k = 0; k < 2; k++)
		{
			for (Moves::iterator move = pieces[k]->_moves.begin(); move != pieces[k]->_moves.end(); move++)
			{
				Move *predecessor = NULL;
				Move *successor = NULL;

				for (Moves::iterator xmove = mandatoryMoves[k ^ 1].begin(); xmove != mandatoryMoves[k ^ 1].end(); xmove++)
				{
					if (xmove->tag().max < move->tag().min)
						if (!predecessor || predecessor->tag().max < xmove->tag().max)
							predecessor = *xmove;
					if (xmove->tag().min > move->tag().max)
						if (!successor || successor->tag().min > xmove->tag().min)
							successor = *xmove;
				}

				if (predecessor)
					move->constraints()->mustFollow(predecessor->piece(), predecessor);
				if (successor)
					move->constraints()->mustPreceed(successor->piece(), successor);
			}
		}
	}

	/* -- Return whether we have made some deductions or not -- */

	return modified;
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
	/* -- Try if castling is required or impossible -- */

	if (indeterminate(_teleported))
	{
		/* -- Compute distances assuming castling state -- */

		int distances[NumSquares];
		int xdistances[NumSquares];

		int distance = infinity;
		int xdistance = infinity;

		computeForwardDistances(_initial, distances, false);
		for (Square square = FirstSquare; square <= LastSquare; square++)
			if (_possibleSquares[square])
				minimize(distance, distances[square]);

		computeForwardDistances(_xinitial, xdistances, false);
		for (Square square = FirstSquare; square <= LastSquare; square++)
			if (_possibleSquares[square])
				minimize(xdistance, xdistances[square]);

		/* -- Eliminate impossible castlings -- */

		if (distance > _availableMoves)
			_teleported = true;

		if (xdistance > _availableMoves)
			_teleported = false;

		if (!_teleported)
			_xinitial = _initial;
	}
}

/* -------------------------------------------------------------------------- */

void Piece::synchronizeCastling(Piece& krook, Piece& qrook)
{
	assert(_superman.isKing());

	/* -- Synchronize castling deductions between king and rooks -- */

	if (_kcastling->mandatory() && indeterminate(krook._teleported))
	{
		krook._teleported = true;
		krook._optimized = false;
	}

	if (!_kcastling->possible() && indeterminate(krook._teleported))
	{
		krook._xinitial = krook._initial;
		krook._teleported = false;
		krook._optimized = false;
	}

	if (_qcastling->mandatory() && indeterminate(qrook._teleported))
	{
		qrook._teleported = true;
		qrook._optimized = false;
	}

	if (!_qcastling->possible() && indeterminate(qrook._teleported))
	{
		qrook._xinitial = qrook._initial;
		qrook._teleported = false;
		qrook._optimized = false;
	}
	
	if (krook._teleported && !_kcastling->mandatory())
	{
		_kcastling->validate();
		_optimized = false;
	}

	if (!krook._teleported && _kcastling->possible())
	{
		_kcastling->invalidate();
		_optimized = false;
	}

	if (qrook._teleported && !_qcastling->mandatory())
	{
		_qcastling->validate();
		_optimized = false;
	}

	if (!qrook._teleported && _qcastling->possible())
	{
		_qcastling->invalidate();
		_optimized = false;
	}

	/* -- Basic coherency checks -- */

	if (_kcastling->mandatory() && !krook._teleported)
		abort(NoSolution);

	if (_qcastling->mandatory() && !qrook._teleported)
		abort(NoSolution);

	if (krook._teleported && !_kcastling->possible())
		abort(NoSolution);

	if (qrook._teleported && !_qcastling->possible())
		abort(NoSolution);
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

#endif

}
