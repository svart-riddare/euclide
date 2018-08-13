#ifndef __EUCLIDE_PROBLEM_H
#define __EUCLIDE_PROBLEM_H

#include "includes.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */

class Problem
{
	public :
		Problem();
		Problem(const EUCLIDE_Problem& problem);

	public :
		inline const array<Glyph, NumSquares>& initialPosition() const
			{ return _initialPosition; }
		inline const array<Glyph, NumSquares>& diagramPosition() const
			{ return _diagramPosition; }
		inline Glyph initialPosition(Square square) const
			{ return _initialPosition[square]; }
		inline Glyph diagramPosition(Square square) const
			{ return _diagramPosition[square]; }
		inline Species piece(Glyph glyph) const
			{ return _pieces[glyph]; }
		inline Variant variant() const
			{ return _variant; }

		inline int moves() const
			{ return _moves[White] + _moves[Black]; }
		inline int moves(Color color) const
			{ return _moves[color]; }
		inline Color turn() const
			{ return _turn; }

		inline bool castling(Color color, CastlingSide side) const
			{ return _castlings[color][side]; }

		inline int initialPieces(Color color) const
			{ return _initialPieces[color]; }
		inline int diagramPieces(Color color) const
			{ return _diagramPieces[color]; }
		inline int capturedPieces(Color color) const
			{ return _initialPieces[color] - _diagramPieces[color]; }

	private :
		array<Glyph, NumSquares> _initialPosition;               /**< Initial position. */
		array<Glyph, NumSquares> _diagramPosition;               /**< Diagram position to be reached from the initial position. */
		array<Species, NumGlyphs> _pieces;                       /**< Fairy pieces. */
		Variant _variant;                                        /**< Chess variant. */
		
		array<int, NumColors> _moves;                            /**< Available moves to reach the diagram position from the initial one. */
		Color _turn;                                             /**< Whose side it is to move. */

		matrix<bool, NumColors, NumCastlingSides> _castlings;    /**< Castling rights. */

		array<int, NumColors> _initialPieces;                    /**< Number of pieces in initial position. */
		array<int, NumColors> _diagramPieces;                    /**< Number of pieces in diagram position. */
};

/* -------------------------------------------------------------------------- */

}

#endif
