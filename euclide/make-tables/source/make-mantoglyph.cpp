#include "make-tables.h"
#include "strings.h"

#include "enumerations.h"
using namespace euclide;

/* -------------------------------------------------------------------------- */

void makeManToGlyph(void)
{	
	CodeFile file("mantoglyph.cpp");

	fprintf(file, "const Glyph manToGlyph[NumSupermen][NumColors] =\n{\n");

	for (Man man = FirstSuperman; man <= LastSuperman; man++)
	{
		Glyph whiteGlyph = NoGlyph;
		Glyph blackGlyph = NoGlyph;

		if (man == King)
		{
			whiteGlyph = WhiteKing;
			blackGlyph = BlackKing;
		}

		if ((man == Queen) || ((man >= FirstSuperQueen) && (man <= LastSuperQueen)))
		{
			whiteGlyph = WhiteQueen;
			blackGlyph = BlackQueen;
		}

		if (((man >= FirstRook) && (man <= LastRook)) || ((man >= FirstSuperRook) && (man <= LastSuperRook)))
		{
			whiteGlyph = WhiteRook;
			blackGlyph = BlackRook;
		}
		
		if (((man >= FirstBishop) && (man <= LastBishop)) || ((man >= FirstSuperBishop) && (man <= LastSuperBishop)))
		{
			whiteGlyph = WhiteBishop;
			blackGlyph = BlackBishop;
		}

		if (((man >= FirstKnight) && (man <= LastKnight)) || ((man >= FirstSuperKnight) && (man <= LastSuperKnight)))
		{
			whiteGlyph = WhiteKnight;
			blackGlyph = BlackKnight;
		}

		if ((man >= FirstPawn) && (man <= LastPawn))
		{
			whiteGlyph = WhitePawn;
			blackGlyph = BlackPawn;
		}

		fprintf(file, "\t{ %s,\t%s },  /* %s */\n", strings::glyphs[whiteGlyph], strings::glyphs[blackGlyph], strings::men[man]);
	}

	fprintf(file, "\n};\n");
}

/* -------------------------------------------------------------------------- */
