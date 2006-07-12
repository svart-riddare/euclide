#include "make-tables.h"
#include "strings.h"

#include "enumerations.h"
using namespace euclide;

/* -------------------------------------------------------------------------- */

void makeSupermanToGlyph(void)
{	
	CodeFile file("mantoglyph.cpp");

	fprintf(file, "const Glyph supermanToGlyph[NumSupermen][NumColors] =\n{\n");

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
	{
		Glyph whiteGlyph = NoGlyph;
		Glyph blackGlyph = NoGlyph;

		if (superman.isKing())
		{
			whiteGlyph = WhiteKing;
			blackGlyph = BlackKing;
		}
		else
		if (superman.isQueen())
		{
			whiteGlyph = WhiteQueen;
			blackGlyph = BlackQueen;
		}
		else
		if (superman.isRook())
		{
			whiteGlyph = WhiteRook;
			blackGlyph = BlackRook;
		}
		else
		if (superman.isBishop())
		{
			whiteGlyph = WhiteBishop;
			blackGlyph = BlackBishop;
		}
		else
		if (superman.isKnight())
		{
			whiteGlyph = WhiteKnight;
			blackGlyph = BlackKnight;
		}
		else
		if (superman.isPawn())
		{
			whiteGlyph = WhitePawn;
			blackGlyph = BlackPawn;
		}

		fprintf(file, "\t{ %16s,\t%16s },  /* %s */\n", strings::glyphs[whiteGlyph], strings::glyphs[blackGlyph], strings::supermen[superman]);
	}

	fprintf(file, "\n};\n");
}

/* -------------------------------------------------------------------------- */
