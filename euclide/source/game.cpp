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
	: m_configuration(configuration), m_callbacks(callbacks), m_problem(problem), m_pieces(pieces), m_hash(problem), m_cache(16 * 1024 * 1024)
{
	for (Glyph glyph : AllGlyphs())
		Tables::initializeLegalMoves(&m_captures[glyph], problem.piece(glyph), color(glyph), problem.variant(), true);

	for (Glyph glyph : AllGlyphs())
		m_constraints[glyph] = *Tables::getMoveConstraints(problem.piece(glyph), problem.variant(), true, false);

	Tables::initializeLineOfSights(problem.pieces(), problem.variant(), &m_lines);

	m_board.fill(nullptr);
	for (Color color : AllColors())
		for (const Piece& piece : pieces[color])
			m_board[piece.initialSquare()] = &piece;

	for (Color color : AllColors())
		m_position[color].set([&](Square square) { return Euclide::color(problem.initialPosition(square)) == color; });

	for (Color color : AllColors())
		m_kings[color] = pieces[color][0].initialSquare();

	m_diagram.set([&](Square square) { return problem.diagramPosition(square) != Empty; });

	m_positions = 0;
	m_solutions = 0;
}

/* -------------------------------------------------------------------------- */

Game::~Game()
{
}

/* -------------------------------------------------------------------------- */

void Game::play()
{
	/* -- Recursively play all moves from initial state -- */

	const State state(m_problem);
	play(state);

	/* -- Done -- */

	EUCLIDE_Thinking thinking;
	memset(&thinking, 0, sizeof(thinking));
	thinking.positions = m_positions;

	if (m_callbacks.displayThinking)
		(*m_callbacks.displayThinking)(m_callbacks.handle, &thinking);
}

/* -------------------------------------------------------------------------- */

bool Game::play(const State& _state)
{
	m_positions += 1;

	/* -- Early exit if position is in cache -- */

	auto cachable = [&](int moves) { return (moves >= 4) && (moves <= m_problem.moves() - 4); };

	if (cachable(m_states.size()))
		if (m_cache.contains(m_hash, m_states.size()))
			return false;

	/* -- Thinking callback -- */

	if ((m_states.size() == countof(EUCLIDE_Thinking::moves)) || (m_positions % (1024 * 1024) == 0))
	{
		EUCLIDE_Thinking thinking;
		thinking.positions = m_positions;
		cmoves(thinking.moves, std::min<int>(countof(EUCLIDE_Thinking::moves), m_states.size()));

		if (m_callbacks.displayThinking)
			(*m_callbacks.displayThinking)(m_callbacks.handle, &thinking);
	}

	/* -- End recursion -- */

	if (m_states.size() >= m_problem.moves())
	{
		/* -- Check for solution -- */

		const bool solved = this->solved();
		if (solved)
		{
			EUCLIDE_Solution solution;
			solution.numHalfMoves = m_states.size();
			cmoves(solution.moves, m_states.size());
			solution.solution = ++m_solutions;

			if (m_callbacks.displaySolution)
				(*m_callbacks.displaySolution)(m_callbacks.handle, &solution);
		}

		/* -- End recursion -- */

		return solved;
	}

	/* -- Get legal moves -- */

	const Squares position = m_position[White] | m_position[Black];
	const Color color = _state.color();

	bool solved = false;

	for (Square from : ValidSquares(m_position[color]))
	{
		const Piece& piece = *m_board[from];

		for (Square to : ValidSquares(piece.moves(from)))
		{
			/* -- Check if capture is ok -- */

			const bool capture = m_position[!color][to];
			if (capture && !is(m_board[to]->captured()))
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
			m_states.push_back(&state);

			/* -- Move is valid only if the king is not in check -- */

			bool valid = true;
			if (piece.royal() || _state.check())
				valid &= !checked(m_kings[color], color);
			else
				valid &= !checked(m_kings[color], from, color);

			if (valid)
			{
				/* -- Set check state -- */

				if (checks(piece.initialGlyph(), to, m_kings[!color]))
					state.check(true);

				if (checked(m_kings[!color], from, !color))
					state.check(true);

				if (castling != NoCastling)
					if (checks(m_board[Castlings[color][castling].free]->initialGlyph(), Castlings[color][castling].free, m_kings[!color]))
						state.check(true);

				/* -- Recursive call -- */

				if (!play(state))
				{
					/* -- Add position to cache if it does not lead to a solution -- */

					if (cachable(m_states.size()))
						m_cache.insert(m_hash, m_states.size());
				}
				else
				{
					solved = true;
				}
			}

			/* -- Undo move -- */

			m_states.pop_back();
			undo(state);
		}
	}

	/* -- Done -- */

	return solved;
}

