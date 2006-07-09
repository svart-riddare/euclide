#ifndef __EUCLIDE_FINAL_SQUARES_H
#define __EUCLIDE_FINAL_SQUARES_H

#include "includes.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

class FinalSquare
{
	public :
		FinalSquare();
		FinalSquare(Square square, Man man, bool captured);

		int computeRequiredMoves(const Board& board, Man man, Color color, const Castling& castling);
		int getRequiredMoves() const;

		operator Square() const;
		operator Man() const;

		bool empty() const;

	private :
		Square square;
		Man man;

		bool captured;

		int requiredMoves;
		int requiredCaptures;
};

/* -------------------------------------------------------------------------- */

class FinalSquares
{
	public :
		FinalSquares();

		FinalSquares& operator+=(const FinalSquare& finalSquare);
		bool operator=(Square square);

		int computeRequiredMoves(const Board& board, Man man, Color color, const Castling& castling);
		int getRequiredMoves() const;

		operator const vector<FinalSquare>&() const;
		
	private :
		vector<FinalSquare> squares;
		tribool captured;

		int requiredMoves;
		int requiredCaptures;

		Castling castling;
};

/* -------------------------------------------------------------------------- */

}

#endif
