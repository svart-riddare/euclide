#include "actions.h"
#include "problem.h"
#include "pieces.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */
/* -- Consequence                                                          -- */
/* -------------------------------------------------------------------------- */

Consequence::Consequence(const Action& action, const Piece& piece)
	: m_action(action), m_piece(piece)
{
	m_requiredMoves.fill(0);
}


/* -------------------------------------------------------------------------- */

void Consequence::updateRequiredMoves(Square square, int requiredMoves)
{
	xstd::maximize<short>(m_requiredMoves[square], requiredMoves);
}

/* -------------------------------------------------------------------------- */

void Consequence::updateRequiredMoves(const array<int, NumSquares>& requiredMoves)
{
	for (Square square : AllSquares())
		updateRequiredMoves(square, requiredMoves[square]);
}

/* -------------------------------------------------------------------------- */
/* -- Consequences                                                         -- */
/* -------------------------------------------------------------------------- */

Consequences::Consequences(const Action& action)
	: m_action(action)
{
	m_matrix.fill(nullptr);
}

/* -------------------------------------------------------------------------- */

void Consequences::updateRequiredMoves(const Piece& piece, Square square, int requiredMoves)
{
	Consequence *consequence = this->consequence(piece);
	consequence->updateRequiredMoves(square, requiredMoves);
}

/* -------------------------------------------------------------------------- */

void Consequences::updateRequiredMoves(const Piece& piece, const array<int, NumSquares>& requiredMoves)
{
	Consequence *consequence = this->consequence(piece);
	consequence->updateRequiredMoves(requiredMoves);
}

/* -------------------------------------------------------------------------- */

Consequence *Consequences::consequence(const Piece& piece)
{
	Consequence *consequence = m_matrix[piece.man()][piece.color()];
	if (!consequence)
	{
		m_consequences.emplace_back(m_action, piece);
		consequence = &m_consequences.back();

		m_matrix[piece.man()][piece.color()] = consequence;
	}

	return consequence;
}

/* -------------------------------------------------------------------------- */
/* -- Action                                                               -- */
/* -------------------------------------------------------------------------- */

Action::Action(const Piece& piece, Square from, Square to)
	: m_piece(piece), m_from(from), m_to(to), m_consequences(*this)
{
	m_mandatory = false;
	m_unique = false;
}

/* -------------------------------------------------------------------------- */

void Action::mandatory(bool mandatory)
{
	assert(mandatory || !m_mandatory);
	m_mandatory = mandatory;
}

/* -------------------------------------------------------------------------- */

void Action::unique(bool unique)
{
	assert(unique || !m_unique);
	m_unique = unique;
}

/* -------------------------------------------------------------------------- */
/* -- Actions                                                              -- */
/* -------------------------------------------------------------------------- */

Actions::Actions(const Piece& piece)
	: m_piece(piece)
{
	for (Square from : ValidSquares(piece.stops()))
		for (Square to : ValidSquares(piece.moves(from, false)))
			emplace_back(piece, from, to);

	m_actions.fill(nullptr);
	for (Action& action : *this)
		m_actions[action.from()][action.to()] = &action;
}

/* -------------------------------------------------------------------------- */

void Actions::clean()
{
	/* -- Remove actions corresponding to moves that may no longer be played -- */

	remove_if([&](const Action& action) -> bool {
		const bool ok = m_piece.move(action.from(), action.to(), false);
		if (!ok)
			m_actions[action.from()][action.to()] = nullptr;
		return !ok;
	});
}

/* -------------------------------------------------------------------------- */

}