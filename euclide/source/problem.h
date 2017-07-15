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

	private :
		array<Glyph, NumSquares> _initialPosition;    /**< Initial position. */
		array<Glyph, NumSquares> _diagramPosition;    /**< Diagram position to be reached from the initial position. */
		array<Piece, NumGlyphs> _pieces;              /**< Fairy pieces. */
		Variant _variant;                             /**< Chess variant. */
		
		array<int, NumColors> _moves;                 /**< Available moves to reach the diagram position from the initial one. */
		Color _turn;                                  /**< Whose side it is to move. */

		bool _forbidWhiteKingSideCastling;            /**< White castling with king rook is forbidden. */
		bool _forbidWhiteQueenSideCastling;           /**< White castling with queen rook is forbidden. */
		bool _forbidBlackKingSideCastling;            /**< Black castling with king rook is forbidden. */
		bool _forbidBlackQueenSideCastling;           /**< Black castling with queen rook is forbidden. */
};

/* -------------------------------------------------------------------------- */

}

#endif
