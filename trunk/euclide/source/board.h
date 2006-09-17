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
		int distance(Superman superman, Color color, Square from, Square to) const;
		int distance(Man man, Superman superman, Color color, Square from, Square to) const;
		int distance(Man man, Superman superman, Color color, Square to, const Castling& castling) const;

		int captures(Glyph glyph, Square from, Square to) const;
		int captures(Superman superman, Color color, Square from, Square to) const;
		int captures(Man man, Superman superman, Color color, Square from, Square to) const;
		int captures(Man man, Superman superman, Color color, Square to) const;

		int idistance(Man man, Superman superman, Color color, Square to) const;
		int icaptures(Man man, Superman superman, Color color, Square to) const;

		int captures(Glyph glyph, Square from, Square to, vector<bitset<NumSquares> >& captures) const;
		int captures(Man man, Superman superman, Color color, Square from, Square to, vector<bitset<NumSquares> >& captures) const;

		void block(Glyph glyph, Square square, bool captured = false);
		void block(Glyph glyph, Square from, Square to, bool captured = false);
		void unblock(Glyph glyph, Square square, bool captured = false);

		bool lock(Man man, Color color);

	protected :
		static int distance(const int movements[NumSquares][NumSquares], Square from, Square to);
		static int captures(const int movements[NumSquares][NumSquares], const bool captures[NumSquares][NumSquares], Square from, Square to);

	private :
		int movements[NumGlyphs][NumSquares][NumSquares];
		int **obstructions[NumGlyphs][NumSquares];
		int **checks[NumGlyphs][NumSquares];
		
		bool locks[NumMen][NumColors];
		bool empty;
};

/* -------------------------------------------------------------------------- */

}

#endif
