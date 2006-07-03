#ifndef __EUCLIDE_GLYPHS_H
#define __EUCLIDE_GLYPHS_H

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
	
} Glyph;

/* -------------------------------------------------------------------------- */

inline bool isValidGlyph(Glyph glyph)
{
	return ((glyph >= FirstGlyph) && (glyph <= LastGlyph));
}

/* -------------------------------------------------------------------------- */

inline bool isWhiteGlyph(Glyph glyph)
{
	assert(isValidGlyph(glyph));
	return ((glyph >= FirstWhiteGlyph) && (glyph <= LastWhiteGlyph));
}

inline bool isBlackGlyph(Glyph glyph)
{
	assert(isValidGlyph(glyph));
	return ((glyph >= FirstBlackGlyph) && (glyph <= LastBlackGlyph));
}

/* -------------------------------------------------------------------------- */

inline bool isGlyphKing(Glyph glyph)
{
	assert(isValidGlyph(glyph));
	return ((glyph == WhiteKing) || (glyph == BlackKing));
}

inline bool isGlyphQueen(Glyph glyph)
{
	assert(isValidGlyph(glyph));
	return ((glyph == WhiteQueen) || (glyph == BlackQueen));
}

inline bool isGlyphRook(Glyph glyph)
{
	assert(isValidGlyph(glyph));
	return ((glyph == WhiteRook) || (glyph == BlackRook));
}

inline bool isGlyphBishop(Glyph glyph)
{
	assert(isValidGlyph(glyph));
	return ((glyph == WhiteBishop) || (glyph == BlackBishop));
}

inline bool isGlyphKnight(Glyph glyph)
{
	assert(isValidGlyph(glyph));
	return ((glyph == WhiteKnight) || (glyph == BlackKnight));
}

inline bool isGlyphPawn(Glyph glyph)
{
	assert(isValidGlyph(glyph));
	return ((glyph == WhitePawn) || (glyph == BlackPawn));
}

/* -------------------------------------------------------------------------- */

inline Glyph operator++(Glyph& glyph, int)
{
	Glyph result = glyph;
	glyph = static_cast<Glyph>(glyph + 1);
	return result;
}

inline Glyph operator++(Glyph& glyph)
{
	return
	glyph = static_cast<Glyph>(glyph + 1);
}

/* -------------------------------------------------------------------------- */

}

#endif
