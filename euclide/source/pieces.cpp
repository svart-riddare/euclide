#include "pieces.h"
#include "actions.h"
#include "problem.h"
#include "tables/tables.h"
#include "cache.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */
/* -- Piece                                                                -- */
/* -------------------------------------------------------------------------- */

Piece::Piece(const Problem& problem, Square square, Man man, Glyph glyph, tribool promoted)
{
	/* -- Piece initial characteristics -- */

	m_man = man;
	m_child = problem.initialPosition(square);
	m_color = Euclide::color(m_child);

	/* -- Promotion -- */

	const Species species = problem.piece(m_child);
	m_promoted = ((species == Pawn) && problem.promotionPieces(m_color)) ? promoted : tribool(false);

	/* -- Piece final characteristics -- */

	m_glyph = maybe(m_promoted) ? glyph : m_child;
	m_glyphs = m_glyph ? Glyphs(m_glyph) : (Glyphs(m_child) | PromotionGlyphs[m_color]);

	m_species = problem.piece(m_glyph);

	m_royal = (m_species == King);
	m_captured = (m_royal || !problem.capturedPieces(m_color)) ? tribool(false) : unknown;

	/* -- Piece squares -- */

	m_initialSquare = square;
	m_castlingSquare = Nowhere;
	m_finalSquare = Nowhere;

	if (maybe(m_promoted))
		m_promotionSquares = PromotionSquares[m_color];

	/* -- Initialize number of available moves and captures -- */

	m_availableMoves = problem.moves(m_color);
	m_availableCaptures = problem.initialPieces(!m_color) - problem.diagramPieces(!m_color);

	m_requiredMoves = 0;
	m_requiredCaptures = 0;

	m_freeMoves = m_availableMoves;
	m_freeCaptures = m_availableCaptures;

	/* -- Initialize possible final squares and capture squares -- */

	for (Square square : AllSquares())
		m_possibleSquares.set(square, maybe(m_captured) || m_glyphs[problem.diagramPosition(square)]);

	if (m_availableCaptures)
		m_possibleCaptures.set();

	/* -- Initialize legal moves and move tables -- */

	Tables::initializeLegalMoves(&m_moves, m_glyph ? m_species : Pawn, m_color, problem.variant(), m_availableCaptures ? unknown : tribool(false), maybe(m_promoted));
	m_xmoves = Tables::getCaptureMoves(m_glyph ? m_species : Pawn, m_color, problem.variant());

	m_constraints = Tables::getMoveConstraints(m_glyph ? m_species : Pawn, problem.variant(), false);
	m_xconstraints = Tables::getMoveConstraints(m_glyph ? m_species : Pawn, problem.variant(), true);

	m_checks = Tables::getUnstoppableChecks(m_glyph ? m_species : Pawn, m_color, problem.variant());

	if (is(m_promoted))
	{
		Tables::initializeLegalMoves(&m_pawn.moves, Pawn, m_color, problem.variant(), m_availableCaptures ? unknown : tribool(false), true);
		m_pawn.xmoves = Tables::getCaptureMoves(Pawn, m_color, problem.variant());

		m_pawn.constraints = Tables::getMoveConstraints(Pawn, problem.variant(), false);
		m_pawn.xconstraints = Tables::getMoveConstraints(Pawn, problem.variant(), true);

		m_pawn.checks = Tables::getUnstoppableChecks(Pawn, m_color, problem.variant());
	}
	else
	{
		m_pawn.moves.fill(Squares());
		m_pawn.xmoves = nullptr;

		m_pawn.constraints = nullptr;
		m_pawn.xconstraints = nullptr;

		m_pawn.checks = nullptr;
	}

	/* -- Initialize occupied squares -- */

	for (Square square : AllSquares())
		m_occupied[square].pieces.fill(nullptr);

	/* -- Distances will be computed later -- */

	m_distances.fill(0);
	m_rdistances.fill(0);
	m_captures.fill(0);
	m_rcaptures.fill(0);

	m_pawn.distances.fill(is(promoted) ? 0 : Infinity);
	m_pawn.rdistances.fill(is(promoted) ? 0 : Infinity);
	m_pawn.captures.fill(is(promoted) ? 0 : Infinity);
	m_pawn.rcaptures.fill(is(promoted) ? 0 : Infinity);

	/* -- Handle castling -- */

	std::fill_n(m_castling, NumCastlingSides, false);

	if (((m_glyph == WhiteKing) || (m_glyph == BlackKing)) && !is(promoted))
		for (CastlingSide side : AllCastlingSides())
			if (m_initialSquare == Castlings[m_color][side].from)
				if (problem.castling(m_color, side) && (problem.variant() != Bichromatic))
					m_moves[Castlings[m_color][side].from][Castlings[m_color][side].to] = true, m_castling[side] = unknown;

	if (((m_glyph == WhiteRook) || (m_glyph == BlackRook)) && !is(promoted))
		for (CastlingSide side : AllCastlingSides())
			if (m_initialSquare == Castlings[m_color][side].rook)
				if (problem.castling(m_color, side))
					m_castlingSquare = Castlings[m_color][side].free, m_castling[side] = unknown;

	/* -- Actions will be initialized later -- */

	m_actions = nullptr;

	/* -- Initialize personalities, when the final glyph is not known -- */

	m_pieces.fill(nullptr);
	m_pieces[m_child] = this;
	m_piece = this;
	m_virtual = !unknown(promoted);

	if (!m_glyph)
	{
		for (Glyph glyph : ValidGlyphs(m_glyphs))
			m_personalities.emplace_back(problem, m_initialSquare, m_man, glyph, glyph != m_child);

		for (Piece& personality : m_personalities)
			m_pieces[personality.glyph()] = &personality;

		for (Piece& personality : m_personalities)
			personality.m_piece = this;
	}

	/* -- Update possible moves -- */

	m_update = true;
	update();
}

/* -------------------------------------------------------------------------- */

Piece::~Piece()
{
	delete m_actions;
}

/* -------------------------------------------------------------------------- */

