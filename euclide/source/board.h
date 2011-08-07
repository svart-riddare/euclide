#ifndef __EUCLIDE_BOARD_H
#define __EUCLIDE_BOARD_H

#include "includes.h"

namespace euclide
{

class Assignations;
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
		Board(const Position& whitePosition, const Position& blackPosition, const Problem& problem);
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

	public :
		bool optimize(int level = 1, bool recursive = true);

	protected :
		void block(Superman superman, Color color, Square square, bool captured = false);
		void unblock(Superman superman, Color color, Square square, bool captured = false);
		void transblock(Superman superman, Color color, Square from, Square to, bool captured = false);
		
		void block(Man man, Superman superman, Color color);
		void block(Man man, const Supermen& supermen, Color color);
		void block(Man man, Superman superman, Color color, const Squares& squares);
		void block(Man man, const Supermen& supermen, Color color, const Squares& squares);

		void setPossibleSquares(Man man, const Supermen& supermen, Color color, const Squares& squares, tribool captured, int availableMoves, int availableCaptures);
		void setPossibleCaptures(Man man, Superman superman, Color color, const Squares& squares);

		void optimizeLevelOne();
		void optimizeLevelTwo();
		void optimizeLevelThree();

	private :
		class OptimizeLevelThreeItem
		{
			public :
				OptimizeLevelThreeItem() {}
				OptimizeLevelThreeItem(Man manA, Man manB, Color colorA, Color colorB, int movesA, int movesB, int assignedMoves, int availableMoves) : manA(manA), manB(manB), colorA(colorA), colorB(colorB), movesA(movesA), movesB(movesB), assignedMoves(assignedMoves), availableMoves(availableMoves)
					{ complexity = std::min(movesA, movesB) * std::max(movesA, movesB) * (availableMoves - assignedMoves) * (availableMoves - assignedMoves); }
				
				bool operator<(const OptimizeLevelThreeItem& item) const
					{ return complexity < item.complexity; }

			public :
				Man manA;
				Man manB;
				Color colorA;
				Color colorB;
				int movesA;
				int movesB;
	
				int assignedMoves;
				int availableMoves;

				int complexity;
		};
		
	private :
		Piece *_pieces[NumColors][NumSupermen];        /**< Movement descriptors for each piece type. */
		
		const Position *_positions[NumColors];         /**< Positional deductions. */
		const Problem *_problem;                       /**< Problem definition. */

		Assignations *_assignedMoves[NumColors];       /**< Assigned moves. */
		Assignations *_assignedCaptures[NumColors];    /**< Assigned captures. */
};

/* -------------------------------------------------------------------------- */

}

#endif
