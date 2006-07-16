#ifndef __EUCLIDE_FINAL_SQUARES_H
#define __EUCLIDE_FINAL_SQUARES_H

#include "includes.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

class FinalSquare
{
	public :
		FinalSquare(Square square, Man man, Superman superman, bool captured);

		int computeRequiredMoves(const Board& board, Color color, const Castling& castling);
		int getRequiredMoves() const;

		operator Square() const;
		operator Man() const;
		operator Superman() const;

		bool isSquare(Square square, bool captured) const;
		bool isSquare(Square square) const;
		bool isEmpty() const;

	private :
		Square square;
		Man man;
		Superman superman;

		bool captured;

		int requiredMoves;
		int requiredCaptures;
};

/* -------------------------------------------------------------------------- */

typedef vector<FinalSquare> finalsquares_t;

/* -------------------------------------------------------------------------- */

class FinalSquares
{
	public :
		FinalSquares();
		FinalSquares& operator+=(const FinalSquare& finalSquare);

		int computeRequiredMoves(const Board& board, Color color, const Castling& castling);
		int getRequiredMoves(array<int, NumSquares>& squares) const;
		int getRequiredMoves() const;

		operator const finalsquares_t&() const;

		bool applyDeduction(Square square, bool captured);
		bool applyDeduction(int availableMoves);
		bool applyDeduction();

	private :
		finalsquares_t squares;
		tribool captured;

		int requiredMoves;
		int requiredCaptures;

		Castling castling;
};

/* -------------------------------------------------------------------------- */

}

#endif
