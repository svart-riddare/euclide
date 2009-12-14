#ifndef __EUCLIDE_MOVES_H
#define __EUCLIDE_MOVES_H

#include "includes.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

class Move
{
	public :
		Move();
		Move(Square from, Square to, Superman superman, Color color, int moves = infinity);
		void initialize(Square from, Square to, Superman superman, Color color, int moves = infinity);
		
		void invalidate();

		void block();
		void unblock();

	public :
		inline Square from() const
			{ return _from; }		
		inline Square to() const
			{ return _to; }
		inline Color color() const
			{ return _color; }		
		inline Glyph glyph() const
			{ return _glyph; }		
		inline Superman superman() const  
			{ return _superman; }

	public :
		inline bool possible() const
			{ return (_obstructions == 0) ? true : false; }
		inline tribool capture() const
			{ return _capture; }
		inline tribool mandatory() const
			{ return _mandatory; }
	
	public :
		inline bool isFromSquare(Square square) const
			{ return _from == square; }
		inline bool isToSquare(Square square) const
			{ return _to == square; }
		inline bool isColor(Color color) const
			{ return _color == color; }
		inline bool isGlyph(Glyph glyph) const
			{ return _glyph == glyph; }
		inline bool isSuperman(Superman superman) const
			{ return _superman == superman; }

	private :
		Square _from;                    /**< Departure square. */
		Square _to;                      /**< Arrival square. */
		Color _color;                    /**< Moving piece color. */
		Glyph _glyph;                    /**< Moving piece figure. */ 
		Superman _superman;              /**< Moving piece. */

		int _obstructions;               /**< Obstruction count. If zero, the move is possible. If positive, the move is not possible. If infinity, the move is not legal or must not be played to be able to solve the problem. */

		tribool _capture;                /**< True if move requires to capture another piece, false if moves can not be a capture and indeterminate otherwise. */
		tribool _mandatory;              /**< True if move is mandatory to solve the problem. */

		int _earliest;                   /**< Earliest time at which this move can be played in proof game. Move numbering starts from 1. */
		int _latest;                     /**< Latest time at which this move can be played in proof game. Move numbering starts from 1. */

		Squares _squares;                /**< Intermediate squares which should be free for this move to be played. */
};

/* -------------------------------------------------------------------------- */

typedef vector_ptr<Move> Moves;

/* -------------------------------------------------------------------------- */

}

#endif
