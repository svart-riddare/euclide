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
	public :
		Target(Glyph glyph, Square square);
		Target(Glyph glyph, const bitset<NumSquares>& squares);

		void updateMen(const bitset<NumMen>& men);
		void updateMen(const array<bitset<NumMen>, NumSquares>& men);
		void updateSquares(const bitset<NumSquares>& squares);
		int updateRequiredMoves(const array<int, NumSquares>& requiredMoves);
		int updateRequiredCaptures(const array<int, NumSquares>& requiredCaptures);

		bool isTargetOf(const Destination& destination) const;

	public :
		inline bool isOccupied() const
			{ return (_glyph != NoGlyph); }

	public :
		inline Glyph glyph() const
			{ return _glyph; }
		inline Square square() const
			{ return _square; }

		inline const bitset<NumSquares>& squares() const
			{ return _squares; }
		inline const bitset<NumMen>& men() const
			{ return _men; }

	public :
		inline int getRequiredMoves() const
			{ return requiredMoves; }
		inline int getRequiredCaptures() const
			{ return requiredCaptures; }

	private :
		Glyph _glyph;
		Square _square;

		bitset<NumSquares> _squares;
		bitset<NumMen> _men;
		
		int requiredMoves;
		int requiredCaptures;
};

/* -------------------------------------------------------------------------- */

class Targets : public vector<Target>
{
	public :
		Targets(const Problem& problem, Color color);

		void reset(const Board& board, const Pieces& pieces);
		void update(const Destinations& destinations);

	public :
		inline int getRequiredMoves() const
			{ return requiredMoves; }
		inline int getRequiredCaptures() const
			{ return requiredCaptures; }

	public :
		inline const bitset<NumSquares>& getCaptures() const
			{ return captures; }

	private :
		Color color;

		int requiredMoves;
		int requiredCaptures;

		bitset<NumSquares> captures;
};

/* -------------------------------------------------------------------------- */

}

#endif
