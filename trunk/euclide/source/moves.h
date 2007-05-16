#ifndef __EUCLIDE_MOVES_H
#define __EUCLIDE_MOVES_H

#include "includes.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

class Move
{
	public :
		Move(Square from, Square to, Color color, Man man, Superman superman);

	public :
		inline Square from() const
			{ return _from; }		
		inline Square to() const
			{ return _to; }
		inline Color color() const
			{ return _color; }		
		inline Man man() const           
			{ return _man; }		
		inline Superman superman() const  
			{ return _superman; }
	
	public :
		inline bool isFromSquare(Square square) const
			{ return _from == square; }
		inline bool isToSquare(Square square) const
			{ return _to == square; }
		inline bool isColor(Color color) const
			{ return _color == color; }
		inline bool isMan(Man man) const
			{ return _man == man; }
		inline bool isSuperman(Superman superman) const
			{ return _superman == superman; }

	private :
		Square _from;
		Square _to;
		Color _color;
		Man _man;
		Superman _superman;
};

/* -------------------------------------------------------------------------- */

typedef vector<Move> Moves;

/* -------------------------------------------------------------------------- */

}

#endif