void Piece::initializeActions()
{
	assert(!m_actions);
	m_actions = new Actions(*this);
	updateConsequences();
}

/* -------------------------------------------------------------------------- */

void Piece::setCastling(CastlingSide side, bool castling)
{
	if (!unknown(m_castling[side]))
		return;

	/* -- King can only castle on one side -- */

	if (castling && m_royal)
		for (CastlingSide other : AllCastlingSides())
			if (side != other)
				setCastling(other, false);

	/* -- Prohibit castling moves -- */

	if (!castling)
	{
		if (m_royal)
			m_moves[Castlings[m_color][side].from][Castlings[m_color][side].to] = false;

		m_castlingSquare = Nowhere;
	}

	/* -- Update state -- */

	m_castling[side] = castling;
	m_update = true;
}

/* -------------------------------------------------------------------------- */

void Piece::setCaptured(bool captured)
{
	if (!unknown(m_captured))
		return;

	for (Piece& personality : m_personalities)
		personality.setCaptured(captured);

	m_captured = captured;
	m_update = true;
}

/* -------------------------------------------------------------------------- */

void Piece::setRequiredMoves(int requiredMoves)
{
	if (requiredMoves <= m_requiredMoves)
		return;

	for (Piece& personality : m_personalities)
		personality.setRequiredMoves(requiredMoves);

	xstd::maximize(m_requiredMoves, requiredMoves);
	m_update = true;
}

/* -------------------------------------------------------------------------- */

void Piece::setRequiredCaptures(int requiredCaptures)
{
	if (requiredCaptures <= m_requiredCaptures)
		return;

	for (Piece& personality : m_personalities)
		personality.setRequiredCaptures(requiredCaptures);

	xstd::maximize(m_requiredCaptures, requiredCaptures);
	m_update = true;
}

/* -------------------------------------------------------------------------- */

void Piece::setAvailableMoves(int availableMoves, int freeMoves)
{
	if ((availableMoves >= m_availableMoves) && (freeMoves >= m_freeMoves))
		return;

	for (Piece& personality : m_personalities)
		personality.setAvailableMoves(availableMoves, freeMoves);

	xstd::minimize(m_availableMoves, availableMoves);
	xstd::minimize(m_freeMoves, freeMoves);
	m_update = true;
}

/* -------------------------------------------------------------------------- */

void Piece::setAvailableCaptures(int availableCaptures, int freeCaptures)
{
	if ((availableCaptures >= m_availableCaptures) && (freeCaptures >= m_freeCaptures))
		return;

	for (Piece& personality : m_personalities)
		personality.setAvailableCaptures(availableCaptures, freeCaptures);

	xstd::minimize(m_availableCaptures, availableCaptures);
	xstd::minimize(m_freeCaptures, freeCaptures);
	m_update = true;
}

/* -------------------------------------------------------------------------- */

void Piece::setPossibleGlyphs(Glyphs glyphs)
{
	if ((m_glyphs & glyphs) == m_glyphs)
		return;

	m_glyphs &= glyphs;
	m_update = true;
}

/* -------------------------------------------------------------------------- */

void Piece::setPossibleSquares(Squares squares)
{
	if ((m_possibleSquares & squares) == m_possibleSquares)
		return;

	for (Piece& personality : m_personalities)
		personality.setPossibleSquares(squares);

	m_possibleSquares &= squares;
	m_update = true;
}

/* -------------------------------------------------------------------------- */

void Piece::setPossibleCaptures(Squares squares)
{
	if ((m_possibleCaptures & squares) == m_possibleCaptures)
		return;

	for (Piece& personality : m_personalities)
		personality.setPossibleCaptures(squares);

	m_possibleCaptures &= squares;
	m_update = true;
}

/* -------------------------------------------------------------------------- */

void Piece::bypassObstacles(const Piece& blocker)
{
	const Squares& obstacles = blocker.stops();

	/* -- Handle personalities -- */

	for (Piece& personality : m_personalities)
		personality.bypassObstacles(blocker);

	if (!m_glyph)
		return;

	/* -- Blocked movements -- */

	if ((obstacles & m_route).any())
		for (Square from : ValidSquares(m_stops))
			for (Square to : ValidSquares(m_moves[from]))
				if (obstacles <= ((*m_constraints)[from][to] | from))
					m_moves[from][to] = false, m_update = true;

	/* -- Castling -- */

	if (m_castlingSquare != Nowhere)
		for (CastlingSide side : AllCastlingSides())
			if (obstacles <= (*m_constraints)[Castlings[m_color][side].rook][Castlings[m_color][side].free])
				setCastling(side, false);

	/* -- Checks -- */

	if (m_royal && (blocker.m_color != m_color) && (obstacles.count() == 1))
		for (Square check : ValidSquares((*blocker.m_checks)[obstacles.first()]))
			if (m_route[check])
				for (Square from : ValidSquares(m_stops))
					if (m_moves[from][check])
						m_moves[from][check] = false, m_update = true;
}

/* -------------------------------------------------------------------------- */

