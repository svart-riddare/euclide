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

class Destinations : public vector<Destination>
{
	public :
		Destinations(const Problem& problem, Color color);

		void computeRequiredMoves(const Board& board, const Castling& castling);
		void computeRequiredCaptures(const Board& board);
		void updateRequiredMoves();
		void updateRequiredCaptures();

		bool setShrines(const Squares& squares);
		bool setManSquare(Man man, Square square, bool captured);
		bool setMenSquares(const array<Squares, NumMen>& squares, const array<Squares, NumMen>& shrines);
		bool setAvailableMoves(const array<int, NumMen>& availableMovesByMan, const array<int, NumSquares>& availableMovesBySquare, const array<int, NumSquares>& availableMovesByShrine);
		bool setAvailableCaptures(const array<int, NumMen>& availableCapturesByMan, const array<int, NumSquares>& availableCapturesBySquare, const array<int, NumSquares>& availableCapturesByShrine);
		
	public :
		inline int getRequiredMoves() const
			{ return requiredMoves; }
		inline int getRequiredCaptures() const
			{ return requiredCaptures; }

		inline const array<int, NumMen>& getRequiredMovesByMan() const
			{ return requiredMovesByMan; }
		inline const array<int, NumSquares>& getRequiredMovesBySquare(bool captured) const
			{ return captured ? requiredMovesByShrine : requiredMovesBySquare; }

		inline const array<int, NumMen>& getRequiredCapturesByMan() const
			{ return requiredCapturesByMan; }
		inline const array<int, NumSquares>& getRequiredCapturesBySquare(bool captured) const
			{ return captured ? requiredCapturesByShrine : requiredCapturesBySquare; }

	protected :
		template <class Predicate>
		bool remove(Predicate predicate);

	protected :
		static bool isDestinationCompatible(const Destination& destination, const Men& men, const Squares& squares, const Squares& shrines);
		static bool isDestinationPossible(const Destination& destination, const array<Squares, NumMen>& squares, const array<Squares, NumMen>& shrines);
		static bool isEnoughMovesForDestination(const Destination& destination, const array<int, NumMen>& availableMovesByMan, const array<int, NumSquares>& availableMovesBySquare, const array<int, NumSquares>& availableMovesByShrine);
		static bool isEnoughCapturesForDestination(const Destination& destination, const array<int, NumMen>& availableCapturesByMan, const array<int, NumSquares>& availableCapturesBySquare, const array<int, NumSquares>& availableCapturesByShrine);

	private :
		int requiredMoves;
		array<int, NumMen> requiredMovesByMan;
		array<int, NumSquares> requiredMovesBySquare;
		array<int, NumSquares> requiredMovesByShrine;

		int requiredCaptures;
		array<int, NumMen> requiredCapturesByMan;
		array<int, NumSquares> requiredCapturesBySquare;
		array<int, NumSquares> requiredCapturesByShrine;
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

template <class Predicate>
bool Destinations::remove(Predicate predicate)
{
	iterator last = std::remove_if(begin(), end(), !predicate);
	if (last == end())
		return false;

	erase(last, end());

	updateRequiredMoves();
	updateRequiredCaptures();

	return true;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

}

#endif
