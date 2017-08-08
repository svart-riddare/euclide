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
		inline Glyph initialPosition(Square square) const
			{ return _initialPosition[square]; }
		inline Glyph diagramPosition(Square square) const
			{ return _diagramPosition[square]; }
		inline Species piece(Glyph glyph) const
			{ return _pieces[glyph]; }
		inline Variant variant() const
			{ return _variant; }

		inline int moves(Color color) const
			{ return _moves[color]; }

		inline int initialPieces(Color color) const
			{ return _initialPieces[color]; }
		inline int diagramPieces(Color color) const
			{ return _diagramPieces[color]; }
		inline int capturedPieces(Color color) const
			{ return _initialPieces[color] - _diagramPieces[color]; }

	private :
		array<Glyph, NumSquares> _initialPosition;    /**< Initial position. */
		array<Glyph, NumSquares> _diagramPosition;    /**< Diagram position to be reached from the initial position. */
		array<Species, NumGlyphs> _pieces;            /**< Fairy pieces. */
		Variant _variant;                             /**< Chess variant. */
		
		array<int, NumColors> _moves;                 /**< Available moves to reach the diagram position from the initial one. */
		Color _turn;                                  /**< Whose side it is to move. */

		bool _forbidWhiteKingSideCastling;            /**< White castling with king rook is forbidden. */
		bool _forbidWhiteQueenSideCastling;           /**< White castling with queen rook is forbidden. */
		bool _forbidBlackKingSideCastling;            /**< Black castling with king rook is forbidden. */
		bool _forbidBlackQueenSideCastling;           /**< Black castling with queen rook is forbidden. */

		array<int, NumColors> _initialPieces;         /**< Number of pieces in initial position. */
		array<int, NumColors> _diagramPieces;         /**< Number of pieces in diagram position. */
};

/* -------------------------------------------------------------------------- */

}

#endif