int Piece::mutualInteractions(Piece& pieceA, Piece& pieceB, const array<int, NumColors>& freeMoves, bool fast)
{
	const int requiredMoves = pieceA.m_requiredMoves + pieceB.m_requiredMoves;
	const bool enemies = pieceA.m_color != pieceB.m_color;

	/* -- Don't bother if these two pieces can not interact with each other -- */

	const Squares routes[2] = {
		pieceA.m_route | ((enemies && pieceB.m_royal) ? pieceA.m_threats : Squares()),
		pieceB.m_route | ((enemies && pieceA.m_royal) ? pieceB.m_threats : Squares())
	};

	if (!(routes[0] & routes[1]))
		return requiredMoves;

	/* -- Interactions with captures are not yet implemented -- */

	if (maybe(pieceA.m_captured) || maybe(pieceB.m_captured))
		return requiredMoves;

	/* -- Interactions with promotions are not yet implemented -- */

	if (maybe(pieceA.m_promoted) || maybe(pieceB.m_promoted))
		return requiredMoves;

	/* -- Compute available moves for these two pieces -- */

	const int availableMoves = std::min(
		pieceA.m_availableMoves + pieceB.m_availableMoves - (enemies ? 0 : std::min(pieceA.m_freeMoves, pieceB.m_freeMoves)),
		requiredMoves + freeMoves[pieceA.m_color] + (enemies ? freeMoves[pieceB.m_color] : 0)
	);

	/* -- Use fast method if the search space is too large -- */

	const int threshold = 5000;
	if ((pieceA.nmoves() * pieceB.nmoves() > threshold) || (availableMoves - requiredMoves > 20))
		fast = true;

	/* -- Play all possible moves with these two pieces -- */

	array<State, 2> states = {
		State(pieceA, pieceA.m_availableMoves),
		State(pieceB, pieceB.m_availableMoves)
	};

	TwoPieceCache cache;
	const int newRequiredMoves = fast ? fastplay(states, availableMoves, cache) : fullplay(states, availableMoves, availableMoves, cache);

	if (newRequiredMoves >= Infinity)
		throw NoSolution;

	/* -- Store required moves for each piece, if greater than the previously computed values -- */

	for (const State& state : states)
		if (state.requiredMoves > state.piece.m_requiredMoves)
			state.piece.m_requiredMoves = state.requiredMoves, state.piece.m_update = true;

	/* -- Early exit if we have not performed all computations -- */

	if (fast)
		return newRequiredMoves;

	/* -- Remove never played moves and keep track of occupied squares -- */

	for (const State& state : states)
	{
		for (Square square : AllSquares())
		{
			if (state.moves[square] < state.piece.m_moves[square])
				state.piece.m_moves[square] = state.moves[square], state.piece.m_update = true;

			if (state.squares[square].count() == 1)
			{
				const Square occupied = state.squares[square].first();
				if (!state.piece.m_occupied[square].squares[occupied])
				{
					state.piece.m_occupied[square].squares[occupied] = true;
					state.piece.m_occupied[square].pieces[occupied] = &states[&state == &states[0]].piece;
					state.piece.m_update = true;
				}
			}

			if (state.distances[square] > state.piece.m_distances[square])
				state.piece.m_distances[square] = state.distances[square], state.piece.m_update = true;
		}
	}

	/* -- Done -- */

	return newRequiredMoves;
}

/* -------------------------------------------------------------------------- */

void Piece::findConsequences(const std::array<Pieces, NumColors>& pieces)
{
	/* -- Early exit conditions -- */

	if (!m_requiredMoves || maybe(m_captured) || maybe(m_promoted))
		return;

	/* -- Find consequences of playing on a final square -- */

	for (Square final : ValidSquares(m_possibleSquares))
	{
		if (!m_moves[final])
		{
			const Castling *castling = nullptr;
			if (m_royal)
				for (const Castling& c : Castlings[m_color])
					if (final == c.to)
						castling = &c;

			/* -- List actions reaching this final square -- */

			std::vector<Action *> actions;
			for (Square from : AllSquares())
				if (m_moves[from][final])
					actions.push_back(&m_actions->get(from, final));

			/* -- Find interactions with all other relevant pieces -- */

			for (Color color : AllColors())
			{
				for (const Piece& piece : pieces[color])
				{
					if (((m_route & piece.m_route) && (&piece != this)) || ((m_royal || piece.m_royal) && (m_color != piece.m_color)))
					{
						/* -- Exception for castling -- */

						if (castling && (piece.m_castlingSquare == castling->free))
							continue;

						/* -- Early exit if promoted -- */

						if (maybe(piece.m_promoted))
							continue;

						/* -- Find distances assuming final square is blocked, and record any extra moves required -- */

						const array<int, NumSquares> rdistances = maybe(piece.m_captured) ? piece.computeDistancesTo(piece.m_possibleSquares, false) : piece.computeDistancesTo(piece.m_possibleSquares, *this, final);
						for (Square square : ValidSquares(piece.m_stops - Squares(final)))
						{
							const int requiredMoves = std::min(piece.m_distances[square] + rdistances[square], Infinity);
							const int nominalMoves = std::max(piece.m_distances[square] + piece.m_rdistances[square], piece.m_requiredMoves);
							if (requiredMoves > nominalMoves)
								for (Action *action : actions)
									action->consequences().updateRequiredMoves(piece, square, requiredMoves);
						}

						/* -- Sepcial case for rooks which have performed castling -- */

						if (piece.m_distances[piece.m_initialSquare] && (m_castlingSquare != Nowhere))
						{
							assert(piece.m_initialSquare != piece.m_castlingSquare);

							for (Action *action : actions)
								for (const Consequence& consequence : action->consequences().consequences())
									if ((&consequence.piece() == &piece) && consequence.requiredMoves(piece.m_castlingSquare))
										action->consequences().updateRequiredMoves(piece, piece.m_initialSquare, consequence.requiredMoves(piece.m_castlingSquare));
						}
					}
				}
			}
		}
	}
}

/* -------------------------------------------------------------------------- */

bool Piece::update()
{
	bool updated = false;
	for (Piece& personality : m_personalities)
		if (personality.update())
			updated = true;

	if (!m_update && !updated)
		return updated;

	m_glyph ? unfold() : summarize();

	m_update = false;
	return true;
}

/* -------------------------------------------------------------------------- */

const Action& Piece::action(Square from, Square to) const
{
	assert(m_actions);
	return m_actions->get(from, to);
}

/* -------------------------------------------------------------------------- */

const Consequences& Piece::consequences(Square from, Square to) const
{
	return action(from, to).consequences();
}

/* -------------------------------------------------------------------------- */

