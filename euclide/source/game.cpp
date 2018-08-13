#include "game.h"
#include "pieces.h"
#include "problem.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */
/* -- Game                                                                 -- */
/* -------------------------------------------------------------------------- */

Game::Game(const Problem& problem, const array<Pieces, NumColors>& pieces)
	: _problem(problem), _pieces(pieces), _state(problem, pieces)
{
	for (Square square : AllSquares())
		if (_problem.diagramPosition(square) != Empty)
			_diagram[square] = true;

	_positions = 0;
	_solutions = 0;
}

/* -------------------------------------------------------------------------- */

Game::~Game()
{
}

/* -------------------------------------------------------------------------- */

void Game::play(const EUCLIDE_Callbacks& callbacks)
{
	_positions += 1;

	/* -- Thinking callback -- */

	if (_state.move() == 8)
	{
		EUCLIDE_Thinking thinking;
		thinking.positions = _positions;
		_state.moves(_problem, std::extent<decltype(thinking.moves)>::value, thinking.moves);

		if (callbacks.displayThinking)
			(*callbacks.displayThinking)(callbacks.handle, &thinking);
	}

	/* -- End recursion -- */

	if (_state.move() >= _problem.moves())
	{
		/* -- Check for solution -- */

		if (solved())
		{
			EUCLIDE_Solution solution;
			solution.numHalfMoves = _state.move();
			_state.moves(_problem, _state.move(), solution.moves);
			solution.solution = ++_solutions;

			if (callbacks.displaySolution)
				(*callbacks.displaySolution)(callbacks.handle, &solution);
		}

		/* -- End recursion -- */

		return;
	}

	/* -- Get legal moves -- */

	std::vector<State::Move> moves;
	_state.moves(moves);

	/* -- Try all legal moves -- */

	for (const State::Move& move : moves)
	{
		/* -- Perform move -- */

		const Piece *capture = _state.move(move.from, move.to);

		/* -- Recursive call -- */

		play(callbacks);

		/* -- Undo move -- */

		_state.undo(move.from, move.to, capture);
	}

	/* -- Done -- */

	if (!_state.move())
	{
		EUCLIDE_Thinking thinking;
		memset(&thinking, 0, sizeof(thinking));
		thinking.positions = _positions;

		if (callbacks.displayThinking)
			(*callbacks.displayThinking)(callbacks.handle, &thinking);
	}
}

/* -------------------------------------------------------------------------- */

bool Game::solved() const
{
	/* -- Check number of moves -- */

	assert(_state.move() == _problem.moves());

	/* -- Quick check for diagram layout -- */

	if (_state.squares() != _diagram)
		return false;

	/* -- Check all pieces -- */

	for (Square square : ValidSquares(_diagram))
		if (_state.piece(square).glyph() != _problem.diagramPosition(square))
			return false;

	/* -- Solution found! -- */

	return true;
}

/* -------------------------------------------------------------------------- */
/* -- State                                                                -- */
/* -------------------------------------------------------------------------- */

Game::State::State(const Problem& problem, const array<Pieces, NumColors>& pieces)
{
	_color = problem.turn();
	_move = 0;

	/* -- Dispatch pieces -- */

	_pieces.fill(nullptr);
	for (Color color : AllColors())
		for (const Piece& piece : pieces[color])
			_pieces[piece.square(true)] = &piece;

	/* -- Set list of occupied squares -- */

	_squares.set([&](Square square) { return _pieces[square] != nullptr; });
}

/* -------------------------------------------------------------------------- */

const Piece *Game::State::move(Square from, Square to)
{
	const Piece *capture = _pieces[to];

	_pieces[to] = _pieces[from];
	_pieces[from] = nullptr;

	_squares[to] = true;
	_squares[from] = false;

	_color = !_color;
	_moves[_move++] = Move(from, to);

	return capture;
}

/* -------------------------------------------------------------------------- */

void Game::State::undo(Square from, Square to, const Piece *capture)
{
	_pieces[from] = _pieces[to];
	_pieces[to] = capture;

	_squares[from] = true;
	_squares[to] = (capture != nullptr);

	_color = !_color;
	_move -= 1;
}

/* -------------------------------------------------------------------------- */

void Game::State::moves(std::vector<Move>& moves)
{
	moves.clear();

	for (Square from : ValidSquares(_squares))
	{
		const Piece& piece = *_pieces[from];

		if (piece.color() == _color)
		{
			for (Square to : ValidSquares(piece.moves(from)))
			{
				const bool capture = _pieces[to] && (_pieces[to]->color() != _color);
				if (capture)
					continue;

				if (!(_squares & piece.constraints(from, to, capture)))
					moves.emplace_back(from, to);
			}
		}
	}
}

/* -------------------------------------------------------------------------- */

void Game::State::moves(const Problem& problem, int nmoves, EUCLIDE_Move *moves) const
{
	array<Glyph, NumSquares> diagram = problem.initialPosition();

	for (int m = 0; m < nmoves; m++)
	{
		EUCLIDE_Move *move = moves + m;

		move->glyph = static_cast<EUCLIDE_Glyph>(diagram[_moves[m].from]);
		move->promotion = static_cast<EUCLIDE_Glyph>(diagram[_moves[m].from]);
		move->captured = static_cast<EUCLIDE_Glyph>(diagram[_moves[m].to]);

		move->move = (m + ((problem.turn() == Black) ? 3 : 2)) / 2;
		move->from = _moves[m].from;
		move->to = _moves[m].to;

		move->capture = (diagram[_moves[m].to] != Empty);
		move->enpassant = false;          // TODO
		move->check = false;              // TODO
		move->mate = false;               // TODO
		move->kingSideCastling = false;   // TODO
		move->queenSideCastling = false;  // TODO

		diagram[move->to] = diagram[move->from];
		diagram[move->from] = Empty;
	}
}

/* -------------------------------------------------------------------------- */

}
