#ifndef __EUCLIDE_PROBLEM_H
#define __EUCLIDE_PROBLEM_H

#include "includes.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */

class Problem
{
	public:
		Problem();
		Problem(const EUCLIDE_Problem& problem);

	public:
		inline const array<Glyph, NumSquares>& initialPosition() const
			{ return m_initialPosition; }
		inline const array<Glyph, NumSquares>& diagramPosition() const
			{ return m_diagramPosition; }
		inline Glyph initialPosition(Square square) const
			{ return m_initialPosition[square]; }
		inline Glyph diagramPosition(Square square) const
			{ return m_diagramPosition[square]; }
		inline const array<Species, NumGlyphs> pieces() const
			{ return m_pieces; }
		inline Species piece(Glyph glyph) const
			{ return m_pieces[glyph]; }
		inline bool enpassant(Glyph glyph) const
			{ return m_pieces[glyph] == Pawn; }
		inline Variant variant() const
			{ return m_variant; }

		inline int moves() const
			{ return m_moves[White] + m_moves[Black]; }
		inline int moves(Color color) const
			{ return m_moves[color]; }
		inline Color turn() const
			{ return m_turn; }

		inline const matrix<bool, NumColors, NumCastlingSides>& castlings() const
			{ return m_castlings; }
		inline bool castling(Color color, CastlingSide side) const
			{ return m_castlings[color][side]; }

		inline int initialPieces(Color color) const
			{ return m_initialPieces[color]; }
		inline int diagramPieces(Color color) const
			{ return m_diagramPieces[color]; }
		inline int capturedPieces(Color color) const
			{ return m_initialPieces[color] - m_diagramPieces[color]; }

	private:
		array<Glyph, NumSquares> m_initialPosition;               /**< Initial position. */
		array<Glyph, NumSquares> m_diagramPosition;               /**< Diagram position to be reached from the initial position. */
		array<Species, NumGlyphs> m_pieces;                       /**< Fairy pieces. */
		Variant m_variant;                                        /**< Chess variant. */

		array<int, NumColors> m_moves;                            /**< Available moves to reach the diagram position from the initial one. */
		Color m_turn;                                             /**< Whose side it is to move. */

		matrix<bool, NumColors, NumCastlingSides> m_castlings;    /**< Castling rights. */

		array<int, NumColors> m_initialPieces;                    /**< Number of pieces in initial position. */
		array<int, NumColors> m_diagramPieces;                    /**< Number of pieces in diagram position. */
};

/* -------------------------------------------------------------------------- */

}

#endif