void Piece::unfold()
{
	assert(m_glyph);

	/* -- Castling for rooks -- */

	if (m_castlingSquare != Nowhere)
		if (!m_moves[m_castlingSquare] && !m_possibleSquares[m_castlingSquare])
			for (CastlingSide side : AllCastlingSides())
				setCastling(side, false);

	if (m_castlingSquare != Nowhere)
		if (!m_moves[m_initialSquare] && !m_possibleSquares[m_initialSquare])
			for (CastlingSide side : AllCastlingSides())
				setCastling(side, true);

	if (m_castlingSquare != Nowhere)
		if (m_distances[m_castlingSquare])
			for (CastlingSide side : AllCastlingSides())
				setCastling(side, false);

	/* -- Compute distances -- */

	updateDistances();
	updateCaptures();

	for (Square square : ValidSquares(m_possibleSquares))
		if ((m_distances[square] > m_availableMoves) || (m_captures[square] > m_availableCaptures))
			m_possibleSquares[square] = false;

	for (Square square : ValidSquares(m_promotionSquares))
		if ((m_pawn.distances[square] > m_availableMoves) || (m_pawn.captures[square] > m_availableCaptures))
			m_promotionSquares[square] = false;

	updateDistancesTo();
	updateCapturesTo();

	/* -- Update possible final square -- */

	if (!m_possibleSquares && !m_virtual)
		throw NoSolution;

	if (m_possibleSquares.count() == 1)
		m_finalSquare = m_possibleSquares.first();

	/* -- Compute minimum number of moves and captures performed by this piece -- */

	xstd::maximize(m_requiredMoves, xstd::min(ValidSquares(m_possibleSquares), [&](Square square) { return m_distances[square]; }));
	xstd::maximize(m_requiredCaptures, xstd::min(ValidSquares(m_possibleSquares), [&](Square square) { return m_captures[square]; }));

	/* -- Remove moves that will obviously never be played -- */

	for (Square from : AllSquares())
		for (Square to : ValidSquares(m_moves[from]))
			if (m_distances[from] + 1 + m_rdistances[to] > m_availableMoves)
				m_moves[from][to] = false;

	if (m_xmoves)
		for (Square from : AllSquares())
			for (Square to : ValidSquares(m_moves[from]))
				if (m_captures[from] + (*m_xmoves)[from][to] + m_rcaptures[to] > m_availableCaptures)
					m_moves[from][to] = false;

	if (is(m_promoted))
		for (Square from : AllSquares())
			for (Square to : ValidSquares(m_pawn.moves[from]))
				if (m_pawn.distances[from] + 1 + m_pawn.rdistances[to] > m_availableMoves)
					m_pawn.moves[from][to] = false;

	if (m_pawn.xmoves)
		for (Square from : AllSquares())
			for (Square to : ValidSquares(m_pawn.moves[from]))
				if (m_pawn.captures[from] + (*m_pawn.xmoves)[from][to] + m_pawn.rcaptures[to] > m_availableCaptures)
					m_pawn.moves[from][to] = false;

	/* -- Update number of possible moves -- */

	m_nmoves = xstd::sum(m_moves, [](const Squares& squares) { return squares.count(); });
	if (is(m_promoted))
		m_nmoves += xstd::sum(m_pawn.moves, [](const Squares& squares) { return squares.count(); });

	/* -- Update castling state according to corresponding king moves -- */

	if (m_royal)
	{
		for (CastlingSide side : AllCastlingSides())
		{
			if (maybe(m_castling[side]))
			{
				if (!m_moves[Castlings[m_color][side].from][Castlings[m_color][side].to])
					setCastling(side, false);

				if (m_moves[Castlings[m_color][side].from].count() == 1)
					if (m_moves[Castlings[m_color][side].from][Castlings[m_color][side].to])
						setCastling(side, true);
			}
		}
	}

	/* -- Get all squares the piece may have crossed or stopped on -- */

	m_stops = Squares(m_initialSquare);
	if (m_castlingSquare != Nowhere)
		m_stops.set(m_castlingSquare);
	m_stops = xstd::merge(m_moves, m_stops);
	if (is(m_promoted))
		m_stops = xstd::merge(m_pawn.moves, m_stops);

	m_route = m_stops;
	for (Square from : AllSquares())
		for (Square to : ValidSquares(m_moves[from]))
			m_route |= (*m_constraints)[from][to];

	m_threats.reset();
	for (Square square : ValidSquares(m_stops))
		m_threats |= (*m_checks)[square];

	/* -- Update possible captures -- */

	if (!m_availableCaptures)
		m_possibleCaptures.reset();

	if (m_possibleCaptures)
	{
		Squares captures;
		for (Square square : AllSquares())
		{
			captures |= m_xmoves ? m_moves[square] & (*m_xmoves)[square] : m_moves[square];
			if (is(m_promoted))
				captures |= m_pawn.xmoves ? m_pawn.moves[square] & (*m_pawn.xmoves)[square] : m_pawn.moves[square];
		}

		m_possibleCaptures &= captures;
	}

	/* -- Update occupied squares -- */

	for (Square square : AllSquares())
	{
		for (bool loop = true; loop; )
		{
			loop = false;
			for (Square occupied : ValidSquares(m_occupied[square].squares))
			{
				for (Square other : ValidSquares(m_occupied[square].pieces[occupied]->m_occupied[occupied].squares))
				{
					if (!m_occupied[square].squares[other])
					{
						m_occupied[square].pieces[other] = m_occupied[square].pieces[occupied]->m_occupied[occupied].pieces[other];
						m_occupied[square].squares[other] = true;
						loop = true;
					}
				}
			}
		}
	}
}

/* -------------------------------------------------------------------------- */

