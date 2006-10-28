#ifndef __EUCLIDE_TARGETS_H
#define __EUCLIDE_TARGETS_H

#include "includes.h"
#include "destinations.h"

namespace euclide
{

class Pieces;

/* -------------------------------------------------------------------------- */

class Target : public vector<Destination>
{
	public:
		typedef std::pair<const Target *, int> Cause;

	public :
		Target(Glyph glyph, Square square);
		Target(Color color, const Squares& squares);

		int computeRequiredMoves(const Board& board, const Castling& castling);
		int computeRequiredCaptures(const Board& board);

		int updateRequiredMoves();
		int updateRequiredCaptures();
		const Men& updatePossibleMen();
		const Squares& updatePossibleSquares();

		bool setPossibleMen(const Men& men);
		bool setPossibleSquares(const Squares& squares);
		bool setAvailableMoves(int numAvailableMoves);
		bool setAvailableCaptures(int numAvailableCaptures);

		void setCause(const Cause& cause);

	public :
		inline bool isOccupied() const
			{ return (_glyph != NoGlyph); }
		inline bool isGeneric() const
			{ return (_glyph == NoGlyph) && !_cause.first; }

	public :
		inline Man man() const
			{ return _man; }
		inline Glyph glyph() const
			{ return _glyph; }
		inline Color color() const
			{ return _color; }
		inline Square square() const
			{ return _square; }

		inline const Squares& squares() const
			{ return _squares; }
		inline const Men& men() const
			{ return _men; }
		inline int candidates() const
			{ return (int)_men.count(); }

		inline bool isMan(Man man) const
			{ return _men[man]; }
		inline bool isSquare(Square square) const
			{ return _squares[square]; }

		inline const Cause& cause() const
			{ return _cause; }

	public :
		inline int getRequiredMoves() const
			{ return requiredMoves; }
		inline int getRequiredCaptures() const
			{ return requiredCaptures; }

	private :
		Man _man;
		Glyph _glyph;
		Color _color;
		Square _square;

		Squares _squares;
		Men _men;

		Cause _cause;

	private :
		int requiredMoves;
		int requiredCaptures;
};

/* -------------------------------------------------------------------------- */

typedef vector<Target> Targets;

/* -------------------------------------------------------------------------- */

}

#endif
