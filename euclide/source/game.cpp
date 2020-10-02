#include "game.h"
#include "pieces.h"
#include "problem.h"
#include "conditions.h"
#include "tables/tables.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */
/* -- Game                                                                 -- */
/* -------------------------------------------------------------------------- */

Game::Game(const EUCLIDE_Configuration& configuration, const EUCLIDE_Callbacks& callbacks, const Problem& problem, const array<Pieces, NumColors>& pieces, const array<int, NumColors>& freeMoves)
	: m_configuration(configuration), m_callbacks(callbacks), m_problem(problem), m_pieces(pieces), m_hash(problem), m_cache(16 * 1024 * 1024)
{
	/* -- Initialize constant tables -- */

	for (Glyph glyph : AllGlyphs())
		Tables::initializeLegalMoves(&m_captures[glyph], problem.piece(glyph), color(glyph), problem.variant(), true, true);

	for (Glyph glyph : AllGlyphs())
		m_constraints[glyph] = *Tables::getMoveConstraints(problem.piece(glyph), problem.variant(), true, false);

	Tables::initializeLineOfSights(problem.pieces(), problem.variant(), &m_lines);

	/* -- Initialize position and relevant information -- */

	m_board.fill(nullptr);
	for (Color color : AllColors())
		for (const Piece& piece : pieces[color])
			m_board[piece.initialSquare()] = &piece;

	for (Color color : AllColors())
		m_position[color].set([&](Square square) { return Euclide::color(problem.initialPosition(square)) == color; });

	for (Color color : AllColors())
		m_kings[color] = pieces[color][0].initialSquare();

	m_diagram.set([&](Square square) { return problem.diagramPosition(square) != Empty; });

	/* -- Initialize piece states -- */

	for (Color color : AllColors())
	{
		for (const Piece& piece : m_pieces[color])
		{
			piece.state.glyph = piece.glyph();
			piece.state.square = piece.initialSquare();
			piece.state.moves = 0;
		}
	}

	/* -- Initialize free moves -- */

	m_moves = freeMoves;

	/* -- Initialize solution/position counters -- */

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
	thinking.positions = m_positions;
	thinking.numHalfMoves = 0;

	if (m_callbacks.displayThinking)
		(*m_callbacks.displayThinking)(m_callbacks.handle, &thinking);
}

/* -------------------------------------------------------------------------- */

