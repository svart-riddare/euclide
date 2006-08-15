#ifndef __EUCLIDE_GLYPHS_H
#define __EUCLIDE_GLYPHS_H

#include "colors.h"
#include "errors.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

typedef enum
{
	NoGlyph,
	WhiteKing, WhiteQueen, WhiteRook, WhiteBishop, WhiteKnight, WhitePawn,
	BlackKing, BlackQueen, BlackRook, BlackBishop, BlackKnight, BlackPawn,
	
	NumGlyphs, UndefinedGlyph = -1,
	FirstGlyph = NoGlyph, LastGlyph = BlackPawn,
	FirstWhiteGlyph = WhiteKing, LastWhiteGlyph = WhitePawn,
	FirstBlackGlyph = BlackKing, LastBlackGlyph = BlackPawn,
	
} glyph_t;

/* -------------------------------------------------------------------------- */

class Glyph 
{
	public :
		inline Glyph() {}
		inline Glyph(glyph_t glyph)
			{ this->glyph = glyph; }
		inline Glyph(EUCLIDE_Glyph glyph)
			{ this->glyph = (glyph_t)glyph; }

		/* ---------------------------------- */

		inline operator glyph_t() const
			{ return glyph; }		
		inline operator glyph_t&()
			{ return glyph; }

		inline EUCLIDE_Glyph glyph_c() const
			{ return (EUCLIDE_Glyph)glyph; }

		/* ---------------------------------- */

		inline Glyph operator++(int)
			{ return (glyph_t)((int&)glyph)++; }
		inline Glyph operator--(int)
			{ return (glyph_t)((int&)glyph)--; }

		inline Glyph& operator++()
			{ ++(int&)glyph; return *this; }
		inline Glyph& operator--()
			{ --(int&)glyph; return *this; }
		
		/* ---------------------------------- */

		bool isValid() const
		{
			return ((glyph >= FirstGlyph) && (glyph <= LastGlyph));
		}

		/* ---------------------------------- */

		bool isColor(Color color) const
		{
			if (color == White)
				return ((glyph >= FirstWhiteGlyph) && (glyph <= LastWhiteGlyph));

			if (color == Black)
				return ((glyph >= FirstBlackGlyph) && (glyph <= LastBlackGlyph));

			return false;
		}

		bool isWhite() const
			{ return isColor(White); }
		bool isBlack() const
			{ return isColor(Black); }

		/* ---------------------------------- */

		bool isKing() const
		{
			return ((glyph == WhiteKing) || (glyph == BlackKing));
		}

		bool isQueen()
		{
			return ((glyph == WhiteQueen) || (glyph == BlackQueen));
		}

		bool isRook()
		{
			return ((glyph == WhiteRook) || (glyph == BlackRook));
		}

		bool isBishop()
		{
			return ((glyph == WhiteBishop) || (glyph == BlackBishop));
		}

		bool isKnight()
		{
			return ((glyph == WhiteKnight) || (glyph == BlackKnight));
		}

		bool isPawn()
		{
			return ((glyph == WhitePawn) || (glyph == BlackPawn));
		}

		/* ---------------------------------- */

	private :
		glyph_t glyph;
};

/* -------------------------------------------------------------------------- */

}

#endif