/* -------------------------------------------------------------------------- */

Game::State Game::move(const State& state, Square from, Square to, CastlingSide castling)
{
	const Color color = state.color();

	/* -- Update board position -- */

	const Piece *captured = m_board[to];

	m_hash[to] = m_board[from]->glyph();
	m_hash[from] = Empty;

	m_board[to] = m_board[from];
	m_board[from] = nullptr;

	assert(m_position[color][from]);
	assert(!m_position[color][to]);

	m_position[color][to] = true;
	m_position[!color][to] = false;
	m_position[color][from] = false;

	if (m_kings[color] == from)
		m_kings[color] = to;

	/* -- Handle castling -- */

	if (castling != NoCastling)
	{
		const Square rook = Castlings[color][castling].rook;
		const Square free = Castlings[color][castling].free;

		m_hash[free] = m_board[rook]->glyph();
		m_hash[rook] = Empty;

		m_board[free] = m_board[rook];
		m_board[rook] = nullptr;

		assert(m_position[color][rook]);
		assert(!m_position[color][free]);

		m_position[color][free] = true;
		m_position[color][rook] = false;

		assert(!captured);
	}

	/* -- Update castlings states -- */

	array<bool, NumCastlingSides> castlings;
	for (CastlingSide side : AllCastlingSides())
		castlings[side] = state.castling(side) && (m_kings[color] != to) && (m_board[to]->initialSquare() != Castlings[color][side].rook);

	m_hash[m_kings[color]] = castlings;

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

	m_hash[from] = m_board[to]->glyph();
	m_hash[to] = state.captured() ? state.captured()->glyph() : Empty;

	m_board[from] = m_board[to];
	m_board[to] = state.captured();

	m_position[color][to] = false;
	m_position[color][from] = true;
	if (state.captured())
		m_position[!color][to] = true;

	if (m_kings[color] == to)
		m_kings[color] = from;

	/* -- Handle castling -- */

	if (state.castling() != NoCastling)
	{
		const Square rook = Castlings[color][state.castling()].rook;
		const Square free = Castlings[color][state.castling()].free;

		m_hash[rook] = m_board[free]->glyph();
		m_hash[free] = Empty;

		m_board[rook] = m_board[free];
		m_board[free] = nullptr;

		m_position[color][rook] = true;
		m_position[color][free] = false;
	}

	m_hash[m_kings[color]] = state.castlings(color);
}

/* -------------------------------------------------------------------------- */

bool Game::checks(Glyph glyph, Square from, Square king) const
{
	const Squares blockers = m_position[White] | m_position[Black];

	if (m_captures[glyph][from][king])
		if (!(m_constraints[glyph][from][king] & blockers))
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
	const Squares enemies = m_lines[color][king][free] & m_position[!color];

	for (Square from : ValidSquares(enemies))
		if (checks(m_board[from]->initialGlyph(), from, king))
			return true;

	return false;
}

/* -------------------------------------------------------------------------- */

bool Game::solved() const
{
	/* -- Check number of moves -- */

	assert(m_states.size() == m_problem.moves());

	/* -- Quick check for diagram layout -- */

	if ((m_position[White] | m_position[Black]) != m_diagram)
		return false;

	/* -- Check all pieces -- */

	for (Square square : ValidSquares(m_diagram))
		if (m_board[square]->glyph() != m_problem.diagramPosition(square))
			return false;

	/* -- Solution found -- */

	return true;
}

/* -------------------------------------------------------------------------- */

void Game::cmoves(EUCLIDE_Move *moves, int nmoves) const
{
	array<Glyph, NumSquares> diagram = m_problem.initialPosition();

	for (int m = 0; m < nmoves; m++)
	{
		const State& state = *m_states[m];
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
	m_castlings = problem.castlings();
	m_enpassant = Nowhere;
	m_color = problem.turn();
	m_check = false;  // TODO

	m_castling = NoCastling;
	m_captured = nullptr;
	m_from = Nowhere;
	m_to = Nowhere;
}

/* -------------------------------------------------------------------------- */

Game::State::State(const State& state, Square from, Square to, const Piece *captured, CastlingSide castling, const array<bool, NumCastlingSides>& castlings)
{
	m_castlings = state.m_castlings;
	for (CastlingSide side : AllCastlingSides())
		m_castlings[state.m_color][side] = m_castlings[state.m_color][side] && castlings[side];

	m_enpassant = Nowhere;
	m_color = !state.m_color;
	m_check = false;

	m_castling = castling;
	m_captured = captured;
	m_from = from;
	m_to = to;
}

/* -------------------------------------------------------------------------- */

}