void Piece::summarize()
{
	assert(!m_glyph);

	/* -- Update distances -- */

	m_distances.fill(Infinity);
	for (const Piece& personality : m_personalities)
		xstd::minimize(m_distances, personality.m_distances);

	m_captures.fill(Infinity);
	for (const Piece& personality : m_personalities)
		xstd::minimize(m_captures, personality.m_captures);

	m_possibleSquares = xstd::merge(m_personalities, Squares(), [](const Piece& piece) { return piece.m_possibleSquares; });
	m_possibleCaptures = xstd::merge(m_personalities, Squares(), [](const Piece& piece) { return piece.m_possibleCaptures; });

	m_rdistances.fill(Infinity);
	for (const Piece& personality : m_personalities)
		xstd::minimize(m_rdistances, personality.m_rdistances);

	m_rcaptures.fill(Infinity);
	for (const Piece& personality : m_personalities)
		xstd::minimize(m_rcaptures, personality.m_rcaptures);

	/* -- Update possible promotion squares -- */

	m_promotionSquares = xstd::merge(m_personalities, Squares(), [](const Piece& piece) { return piece.m_promotionSquares; });

	/* -- Update possible glyphs -- */

	for (Glyph glyph : ValidGlyphs(m_glyphs))
		if (!m_pieces[glyph]->m_possibleSquares)
			m_glyphs.reset(glyph);

	/* -- Remove unused personalities -- */

	m_personalities.remove_if([&](const Piece& piece) -> bool
		{ if (!m_glyphs[piece.m_glyph]) { m_pieces[piece.m_glyph] = nullptr; return true; } return false; }
	);

	if (m_personalities.empty())
		throw NoSolution;

	/* -- Collapse piece if only one personality remains -- */

	if (m_glyphs.count() == 1)
	{
		assert(m_personalities.size() == 1);
		std::list<Piece> personalities(std::move(m_personalities));
		*this = personalities.front();

		m_pieces[m_glyph] = this;
		m_piece = this;
		m_virtual = false;
		return;
	}

	/* -- Update possible final square -- */

	if (m_possibleSquares.count() == 1)
		m_finalSquare = m_possibleSquares.first();

	/* -- Compute minimum number of moves and captures performed by this piece -- */

	xstd::maximize(m_requiredMoves, xstd::min(ValidSquares(m_possibleSquares), [&](Square square) { return m_distances[square]; }));
	xstd::maximize(m_requiredCaptures, xstd::min(ValidSquares(m_possibleSquares), [&](Square square) { return m_captures[square]; }));

	/* -- Update legal moves -- */

	m_moves.fill(Squares());
	for (const Piece& personality : m_personalities)
		for (Square from : AllSquares())
			m_moves[from] |= is(personality.m_promoted) ? personality.m_pawn.moves[from] : personality.m_moves[from];

	m_pawn.moves = m_moves;

	/* -- Update number of possible moves -- */

	m_nmoves = xstd::sum(m_moves, [](Squares squares) { return squares.count(); });
	for (const Piece& piece : m_personalities)
		if (!is(piece.m_promoted))
			m_nmoves += xstd::sum(piece.m_moves, [](Squares squares) { return squares.count(); });

	/* -- Get all squares the piece may have crossed or stopped on -- */

	m_stops = xstd::merge(m_personalities, Squares(), [](const Piece& piece) { return piece.m_stops; });
	m_route = xstd::merge(m_personalities, Squares(), [](const Piece& piece) { return piece.m_route; });
	m_threats = xstd::merge(m_personalities, Squares(), [](const Piece& piece) { return piece.m_threats; });

	/* -- Update occupied squares -- */

	for (Square square : AllSquares())
	{
		for (bool loop = true; loop; )
		{
			loop = false;
			for (Square occupied : ValidSquares(m_occupied[square].squares))
			{
				for (Square other : ValidSquares(m_occupied[square].pieces[occupied]->m_occupied[occupied].squares))
				{
					if (!m_occupied[square].squares[other])
					{
						m_occupied[square].pieces[other] = m_occupied[square].pieces[occupied]->m_occupied[occupied].pieces[other];
						m_occupied[square].squares[other] = true;
						loop = true;
					}
				}
			}
		}
	}
}

/* -------------------------------------------------------------------------- */

void Piece::updateDistances()
{
	if (is(m_promoted))
	{
		m_pawn.distances = computeDistances(m_initialSquare, Nowhere, true);
		xstd::maximize(m_distances, computeDistances(m_promotionSquares, m_pawn.distances));
	}
	else
	{
		const bool castling = (m_castlingSquare != Nowhere) && xstd::any_of(AllCastlingSides(), [&](CastlingSide side) { return is(m_castling[side]); });
		const auto distances = computeDistances(castling ? m_castlingSquare : m_initialSquare, castling ? Nowhere : m_castlingSquare, false);
		xstd::maximize(m_distances, distances);
	}
}

/* -------------------------------------------------------------------------- */

void Piece::updateDistancesTo()
{
	m_rdistances = computeDistancesTo(m_possibleSquares, false);
	if (is(m_promoted))
		m_pawn.rdistances = computeDistancesTo(m_promotionSquares, m_rdistances);
}

/* -------------------------------------------------------------------------- */

void Piece::updateCaptures()
{
	if (is(m_promoted))
	{
		if (m_pawn.xmoves)
			xstd::maximize(m_pawn.captures, computeCaptures(m_initialSquare, Nowhere, true));
		xstd::maximize(m_captures, computeCaptures(m_promotionSquares, m_pawn.captures));
	}
	else
	if (m_xmoves)
	{
		const bool castling = (m_castlingSquare != Nowhere) && xstd::any_of(AllCastlingSides(), [&](CastlingSide side) { return is(m_castling[side]); });
		const auto captures = computeCaptures(castling ? m_castlingSquare : m_initialSquare, castling ? Nowhere : m_castlingSquare, false);
		xstd::maximize(m_captures, captures);
	}
}

/* -------------------------------------------------------------------------- */

void Piece::updateCapturesTo()
{
	if (m_xmoves)
		m_rcaptures = computeCapturesTo(m_possibleSquares, false);
	if (is(m_promoted) && m_pawn.xmoves)
		m_pawn.rcaptures = computeCapturesTo(m_promotionSquares, true);
}

/* -------------------------------------------------------------------------- */

