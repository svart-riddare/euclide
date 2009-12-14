#ifndef __EUCLIDE_BOARD_H
#define __EUCLIDE_BOARD_H

#include "includes.h"

namespace euclide
{

class Position;
class Piece;

/* -------------------------------------------------------------------------- */

/**
 * \class Board
 * Set of \link Piece movements\endlink for each piece type that may be 
 * present on the board.
 */

/* -------------------------------------------------------------------------- */

class Board
{
	public :
		Board(int numWhiteMoves, int numBlackMoves);
		~Board();

		int moves() const;
		int moves(Color color) const;
		int moves(Superman superman, Color color) const;
		int moves(Man man, Superman superman, Color color) const;

		int distance(Man man, Superman superman, Color color, Square from, Square to) const;
		int distance(Man man, Superman superman, Color color, Square to) const;

		int captures(Man man, Superman superman, Color color, Square from, Square to) const;
		int captures(Man man, Superman superman, Color color, Square to) const;

		int getCaptures(Glyph glyph, Square from, Square to, vector<Squares>& captures) const;
		int getCaptures(Man man, Superman superman, Color color, Square from, Square to, vector<Squares>& captures) const;

		bool getUniquePath(Man man, Superman superman, Color color, Square to, vector<Square>& squares) const;

		void block(Superman superman, Color color, Square square, bool captured = false);
		void unblock(Superman superman, Color color, Square square, bool captured = false);
		void transblock(Superman superman, Color color, Square from, Square to, bool captured = false);
		void block(Man man, Superman superman, Color color);
		void block(Man man, Superman superman, Color color, const Squares& squares);

		void reduce(Man man, Superman superman, Color color, Square square, int availableMoves, int availableCaptures);
		void reduce(Man man, Superman superman, Color color, const Squares& squares, int availableMoves, int availableCaptures);
		void reduce(Man man, const Supermen& supermen, Color color, const Squares& squares, int availableMoves, int availableCaptures);
		
		void setCaptureSquares(Man man, Superman superman, Color color, const Squares& squares);
		
		void optimizeLevelOne(const Position& position, Color color, int availableMoves, int availableCaptures);
		void optimizeLevelTwo(const Position& whitePosition, const Position& blackPosition);
		void optimize(Color color);

	private :
		Piece *pieces[NumColors][NumSupermen];    /**< Movement descriptors for each piece type. */
		bool optimized;                           /**< True when pieces have been optimized and distances precomputed. */
};

/* -------------------------------------------------------------------------- */

}

#endif
