#include "game.h"
#include "pieces.h"
#include "problem.h"
#include "tables/tables.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */
/* -- Game                                                                 -- */
/* -------------------------------------------------------------------------- */

Game::Game(const EUCLIDE_Configuration& configuration, const EUCLIDE_Callbacks& callbacks, const Problem& problem, const array<Pieces, NumColors>& pieces)
	: _configuration(configuration), _callbacks(callbacks), _problem(problem), _pieces(pieces)
{
	for (Glyph glyph : AllGlyphs())
		Tables::initializeLegalMoves(&_captures[glyph], problem.piece(glyph), color(glyph), problem.variant(), true);

	for (Glyph glyph : AllGlyphs())
		_constraints[glyph] = *Tables::getMoveConstraints(problem.piece(glyph), problem.variant(), true, false);
	
	Tables::initializeLineOfSights(problem.pieces(), problem.variant(), &_lines);

	_board.fill(nullptr);
	for (Color color : AllColors())
		for (const Piece& piece : pieces[color])
			_board[piece.square(true)] = &piece;

	for (Color color : AllColors())
		_position[color].set([&](Square square) { return Euclide::color(problem.initialPosition(square)) == color; });

	for (Color color : AllColors())
		_kings[color] = pieces[color][0].square(true);

	_diagram.set([&](Square square) { return problem.diagramPosition(square) != Empty; });

	_positions = 0;
	_solutions = 0;
}

/* -------------------------------------------------------------------------- */

Game::~Game()
{
}

/* -------------------------------------------------------------------------- */

void Game::play()
{
	/* -- Recursively play all moves from initial state -- */

	const State state(_problem);
	play(state);

	/* -- Done -- */

	EUCLIDE_Thinking thinking;
	memset(&thinking, 0, sizeof(thinking));
	thinking.positions = _positions;

	if (_callbacks.displayThinking)
		(*_callbacks.displayThinking)(_callbacks.handle, &thinking);
}

/* -------------------------------------------------------------------------- */

void Game::play(const State& _state)
{
	_positions += 1;

	/* -- Thinking callback -- */

	if (_states.size() == std::extent<decltype(EUCLIDE_Thinking::moves)>::value)
	{
		EUCLIDE_Thinking thinking;
		thinking.positions = _positions;
		cmoves(thinking.moves, _states.size());

		if (_callbacks.displayThinking)
			(*_callbacks.displayThinking)(_callbacks.handle, &thinking);
	}

	/* -- End recursion -- */

	if (_states.size() >= _problem.moves())
	{
		/* -- Check for solution -- */

		if (solved())
		{
			EUCLIDE_Solution solution;
			solution.numHalfMoves = _states.size();
			cmoves(solution.moves, _states.size());
			solution.solution = ++_solutions;

			if (_callbacks.displaySolution)
				(*_callbacks.displaySolution)(_callbacks.handle, &solution);
		}

		/* -- End recursion -- */

		return;
	}

	/* -- Get legal moves -- */

	const Squares position = _position[White] | _position[Black];
	const Color color = _state.color();

	for (Square from : ValidSquares(_position[color]))
	{
		const Piece& piece = *_board[from];

		for (Square to : ValidSquares(piece.moves(from)))
		{
			/* -- Check if capture is ok -- */

			const bool capture = _position[!color][to];
			if (capture && !is(_board[to]->captured()))
				continue;

			/* -- Check if move is possible given other pieces on the board -- */

			if (position & piece.constraints(from, to, capture))
				continue;

			/* -- Check if the king is not in check -- */

			if (false)
				continue;

			/* -- Additional checks for castling -- */

			CastlingSide castling = NoCastling;
			if (piece.royal())
				for (CastlingSide side : AllCastlingSides())
					if (_state.castling(side) && (from == Castlings[color][side].from) && (to == Castlings[color][side].to))
						castling = side;

			if (castling != NoCastling)
				if (_state.check() || checked(Castlings[color][castling].free, color))
					continue;

			/* -- Perform move and compute new game state -- */

			State state = move(_state, from, to, castling);
			_states.push_back(&state);

			/* -- Move is valid only if the king is not in check -- */

			bool valid = true;
			if (piece.royal() || _state.check())
				valid &= !checked(_kings[color], color);
			else
				valid &= !checked(_kings[color], from, color);

			if (valid)
			{
				/* -- Set check state -- */

				if (checks(piece.glyph(true), to, _kings[!color]))
					state.check(true);

				if (checked(_kings[!color], from, !color))
					state.check(true);

				if (castling != NoCastling)
					if (checks(_board[Castlings[color][castling].free]->glyph(true), Castlings[color][castling].free, _kings[!color]))
						state.check(true);

				/* -- Recursive call -- */

				play(state);
			}

			/* -- Undo move -- */

			_states.pop_back();
			undo(state);
		}
	}

	/* -- Done -- */
}

/* -------------------------------------------------------------------------- */