bool Game::play(const State& _state)
{
	m_positions += 1;

	/* -- Early exit if position is in cache -- */

	auto cachable = [&](int moves) { return (moves >= 4) && (moves <= m_problem.moves() - 4); };

	if (cachable(m_states.size()) && !_state.enpassant())
		if (m_cache.contains(m_hash, m_states.size()))
			return false;

	/* -- Thinking callback -- */

	if ((m_states.size() == countof(EUCLIDE_Thinking::moves)) || (m_positions % (1024 * 1024) == 0))
	{
		EUCLIDE_Thinking thinking;
		thinking.positions = m_positions;
		cmoves(thinking.moves, thinking.numHalfMoves = std::min<int>(countof(EUCLIDE_Thinking::moves), m_states.size()));

		if (m_callbacks.displayThinking)
			(*m_callbacks.displayThinking)(m_callbacks.handle, &thinking);

		if (m_callbacks.abort)
			if ((*m_callbacks.abort)(m_callbacks.handle))
				throw UserAborted;
	}

	/* -- End recursion -- */

	if (int(m_states.size()) >= m_problem.moves())
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

         /* -- Stop searching if we have found many solutions -- */

         if ((m_configuration.maxSolutions > 0) && (m_solutions >= m_configuration.maxSolutions))
            throw Ok;
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

		const Squares destinations = piece.moves(from) - m_position[color];
		for (Square to : ValidSquares(destinations))
		{
			/* -- Check if capture is ok -- */

			const bool enpassant = _state.enpassant(to) && m_problem.enpassant(piece.glyph());
			const bool capture = m_position[!color][to] || enpassant;
			const Square where = enpassant ? square(col(to), row(from)) : to;

			if (capture && !maybe(m_board[where]->captured()))
				continue;

			if (!capture && piece.captures(from)[to])
				continue;

			/* -- Check if move is possible given other pieces on the board -- */

			if (position & piece.constraints(from, to, capture))
				continue;

			/* -- Check if all conditions have been satisfied -- */

			if (!piece.conditions(from, to).satisfied(m_board))
				continue;

			/* -- Check if there are any free moves left -- */

			const int hiddenMoves = std::max(0, piece.requiredMoves() - (piece.requiredMovesTo(from) + piece.requiredMovesFrom(from)));
			const int extraMoves = std::max(0, 1 + piece.requiredMovesFrom(to) - piece.requiredMovesFrom(from) - hiddenMoves);
			if (extraMoves > m_moves[color])
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

			/* -- Handle promotion -- */

			const bool promotion = piece.promotions()[to];
			const Glyphs glyphs = promotion ? piece.glyphs() - Glyphs(piece.glyph()) : Glyphs(piece.glyph());
			for (Glyph glyph : ValidGlyphs(glyphs))
			{
				/* -- Perform move and compute new game state -- */

				State state = move(_state, from, to, glyph, castling);
				m_states.push_back(&state);

				m_moves[color] -= extraMoves;

				/* -- Move is valid only if the king is not in check -- */

				bool valid = true;
				if (piece.royal() || _state.check())
					valid &= !checked(m_kings[color], color);
				else
					valid &= !checked(m_kings[color], from, color);

				if (valid)
				{
					/* -- Set check state -- */

					if (checks(glyph, to, m_kings[!color]))
						state.check(true);

					if (checked(m_kings[!color], from, !color))
						state.check(true);

					if (castling != NoCastling)
						if (checks(m_board[Castlings[color][castling].free]->glyph(), Castlings[color][castling].free, m_kings[!color]))
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

				m_moves[color] += extraMoves;

				m_states.pop_back();
				undo(state);
			}
		}
	}

	/* -- Done -- */

	return solved;
}

/* -------------------------------------------------------------------------- */

Game::State Game::move(const State& state, Square from, Square to, Glyph glyph, CastlingSide castling)
{
	const Piece *piece = m_board[from];
	const Color color = state.color();

	const Square capture = (state.enpassant(to) && m_problem.enpassant(piece->glyph())) ? square(col(to), row(from)) : to;
	const Piece *captured = m_board[capture];

	/* -- Update board position -- */

	m_hash[capture] = Empty;
	m_hash[to] = glyph;
	m_hash[from] = Empty;

	m_board[capture] = nullptr;
	m_board[to] = piece;
	m_board[from] = nullptr;

	assert(m_position[color][from]);
	assert(!m_position[color][to]);

	m_position[color][to] = true;
	m_position[color][from] = false;
	m_position[!color][capture] = false;

	if (m_kings[color] == from)
		m_kings[color] = to;

	/* -- Update piece state -- */

	piece->state.glyph = glyph;
	piece->state.square = to;
	piece->state.moves += 1;

	if (captured)
		captured->state.square = Nowhere;

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

		m_board[free]->state.square = free;
	}

	/* -- Update castlings states -- */

	array<bool, NumCastlingSides> castlings;
	for (CastlingSide side : AllCastlingSides())
		castlings[side] = state.castling(side) && (m_kings[color] != to) && (m_board[to]->initialSquare() != Castlings[color][side].rook);

	m_hash[m_kings[color]] = castlings;

	/* -- Update en passant state -- */

	Square enpassant = Nowhere;
	if (m_problem.enpassant(piece->glyph()) && maybe(piece->captured()))
		if (abs(row(from) - row(to)) == 2)
			enpassant = Square((from + to) / 2);

	/* -- Return new state -- */

	return State(state, from, to, (glyph != piece->glyph()) ? glyph : Empty, captured, capture, castling, castlings, enpassant);
}

