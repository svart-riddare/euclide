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
		~Board();

		int distance(Glyph glyph, Square from, Square to) const;
		int distance(Superman superman, Color color, Square square, Square to) const;
		int distance(Man man, Superman superman, Color color, Square from, Square to) const;
		int distance(Man man, Superman superman, Color color, Square to, const Castling& castling) const;
		
		int idistance(Man man, Superman superman, Color color, Square to) const;
		int icaptures(Man man, Superman superman, Color color, Square to) const;

		void block(Glyph glyph, Square square, bool captured = false);
		void block(Glyph glyph, Square from, Square to, bool captured = false);
		void unblock(Glyph glyph, Square square, bool captured = false);

	protected :
		static int distance(const int movements[NumSquares][NumSquares], Square from, Square to);

	private :
		int movements[NumGlyphs][NumSquares][NumSquares];
		int **obstructions[NumGlyphs][NumSquares];
		int **checks[NumGlyphs][NumSquares];

		bool empty;
};

/* -------------------------------------------------------------------------- */

}

#endif
