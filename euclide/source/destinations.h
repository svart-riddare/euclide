#ifndef __EUCLIDE_DESTINATIONS_H
#define __EUCLIDE_DESTINATIONS_H

#include "includes.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

class Destination
{
	public :
		Destination(Square square, Color color, Man man, Superman superman, bool captured);

		int computeRequiredMoves(const Board& board, const Castling& castling);
		int computeRequiredCaptures(const Board& board);

	public :
		inline Square square() const
			{ return _square; }		
		inline Color color() const
			{ return _color; }		
		inline Man man() const           
			{ return _man; }		
		inline Superman superman() const  
			{ return _superman; }
		inline bool captured() const
			{ return _captured; }

	public :
		inline bool isSquare(Square square) const
			{ return _square == square; }
		inline bool isColor(Color color) const
			{ return _color == color; }
		inline bool isMan(Man man) const
			{ return _man == man; }
		inline bool isSuperman(Superman superman) const
			{ return _superman == superman; }

	public :
		inline int getRequiredMoves() const
			{ return requiredMoves; }
		inline int getRequiredCaptures() const
			{ return requiredCaptures; }

		static inline bool lessMoves(const Destination& destinationA, const Destination& destinationB)
			{ return destinationA.requiredMoves < destinationB.requiredMoves; }
		static inline bool lessCaptures(const Destination& destinationA, const Destination& destinationB)
			{ return destinationA.requiredCaptures < destinationB.requiredCaptures; }

	public :
		inline bool isInMoves(int numAvailableMoves) const
			{ return requiredMoves <= numAvailableMoves; }
		inline bool isInCaptures(int numAvailableCaptures) const
			{ return requiredCaptures <= numAvailableCaptures; }
		inline bool isInSquares(const Squares& squares) const
			{ return squares[_square]; }
		inline bool isInMen(const Men& men) const
			{ return men[_man]; }

	private :
		Square _square;
		Color _color;
		Man _man;
		Superman _superman;
		bool _captured;

	private :
		int requiredMoves;
		int requiredCaptures;
};

/* -------------------------------------------------------------------------- */

typedef vector<Destination> Destinations;

/* -------------------------------------------------------------------------- */

}

#endif