/* -------------------------------------------------------------------------- */

void Game::undo(const State& state)
{
	const Color color = !state.color();
	const Square capture = state.capture();
	const Square from = state.from();
	const Square to = state.to();
	const Piece *piece = m_board[to];
	const Piece *captured = state.captured();
	const Glyph promotion = state.promotion();

	/* -- Update board position -- */

	m_hash[to] = Empty;
	m_hash[from] = promotion ? piece->glyph() : piece->state.glyph;
	if (captured)
		m_hash[capture] = captured->glyph();

	m_board[to] = nullptr;
	m_board[from] = piece;
	m_board[capture] = captured;

	m_position[color][to] = false;
	m_position[color][from] = true;
	m_position[!color][capture] = captured;

	if (m_kings[color] == to)
		m_kings[color] = from;

	/* -- Update piece state -- */

	piece->state.glyph = promotion ? piece->glyph() : piece->state.glyph;
	piece->state.square = from;
	piece->state.moves -= 1;

	if (captured)
		captured->state.square = to;

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

		m_board[rook]->state.square = rook;
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
		if (checks(m_board[from]->state.glyph, from, king))
			return true;

	return false;
}

/* -------------------------------------------------------------------------- */

bool Game::solved() const
{
	/* -- Check number of moves -- */

	assert(int(m_states.size()) == m_problem.moves());

	/* -- Quick check for diagram layout -- */

	if ((m_position[White] | m_position[Black]) != m_diagram)
		return false;

	/* -- Check all pieces -- */

	for (Square square : ValidSquares(m_diagram))
		if (m_board[square]->state.glyph != m_problem.diagramPosition(square))
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

		const Color color = state.color();
		const Square from = state.from();
		const Square to = state.to();
		const Glyph glyph = state.promotion() ? state.promotion() : diagram[from];

		const Square capture = state.capture();
		const CastlingSide castling = state.castling();

		move->glyph = static_cast<EUCLIDE_Glyph>(diagram[from]);
		move->promotion = static_cast<EUCLIDE_Glyph>(glyph);
		move->captured = static_cast<EUCLIDE_Glyph>(diagram[capture]);

		move->move = (m + ((color == Black) ? 3 : 2)) / 2;
		move->from = from;
		move->to = to;

		move->capture = state.captured() != nullptr;
		move->enpassant = capture != to;
		move->check = state.check();
		move->mate = state.check() && false;
		move->kingSideCastling = castling == KingSideCastling;
		move->queenSideCastling = castling == QueenSideCastling;

		diagram[capture] = Empty;
		diagram[move->to] = glyph;
		diagram[move->from] = Empty;

		if (castling != NoCastling)
		{
			diagram[Castlings[!color][castling].free] = (color == White) ? BlackRook : WhiteRook;
			diagram[Castlings[!color][castling].rook] = Empty;
		}
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
	m_check = false;

	m_castling = NoCastling;
	m_captured = nullptr;
	m_from = Nowhere;
	m_to = Nowhere;
}

/* -------------------------------------------------------------------------- */

Game::State::State(const State& state, Square from, Square to, Glyph promotion, const Piece *captured, Square capture, CastlingSide castling, const array<bool, NumCastlingSides>& castlings, Square enpassant)
{
	m_castlings = state.m_castlings;
	for (CastlingSide side : AllCastlingSides())
		m_castlings[state.m_color][side] = m_castlings[state.m_color][side] && castlings[side];

	m_enpassant = enpassant;
	m_color = !state.m_color;
	m_check = false;

	m_promotion = promotion;
	m_castling = castling;
	m_captured = captured;
	m_capture = capture;
	m_from = from;
	m_to = to;
}

/* -------------------------------------------------------------------------- */

}
