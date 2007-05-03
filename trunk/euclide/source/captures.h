#ifndef __EUCLIDE_CAPTURES_H
#define __EUCLIDE_CAPTURES_H

#include "includes.h"
#include "targets.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

class Capture : public Pointer<Target>
{
	public :
		Capture(Target *target);

		int computeRequiredMoves(const Board& board);
		int computeRequiredCaptures(const Board& board);

		int updateRequiredMoves();
		int updateRequiredCaptures();

		void assign(Target *target, bool required);

		bool setPossibleSquares(const Squares& squares)
			{ return mytarget()->setPossibleSquares(squares); }

	protected :
		Target *mytarget()
			{ return *this; }
		const Target *mytarget() const
			{ return *this; }
		
	public :
		inline int getRequiredMoves() const
			{ return requiredMoves; }
		inline int getRequiredCaptures() const
			{ return requiredCaptures; }

		inline int getRequiredMoves(Man man) const
			{ return menRequiredMoves[man]; }
		inline int getRequiredCaptures(Man man) const
			{ return menRequiredCaptures[man]; }

	public :
		inline bool assigned() const
			{ return target != NULL; }
		inline bool required() const
			{ return _required; }

	public :
		inline Man man() const
			{ return _man; }
		inline Glyph glyph() const
			{ return _glyph; }
		inline Color color() const
			{ return _color; }
		inline Square square() const
			{ return mytarget()->square(); }

		inline const Squares& squares() const
			{ return mytarget()->squares(); }
		inline const Men& men() const
			{ return _men; }
		inline int candidates() const
			{ return (int)_men.count(); }

	private :
		Man _man;
		Men _men;

		Glyph _glyph;
		Color _color;

	private :
		Target *target;
		bool _required;

	private :
		int requiredMoves;
		int requiredCaptures;

		array<int, NumMen> menRequiredMoves;
		array<int, NumMen> menRequiredCaptures;
};

/* -------------------------------------------------------------------------- */

class Captures : public vector<Capture> {};		

/* -------------------------------------------------------------------------- */

}

#endif