void Piece::updateConsequences()
{
	m_actions->clean();

	/* -- Promoted pawns required special treatment -- */

	if (maybe(m_promoted))
		return;

	/* -- Find mandatory moves, using a simple condition -- */

	const int minimumMoves = xstd::min(ValidSquares(m_possibleSquares), [&](Square square) { return m_distances[square]; });
	if (m_nmoves <= minimumMoves)
		for (Action& action : *m_actions)
			action.mandatory(true);

	/* -- Find unique moves, again using a simple condition -- */

	if ((m_species == Pawn) || (m_availableMoves <= minimumMoves + 1))
		for (Action& action : *m_actions)
			action.unique(true);

	/* -- Find unique last moves -- */

	const Squares beyond = xstd::merge(ValidSquares(m_possibleSquares), Squares(), [&](Square square) { return m_moves[square]; });
	if (!beyond)
		for (Action& action : *m_actions)
			if (m_possibleSquares[action.to()])
				action.unique(true);

	/* -- Basic consequences -- */

	for (Action& action : *m_actions)
	{
		const int requiredMoves = m_distances[action.from()] + 1 + m_rdistances[action.to()];
		if (requiredMoves > m_requiredMoves)
			action.consequences().updateRequiredMoves(*this, action.to(), requiredMoves);
	}
}

/* -------------------------------------------------------------------------- */

array<int, NumSquares> Piece::computeDistances(Square initial, Square castling, bool pawn) const
{
	/* -- Initialize distances -- */

	array<int, NumSquares> distances;
	distances.fill(Infinity);
	distances[initial] = 0;
	if (castling != Nowhere)
		distances[castling] = 0;

	/* -- Initialized square queue -- */

	Queue<Square, NumSquares> queue;
	queue.push(initial);
	if (castling != Nowhere)
		queue.push(castling);

	/* -- Loop until every reachable square has been handled -- */

	const ArrayOfSquares& moves = pawn ? m_pawn.moves : m_moves;

	while (!queue.empty())
	{
		const Square from = queue.front(); queue.pop();

		/* -- Handle every possible immediate destination -- */

		for (Square to : ValidSquares(moves[from]))
		{
			/* -- This square may have been attained by a quicker path -- */

			if (distances[to] < Infinity)
				continue;

			/* -- Set square distance -- */

			distances[to] = distances[from] + 1;

			/* -- Add it to queue of reachable queue -- */

			queue.push(to);
		}
	}

	/* -- Done -- */

	return distances;
}

/* -------------------------------------------------------------------------- */

array<int, NumSquares> Piece::computeDistances(Squares promotions, const array<int, NumSquares>& initial) const
{
	assert(is(m_promoted));

	/* -- Initialize distances and square queue -- */

	array<int, NumSquares> distances;
	distances.fill(Infinity);

	Queue<Square, NumSquares> queue;

	for (Square square : ValidSquares(promotions))
		if ((distances[square] = initial[square]) < Infinity)
			queue.push(square);

	queue.sort([&](Square lhs, Square rhs) { return distances[lhs] < distances[rhs]; });

	/* -- Loop until every reachable square has been handled -- */

	while (!queue.empty())
	{
		const Square from = queue.front(); queue.pop();

		/* -- Handle every possible immediate destination -- */

		for (Square to : ValidSquares(m_moves[from]))
		{
			/* -- This square may have been attained by a quicker path -- */

			if (distances[to] < Infinity)
				continue;

			/* -- Set square distance -- */

			distances[to] = distances[from] + 1;

			/* -- Add it to queue of reachable queue -- */

			queue.push(to);
		}
	}

	/* -- Done -- */

	return distances;
}

/* -------------------------------------------------------------------------- */

array<int, NumSquares> Piece::computeDistancesTo(Squares destinations, bool pawn) const
{
	/* -- Initialize distances and square queue -- */

	array<int, NumSquares> distances;
	Queue<Square, NumSquares> squares;

	for (Square square : AllSquares())
		if ((distances[square] = (destinations[square] ? 0 : Infinity)) == 0)
			squares.push(square);

	/* -- Handle castling -- */

	if ((m_castlingSquare != Nowhere) && !distances[m_castlingSquare] && distances[m_initialSquare])
		if ((distances[m_initialSquare] = 0) == 0)
			squares.push(m_initialSquare);

	/* -- Loop until every reachable square has been handled -- */

	const ArrayOfSquares& moves = pawn ? m_pawn.moves : m_moves;

	while (!squares.empty())
	{
		const Square to = squares.front(); squares.pop();

		/* -- Handle every possible immediate destination -- */

		for (Square from : AllSquares())
		{
			/* -- Skip illegal moves -- */

			if (!moves[from][to])
				continue;

			/* -- This square may have been attained by a quicker path -- */

			if (distances[from] < Infinity)
				continue;

			/* -- Set square distance -- */

			distances[from] = distances[to] + 1;

			/* -- Add it to queue of reachable squares -- */

			squares.push(from);

			/* -- Handle castling -- */

			if (from == m_castlingSquare)
			{
				if (distances[m_initialSquare] < Infinity)
					continue;

				distances[m_initialSquare] = distances[m_castlingSquare];
				squares.push(m_initialSquare);
			}
		}
	}

	/* -- Done -- */

	return distances;
}

/* -------------------------------------------------------------------------- */