Game::State Game::move(const State& state, Square from, Square to, CastlingSide castling)
{
	const Color color = state.color();

	/* -- Update board position -- */

	const Piece *captured = _board[to];

	_board[to] = _board[from];
	_board[from] = nullptr;

	assert(_position[color][from]);
	assert(!_position[color][to]);

	_position[color][to] = true;
	_position[!color][to] = false;
	_position[color][from] = false;

	if (_kings[color] == from)
		_kings[color] = to;

	/* -- Handle castling -- */

	if (castling != NoCastling)
	{
		const Square rook = Castlings[color][castling].rook;
		const Square free = Castlings[color][castling].free;

		_board[free] = _board[rook];
		_board[rook] = nullptr;

		assert(_position[color][rook]);
		assert(!_position[color][free]);

		_position[color][free] = true;
		_position[color][rook] = false;

		assert(!captured);
	}

	/* -- Update castlings states -- */

	array<bool, NumCastlingSides> castlings;
	for (CastlingSide side : AllCastlingSides())
		castlings[side] = (_kings[color] != to) && (_board[to]->square(true) != Castlings[color][side].rook);

	/* -- Return new state -- */

	return State(state, from, to, captured, castling, castlings);
}

/* -------------------------------------------------------------------------- */

void Game::undo(const State& state)
{
	const Color color = !state.color();
	const Square from = state.from();
	const Square to = state.to();

	/* -- Update board position -- */

	_board[from] = _board[to];
	_board[to] = state.captured();

	_position[color][to] = false;
	_position[color][from] = true;
	if (state.captured())
		_position[!color][to] = true;

	if (_kings[color] == to)
		_kings[color] = from;

	/* -- Handle castling -- */

	if (state.castling() != NoCastling)
	{
		const Square rook = Castlings[color][state.castling()].rook;
		const Square free = Castlings[color][state.castling()].free;

		_board[rook] = _board[free];
		_board[free] = nullptr;

		_position[color][rook] = true;
		_position[color][free] = false;
	}
}

/* -------------------------------------------------------------------------- */

bool Game::checks(Glyph glyph, Square from, Square king) const
{
	const Squares blockers = _position[White] | _position[Black];

	if (_captures[glyph][from][king])
		if (!(_constraints[glyph][from][king] & blockers))
			return true;

	return false;
}

/* -------------------------------------------------------------------------- */

bool Game::checked(Square king, Color color) const
{
	return checked(king, king, color);
}

/* -------------------------------------------------------------------------- */

bool Game::checked(Square king, Square free, Color color) const
{
	const Squares enemies = _lines[color][king][free] & _position[!color];

	for (Square from : ValidSquares(enemies))
		if (checks(_board[from]->glyph(true), from, king))
			return true;

	return false;
}

/* -------------------------------------------------------------------------- */

bool Game::solved() const
{
	/* -- Check number of moves -- */

	assert(_states.size() == _problem.moves());

	/* -- Quick check for diagram layout -- */

	if ((_position[White] | _position[Black]) != _diagram)
		return false;

	/* -- Check all pieces -- */

	for (Square square : ValidSquares(_diagram))
		if (_board[square]->glyph() != _problem.diagramPosition(square))
			return false;

	/* -- Solution found -- */

	return true;
}

/* -------------------------------------------------------------------------- */

void Game::cmoves(EUCLIDE_Move *moves, int nmoves) const
{
	array<Glyph, NumSquares> diagram = _problem.initialPosition();

	for (int m = 0; m < nmoves; m++)
	{
		const State& state = *_states[m];
		EUCLIDE_Move *move = moves + m;

		move->glyph = static_cast<EUCLIDE_Glyph>(diagram[state.from()]);
		move->promotion = static_cast<EUCLIDE_Glyph>(diagram[state.from()]);
		move->captured = static_cast<EUCLIDE_Glyph>(diagram[state.to()]);

		move->move = (m + ((state.color() == Black) ? 3 : 2)) / 2;
		move->from = state.from();
		move->to = state.to();

		move->capture = state.captured() != nullptr;
		move->enpassant = state.captured() && !diagram[state.to()];
		move->check = state.check();
		move->mate = state.check() && false;
		move->kingSideCastling = state.castling() == KingSideCastling;
		move->queenSideCastling = state.castling() == QueenSideCastling;

		diagram[move->to] = diagram[move->from];
		diagram[move->from] = Empty;
	}
}

/* -------------------------------------------------------------------------- */
/* -- State                                                                -- */
/* -------------------------------------------------------------------------- */

Game::State::State(const Problem& problem)
{
	_castlings = problem.castlings();
	_enpassant = Nowhere;
	_color = problem.turn();
	_check = false;  // TODO

	_castling = NoCastling;
	_captured = nullptr;
	_from = Nowhere;
	_to = Nowhere;
}

/* -------------------------------------------------------------------------- */

Game::State::State(const State& state, Square from, Square to, const Piece *captured, CastlingSide castling, const array<bool, NumCastlingSides>& castlings)
{
	_castlings = state._castlings;
	for (CastlingSide side : AllCastlingSides())
		_castlings[state._color][side] = _castlings[state._color][side] && castlings[side];

	_enpassant = Nowhere;
	_color = !state._color;
	_check = false;

	_castling = castling;
	_captured = captured;
	_from = from;
	_to = to;
}

/* -------------------------------------------------------------------------- */

}
