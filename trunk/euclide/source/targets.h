#ifndef __EUCLIDE_TARGETS_H
#define __EUCLIDE_TARGETS_H

#include "includes.h"
#include "destinations.h"

namespace euclide
{

class Pieces;

/* -------------------------------------------------------------------------- */

class Target
{
	public:
		typedef std::pair<const Target *, int> Cause;

	public :
		Target(Glyph glyph, Square square);
		Target(Glyph glyph, const Squares& squares);

		void setCause(const Cause& cause);
		void updateMen(const Men& men);
		void updateMen(const array<Men, NumSquares>& men);
		void updateSquares(const Squares& squares);
		int updateRequiredMoves(const array<int, NumSquares>& requiredMoves);
		int updateRequiredCaptures(const array<int, NumSquares>& requiredCaptures);

		bool isTargetOf(const Destination& destination) const;
		bool isCausedBy(const Cause& cause) const;

	public :
		inline bool isOccupied() const
			{ return (_glyph != NoGlyph); }

	public :
		inline Glyph glyph() const
			{ return _glyph; }
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

	public :
		inline int getRequiredMoves() const
			{ return requiredMoves; }
		inline int getRequiredCaptures() const
			{ return requiredCaptures; }

	private :
		Glyph _glyph;
		Square _square;

		Squares _squares;
		Men _men;

		Cause _cause;
		
		int requiredMoves;
		int requiredCaptures;
};

/* -------------------------------------------------------------------------- */

class Targets : public vector<Target>
{
	public :
		Targets(const Problem& problem, Color color);

		void refine(const Board& board, const Pieces& pieces);
		void update(const Destinations& destinations);

	public :
		inline int getRequiredMoves() const
			{ return requiredMoves; }
		inline int getRequiredCaptures() const
			{ return requiredCaptures; }

	public :
		inline const Squares& getCaptures() const
			{ return captures; }

	private :
		Color color;

		int requiredMoves;
		int requiredCaptures;

		Squares captures;
};

/* -------------------------------------------------------------------------- */

}

#endif
