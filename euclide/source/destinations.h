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

		int updateRequiredMoves(const Board& board, const Castling& castling);
		int updateRequiredCaptures(const Board& board);

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
		Destinations();
		Destinations& operator+=(const Destination& destination);

		void updateRequiredMoves(const Board& board, const Castling& castling);
		void updateRequiredCaptures(const Board& board);

		bool setManSquare(Man man, Square square, bool captured);
		bool setAvailableMoves(const array<int, NumMen>& availableMovesByMan, const array<int, NumSquares>& availableMovesByOccupiedSquare, const array<int, NumSquares>& availableMovesByUnoccupiedSquare);
		bool setAvailableCaptures(const array<int, NumMen>& availableCapturesByMan, const array<int, NumSquares>& availableCapturesByOccupiedSquare, const array<int, NumSquares>& availableCapturesByUnoccupiedSquare);
		
	public :
		inline const array<int, NumMen>& getRequiredMovesByMan() const
			{ return requiredMovesByMan; }
		inline const array<int, NumSquares>& getRequiredMovesBySquare() const
			{ return requiredMovesBySquare; }
		inline const array<int, NumSquares>& getRequiredMovesBySquare(bool captured) const
			{ return captured ? requiredMovesByUnoccupiedSquare : requiredMovesByOccupiedSquare; }

		inline const array<int, NumMen>& getRequiredCapturesByMan() const
			{ return requiredCapturesByMan; }
		inline const array<int, NumSquares>& getRequiredCapturesBySquare() const
			{ return requiredCapturesBySquare; }
		inline const array<int, NumSquares>& getRequiredCapturesBySquare(bool captured) const
			{ return captured ? requiredCapturesByUnoccupiedSquare : requiredCapturesByOccupiedSquare; }

	protected :
		template <class Predicate>
		bool remove(Predicate predicate);

		template <class ForwardIterator, class Predicate>
		ForwardIterator remove_if(ForwardIterator first, ForwardIterator last, Predicate predicate);

	private :
		array<int, NumMen> requiredMovesByMan;
		array<int, NumSquares> requiredMovesBySquare;
		array<int, NumSquares> requiredMovesByOccupiedSquare;
		array<int, NumSquares> requiredMovesByUnoccupiedSquare;

		array<int, NumMen> requiredCapturesByMan;
		array<int, NumSquares> requiredCapturesBySquare;
		array<int, NumSquares> requiredCapturesByOccupiedSquare;
		array<int, NumSquares> requiredCapturesByUnoccupiedSquare;
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

template <class Predicate>
bool Destinations::remove(Predicate predicate)
{
	iterator last = remove_if(begin(), end(), predicate);
	if (last == end())
		return false;

	erase(last, end());
	return true;
}

/* -------------------------------------------------------------------------- */

template <class ForwardIterator, class Predicate>
ForwardIterator Destinations::remove_if(ForwardIterator first, ForwardIterator last, Predicate predicate)
{
	return std::remove_if(first, last, predicate);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

}

#endif