array<int, NumSquares> Piece::computeDistancesTo(Squares promotions, const array<int, NumSquares>& initial) const
{
	/* -- Initialize distances and square queue -- */

	array<int, NumSquares> distances;
	Queue<Square, NumSquares> squares;

	for (Square square : AllSquares())
		if ((distances[square] = (promotions[square] ? initial[square] : Infinity)) < Infinity)
			squares.push(square);

	squares.sort([&](Square lhs, Square rhs) { return distances[lhs] < distances[rhs]; });

	/* -- Loop until every reachable square has been handled -- */

	while (!squares.empty())
	{
		const Square to = squares.front(); squares.pop();

		/* -- Handle every possible immediate destination -- */

		for (Square from : AllSquares())
		{
			/* -- Skip illegal moves -- */

			if (!m_pawn.moves[from][to])
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

array<int, NumSquares> Piece::computeDistancesTo(Squares destinations, const Piece& blocker, Square obstruction) const
{
	const bool enemies = (m_color != blocker.m_color);

	/* -- Early exit if the blocker is ourself -- */

	if (&blocker == this)
		return computeDistancesTo(destinations, false);

	/* -- Initialize distances and square queue -- */

	array<int, NumSquares> distances;
	Queue<Square, NumSquares> squares;

	for (Square square : AllSquares())
		if ((distances[square] = (destinations[square] && (square != obstruction)) ? 0 : Infinity) == 0)
			squares.push(square);

	if ((m_castlingSquare != Nowhere) && !distances[m_castlingSquare] && distances[m_initialSquare])
		if (!(*m_constraints)[m_initialSquare][m_castlingSquare][obstruction])
			if ((distances[m_initialSquare] = 0) == 0)
				squares.push(m_initialSquare);

	/* -- Loop until every reachable square has been handled -- */

	while (!squares.empty())
	{
		const Square to = squares.front(); squares.pop();

		/* -- Handle every possible immediate destination -- */

		for (Square from : AllSquares())
		{
			/* -- Skip illegal moves -- */

			if (!m_moves[from][to])
				continue;

			/* -- Skip obstructed moves -- */

			if (obstruction == from)
				continue;

			if ((*m_constraints)[from][to][obstruction])
				continue;

			if (m_royal && enemies)
				if ((*blocker.m_checks)[obstruction][to])
					continue;

			if (blocker.m_royal && enemies)
				if ((*m_checks)[from][obstruction])
					continue;

			/* -- This square may have been attained by a quicker path -- */

			if (distances[from] < Infinity)
				continue;

			/* -- Set square distance -- */

			distances[from] = distances[to] + 1;

			/* -- Add it to queue of reachable squares -- */

			squares.push(from);

			/* -- Handle castling -- */

			if (from == m_castlingSquare)
			{
				if (distances[m_initialSquare] < Infinity)
					continue;

				if ((*m_constraints)[m_initialSquare][m_castlingSquare][obstruction])
					continue;

				distances[m_initialSquare] = distances[m_castlingSquare];
				squares.push(m_initialSquare);
			}
		}
	}

	/* -- Done -- */

	return distances;
}

/* -------------------------------------------------------------------------- */

array<int, NumSquares> Piece::computeCaptures(Square initial, Square castling, bool pawn) const
{
	/* -- Initialize required captures -- */

	array<int, NumSquares> captures;
	captures.fill(Infinity);
	captures[initial] = 0;
	if (castling != Nowhere)
		captures[castling] = 0;

	/* -- Initialize squares to process -- */

	Squares candidates(initial);
	if (castling != Nowhere)
		candidates.set(castling);

	/* -- Loop until every reachable square has been handled, by order of number of captures -- */

	const ArrayOfSquares& moves = pawn ? m_pawn.moves : m_moves;
	const ArrayOfSquares *xmoves = pawn ? m_pawn.xmoves : m_xmoves;

	while (candidates)
	{
		Squares squares = candidates;
		candidates.reset();

		while (squares)
		{
			const Square from = squares.pop();

			/* -- Handle every possible immediate destination -- */

			for (Square to : ValidSquares(moves[from]))
			{
				/* -- This square may have been attained using less captures -- */

				const int required = captures[from] + (xmoves && (*xmoves)[from][to]);
				if (required >= captures[to])
					continue;

				/* -- Set required captures -- */

				captures[to] = required;

				/* -- Add it to queue of squares -- */

				if (captures[to] > captures[from])
					candidates.set(to);
				else
					squares.set(to);
			}
		}
	}

	/* -- Done -- */

	return captures;
}

/* -------------------------------------------------------------------------- */

array<int, NumSquares> Piece::computeCaptures(Squares promotions, const array<int, NumSquares>& initial) const
{
	/* -- Initialize required captures and square queue -- */

	array<int, NumSquares> captures;
	captures.fill(Infinity);

	Queue<Square, NumSquares> queue;

	for (Square square : ValidSquares(promotions))
		if ((captures[square] = initial[square]) < Infinity)
			queue.push(square);

	queue.sort([&](Square lhs, Square rhs) { return captures[lhs] < captures[rhs]; });

	/* -- Flood fill required captures -- */

	assert(!m_xmoves);

	while (!queue.empty())
	{
		Squares squares = queue.front(); queue.pop();

		while (squares)
		{
			Square from = squares.pop();

			for (Square to : ValidSquares(m_moves[from]))
			{
				if (captures[to] > captures[from])
				{
					captures[to] = captures[from];
					squares.set(to);
				}
			}
		}
	}

	/* -- Done -- */

	return captures;
}

/* -------------------------------------------------------------------------- */

array<int, NumSquares> Piece::computeCapturesTo(Squares destinations, bool pawn) const
{
	assert(pawn ? m_pawn.xmoves : m_xmoves);

	/* -- Initialize captures and square queue -- */

	array<int, NumSquares> captures;
	Queue<Square, 8 * NumSquares> squares;

	captures.fill(Infinity);

	for (Square square : AllSquares())
		if ((captures[square] = destinations[square] ? 0 : Infinity) < Infinity)
			squares.push(square);

	/* -- Loop until every reachable square has been handled -- */

	const ArrayOfSquares& moves = pawn ? m_pawn.moves : m_moves;
	const ArrayOfSquares *xmoves = pawn ? m_pawn.xmoves : m_xmoves;

	while (!squares.empty())
	{
		const Square to = squares.front(); squares.pop();

		/* -- Handle every possible immediate destination -- */

		for (Square from : AllSquares())
		{
			/* -- Skip illegal moves -- */

			if (!moves[from][to])
				continue;

			/* -- This square may have been attained using less captures -- */

			const int required = captures[to] + (*xmoves)[from][to];
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

	const bool friends = (states[0].piece.m_color == states[1].piece.m_color);
	const bool partners = friends && (states[0].piece.m_royal || states[1].piece.m_royal) && (states[0].teleportation || states[1].teleportation);

	/* -- Initial position -- */

	Position initial(states[0].piece.m_initialSquare, 0, states[1].piece.m_initialSquare, 0);
	queue.push(initial);
	cache.add(initial);

	/* -- Loop -- */

	for ( ; !queue.empty(); queue.pop())
	{
		const Position& position = queue.front();

		/* -- Check if we have reached our goal -- */

		if (states[0].piece.m_possibleSquares[position.squares[0]] && states[1].piece.m_possibleSquares[position.squares[1]])
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
			const Piece& piece = state.piece;
			const Piece& xpiece = xstate.piece;
			const Square from = position.squares[k];
			const Square other = position.squares[k ^ 1];

			/* -- Handle teleportation for rooks -- */

			if (state.teleportation && !position.moves[k] && (position.squares[k] == piece.m_initialSquare))
			{
				Square to = piece.m_castlingSquare;

				/* -- Teleportation could be blocked by other piece -- */

				const bool blocked = (*piece.m_constraints)[from][to][other];
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

			if (xpiece.m_royal && !friends && (*piece.m_checks)[from][other])
				continue;

			/* -- Loop over all moves -- */

			for (Square to : ValidSquares(piece.m_moves[from]))
			{
				Position next(k ? other : to, position.moves[0] + (k ^ 1), k ? to : other, position.moves[1] + (k ^ 0));

				/* -- Take castling into account -- */

				if (piece.m_royal && !position.moves[k] && partners)
					for (CastlingSide side : AllCastlingSides())
						if ((to == Castlings[piece.m_color][side].to) && (other == Castlings[piece.m_color][side].rook) && !position.moves[k ^ 1])
							next.squares[k ^ 1] = Castlings[piece.m_color][side].free;

				/* -- Continue if position was already reached before -- */

				if (cache.hit(next))
					continue;

				/* -- Move could be blocked by other pieces -- */

				bool blocked = (*piece.m_constraints)[from][to][other] || xpiece.m_occupied[other].squares[from];
				for (Square square : ValidSquares(xpiece.m_occupied[other].squares))
					if ((*piece.m_constraints)[from][to][square])
						blocked = true;

				if (blocked)
					continue;

				/* -- Reject move if it brings us to far away -- */

				if (piece.m_rdistances[to] > std::min(availableMoves, state.availableMoves - next.moves[k]))
					continue;

				/* -- Reject move if we move into check -- */

				if (piece.m_royal && !friends && (*xpiece.m_checks)[other][to])
					continue;

				/* -- Castling constraints -- */

				if (piece.m_royal && !friends && (from == piece.m_initialSquare))
					for (CastlingSide side : AllCastlingSides())
						if ((Castlings[piece.m_color][side].from == from) && (Castlings[piece.m_color][side].to == to))
							if (position.moves[k] || (*xpiece.m_checks)[other][from] || (*xpiece.m_checks)[other][Castlings[piece.m_color][side].free])
								continue;

				/* -- Safeguard if maximum queue size is insufficient -- */

				assert(!queue.full());
				if (queue.full())
				{
					states[0].requiredMoves = states[0].piece.m_requiredMoves;
					states[1].requiredMoves = states[1].piece.m_requiredMoves;
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

	if (states[0].piece.m_possibleSquares[states[0].square] && states[1].piece.m_possibleSquares[states[1].square])
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
		const Piece& piece = state.piece;
		const Piece& xpiece = xstate.piece;
		const Square from = state.square;
		const Square other = xstate.square;
		const bool friends = (piece.m_color == xpiece.m_color);

		/* -- Teleportation when castling -- */

		if (state.teleportation && !state.playedMoves)
		{
			const Square king = (xpiece.m_royal && friends) ? other : Nowhere;
			const Square pivot = std::find_if(Castlings[piece.m_color], Castlings[piece.m_color] + NumCastlingSides, [=](const Castling& castling) { return castling.rook == from; })->to;

			if ((king != Nowhere) ? (king == pivot) && (xstate.playedMoves == 1) : !(*piece.m_constraints)[piece.m_initialSquare][piece.m_castlingSquare][other])
			{
				assert(!piece.m_distances[piece.m_castlingSquare]);
				if (!piece.m_distances[piece.m_castlingSquare])
				{
					state.square = piece.m_castlingSquare;
					state.teleportation = false;

					const int myRequiredMoves = fullplay(states, availableMoves, maximumMoves, cache);
					if (myRequiredMoves <= maximumMoves)
					{
						state.squares[from][other] = true;
						xstate.squares[other][from] = true;

						state.distances[piece.m_castlingSquare] = 0;
					}

					xstd::minimize(requiredMoves, myRequiredMoves);

					state.teleportation = true;
					state.square = piece.m_initialSquare;
				}
			}
		}

		/* -- Check if there are any moves left for this piece -- */

		if (state.availableMoves <= 0)
			continue;

		/* -- Check that the enemy is not in check -- */

		if (xpiece.m_royal && !friends && (*piece.m_checks)[from][other])
			continue;

		/* -- Loop over all moves -- */

		for (Square to : ValidSquares(piece.m_moves[from]))
		{
			/* -- Move could be blocked by other pieces -- */

			bool blocked = (*piece.m_constraints)[from][to][other] || xpiece.m_occupied[other].squares[from];
			for (Square square : ValidSquares(xpiece.m_occupied[other].squares))
				if ((*piece.m_constraints)[from][to][square])
					blocked = true;

			if (blocked)
				continue;

			/* -- Reject move if it brings us to far away -- */

			if (1 + piece.m_rdistances[to] > std::min(availableMoves, state.availableMoves))
				continue;

			/* -- Reject move if we have taken a shortcut -- */

			if (state.playedMoves + 1 < piece.m_distances[to])
				if (!invalidate || ((*invalidate = true), true))
					continue;

			/* -- Reject move if we move into check -- */

			if (piece.m_royal && !friends && (*xpiece.m_checks)[other][to])
				continue;

			/* -- Castling constraints -- */

			if (piece.m_royal && !friends && (from == piece.m_initialSquare))
				for (CastlingSide side : AllCastlingSides())
					if ((Castlings[piece.m_color][side].from == from) && (Castlings[piece.m_color][side].to == to))
						if (state.playedMoves || (*xpiece.m_checks)[other][from] || (*xpiece.m_checks)[other][Castlings[piece.m_color][side].free])
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

}
