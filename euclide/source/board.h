#ifndef __EUCLIDE_BOARD_H
#define __EUCLIDE_BOARD_H

#include "includes.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

class Obstructions
{
	public :
		Obstructions(Superman superman, Color color, Square square, int movements[NumSquares][NumSquares]);
		~Obstructions();

		void block(bool soft) const;
		void unblock(bool soft) const;

		void optimize();

	private :
		int numHardObstructions;
		int numSoftObstructions;

		int **obstructions;
};

/* -------------------------------------------------------------------------- */

class Movements
{
	public :
		Movements(Superman superman, Color color);
		~Movements();

		int distance(Square square) const;
		int captures(Square square) const;

		int distance(Square from, Square to) const;
		int captures(Square from, Square to) const;

		int getCaptures(Square from, Square to, vector<Squares>& captures) const;

		void block(Square square, bool captured);
		void unblock(Square square, bool captured);
		void optimize();

		bool locked() const;

	protected :
		int *distances(Square square, int distances[NumSquares]) const;
		int *captures(Square square, int captures[NumSquares]) const;

	private :
		int movements[NumSquares][NumSquares];
		int possibilities;

		Superman superman;
		Glyph glyph;
		Color color;
		
		bool hybrid;

		int initialDistances[NumSquares];
		int initialCaptures[NumSquares];

		Obstructions *obstructions[NumSquares];
};

/* -------------------------------------------------------------------------- */

class Board
{
	public :
		Board();
		~Board();

		int distance(Man man, Superman superman, Color color, Square from, Square to) const;
		int distance(Man man, Superman superman, Color color, Square to, const Castling& castling) const;
		int distance(Man man, Superman superman, Color color, Square to) const;

		int captures(Man man, Superman superman, Color color, Square from, Square to) const;
		int captures(Man man, Superman superman, Color color, Square to) const;

		int getCaptures(Glyph glyph, Square from, Square to, vector<Squares>& captures) const;
		int getCaptures(Man man, Superman superman, Color color, Square from, Square to, vector<Squares>& captures) const;

		void block(Glyph glyph, Square square, bool captured = false);
		void block(Glyph glyph, Square from, Square to, bool captured = false);
		void unblock(Glyph glyph, Square square, bool captured = false);

		bool lock(Man man, Color color);

	private :
		Movements *movements[NumSupermen][NumColors];

		bool locks[NumMen][NumColors];
		bool modified;
};

/* -------------------------------------------------------------------------- */

}

#endif
