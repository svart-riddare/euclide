#include "conditions.h"
#include "problem.h"
#include "pieces.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */
/* -- PositionalCondition                                                  -- */
/* -------------------------------------------------------------------------- */

PositionalCondition::PositionalCondition(const Piece& piece, const Squares& squares)
	: m_piece(piece), m_squares(squares)
{
}

/* -------------------------------------------------------------------------- */

bool PositionalCondition::satisfied() const
{
	return m_squares[m_piece.state.square];
}

/* -------------------------------------------------------------------------- */
/* -- Conditions                                                           -- */
/* -------------------------------------------------------------------------- */

Conditions::Conditions(const Piece& piece, Square from, Square to)
	: m_piece(piece), m_from(from), m_to(to)
{
}

/* -------------------------------------------------------------------------- */

Conditions::~Conditions()
{
	for (Condition *condition : m_conditions)
		delete condition;
}

/* -------------------------------------------------------------------------- */

bool Conditions::satisfied() const
{
	for (const Condition *condition : m_conditions)
		if (!condition->satisfied())
			return false;

	return true;
}

/* -------------------------------------------------------------------------- */

void Conditions::add(Condition *condition)
{
	m_conditions.push_back(condition);
}

/* -------------------------------------------------------------------------- */
/* -- PieceConditions                                                      -- */
/* -------------------------------------------------------------------------- */

PieceConditions::PieceConditions(const Piece& piece)
	: m_piece(piece)
{
	m_conditions.reserve(piece.nmoves());

	for (Square from : AllSquares())
	{
		m_pointers[from].fill(nullptr);

		for (Square to : ValidSquares(piece.moves(from)))
		{
			m_conditions.emplace_back(piece, from, to);
			m_pointers[from][to] = &m_conditions.back();
		}
	}
}

/* -------------------------------------------------------------------------- */

}