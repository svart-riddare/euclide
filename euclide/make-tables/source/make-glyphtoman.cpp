#include "make-tables.h"

#include "enumerations.h"
#include "constants.h"
using namespace euclide;

// ----------------------------------------------------------------------------

void makeGlyphToMan(void)
{	
	CodeFile file("glyphtoman.cpp");

	fprintf(file, "const bool validGlyphManColor[NumGlyphs][NumMen][NumColors] =\n{\n");

	for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
	{
		fprintf(file, "\t{  /* %s */\n\t\t", constants::glyphNames[glyph]);

		for (Man man = FirstMan; man <= LastMan; man++)
		{
			bool valid = false;

			if (isGlyphKing(glyph) && isManKing(man))
				valid = true;

			if (isGlyphQueen(glyph) && isManQueen(man))
				valid = true;

			if (isGlyphRook(glyph) && isManRook(man))
				valid = true;

			if (isGlyphBishop(glyph) && isManBishop(man))
				valid = true;

			if (isGlyphKnight(glyph) && isManKnight(man))
				valid = true;

			if (!isGlyphKing(glyph) && isManPawn(man))
				valid = true;
		
			fprintf(file, "{ %s, %s }, ", (valid && isWhiteGlyph(glyph)) ? " true" : "false", (valid && isBlackGlyph(glyph)) ? " true" : "false");
		}

		fprintf(file, "\n\t},\n");
	}

	fprintf(file, "\n};\n");
	file.comment();

	fprintf(file, "const Man getGlyphSuperman[NumGlyphs][NumColumns] =\n{\n");

	for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
	{
		fprintf(file, "\t{  /* %s */\n\t\t", constants::glyphNames[glyph]);

		const char *name = NULL;

		if (isGlyphQueen(glyph))
			name = "Queen";
		if (isGlyphRook(glyph))
			name = "Rook";
		if (isGlyphBishop(glyph))
			name = "Bishop";
		if (isGlyphKnight(glyph))
			name = "Knight";

		for (Column column = FirstColumn; column <= LastColumn; column++)
		{
			if (name == NULL)
				fprintf(file, "UndefinedSuperman, ");
			else
				fprintf(file, "%s%s, ", constants::columnNames[column], name);
		}

		fprintf(file, "\n\t},\n");
	}

	fprintf(file, "\n};\n");
}

// ----------------------------------------------------------------------------
