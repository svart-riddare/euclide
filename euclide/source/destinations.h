#ifndef __EUCLIDE_DESTINATIONS_H
#define __EUCLIDE_DESTINATIONS_H

#include "includes.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

/**
 * \class Destination
 * Possible final square for a given piece, including eventual promotion type
 * and whether it has been captured or not.
 */

/**
 * \class Destinations
 * List of possible \link Destination destinations\endlink.
 */

/* -------------------------------------------------------------------------- */

class Destination
{
	public :
		Destination(Square square, Color color, Man man, Superman superman, bool captured);
		Destination(const Destination& destination);

		int computeRequiredMoves(const Board& board);
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
		inline int requiredMoves() const
			{ return _requiredMoves; }
		inline int requiredCaptures() const
			{ return _requiredCaptures; }

		static inline bool lessMoves(const Destination& destinationA, const Destination& destinationB)
			{ return destinationA.requiredMoves() < destinationB.requiredMoves(); }
		static inline bool lessCaptures(const Destination& destinationA, const Destination& destinationB)
			{ return destinationA.requiredCaptures() < destinationB.requiredCaptures(); }

	public :
		inline bool isEnoughManMoves(const array<int, NumMen>& availableMoves) const
			{ return _requiredMoves <= availableMoves[_man]; }
		inline bool isEnoughManCaptures(const array<int, NumMen>& availableCaptures) const
			{ return _requiredCaptures <= availableCaptures[_man]; }
		inline bool isEnoughMoves(int availableMoves) const
			{ return _requiredMoves <= availableMoves; }
		inline bool isEnoughCaptures(int availableCaptures) const
			{ return _requiredCaptures <= availableCaptures; }
		inline bool isInSquares(const Squares& squares) const
			{ return squares[_square]; }
		inline bool isInMen(const Men& men) const
			{ return men[_man]; }

	private :
		Square _square;          /**< Destination final square. */
		Color _color;            /**< Piece's color. */
		Man _man;                /**< Piece. */
		Superman _superman;      /**< Promotion piece, if any. */
		bool _captured;          /**< True if piece has been captured on destination square. */

	private :
		int _requiredMoves;      /**< Required moves for this destination. */
		int _requiredCaptures;   /**< Required captures for this destination. */
};

/* -------------------------------------------------------------------------- */

typedef vector<Destination> Destinations;

/* -------------------------------------------------------------------------- */

}

#endif
