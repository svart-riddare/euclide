#ifndef __EUCLIDE_BOARD_H
#define __EUCLIDE_BOARD_H

#include "includes.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

class Board
{
	public :
		Board();

		int distance(Glyph glyph, Square from, Square to) const;
		int distance(Superman superman, Color color, Square square, Square to) const;
		int distance(Man man, Superman superman, Color color, Square from, Square to) const;
		int distance(Man man, Superman superman, Color color, Square to, const Castling& castling) const;
		
		int idistance(Man man, Superman superman, Color color, Square to) const;
		int icaptures(Man man, Superman superman, Color color, Square to) const;

		void block(Glyph glyph, Square square);
		void block(Glyph glyph, Square from, Square to);
		void unblock(Glyph glyph, Square square);
		void unblock(Glyph glyph, Square from, Square to);

	protected :
		static int distance(const int blockedMovements[NumSquares][NumSquares], Square from, Square to);

	private :
		int blockedMovements[NumGlyphs][NumSquares][NumSquares];
		int blocks;
};

/* -------------------------------------------------------------------------- */

}

#endif
