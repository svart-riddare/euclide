#include "make-tables.h"
#include "strings.h"

#include "enumerations.h"
using namespace euclide;

/* -------------------------------------------------------------------------- */

void makeGlyphToMan(void)
{	
	CodeFile file("glyphtoman.cpp");

	fprintf(file, "const bool validGlyphManColor[NumGlyphs][NumMen][NumColors] =\n{\n");

	for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
	{
		fprintf(file, "\t{  /* %s */\n\t\t", strings::glyphs[glyph]);

		for (Man man = FirstMan; man <= LastMan; man++)
		{
			bool valid = false;

			if (glyph.isKing() && man.isKing())
				valid = true;

			if (glyph.isQueen() && man.isQueen())
				valid = true;

			if (glyph.isRook() && man.isRook())
				valid = true;

			if (glyph.isBishop() && man.isBishop())
				valid = true;

			if (glyph.isKnight() && man.isKnight())
				valid = true;

			if (!glyph.isKing() && man.isPawn())
				valid = true;
		
			fprintf(file, "{ %s, %s }, ", (valid && glyph.isWhite()) ? " true" : "false", (valid && glyph.isBlack()) ? " true" : "false");
		}

		fprintf(file, "\n\t},\n");
	}

	fprintf(file, "\n};\n");
	file.comment();

	fprintf(file, "const Man glyphToMan[NumGlyphs][NumColumns] =\n{\n");

	for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
	{
		fprintf(file, "\t{  /* %s */\n\t\t", strings::glyphs[glyph]);

		const char *name = NULL;

		if (glyph.isQueen())
			name = "Queen";
		if (glyph.isRook())
			name = "Rook";
		if (glyph.isBishop())
			name = "Bishop";
		if (glyph.isKnight())
			name = "Knight";

		for (Column column = FirstColumn; column <= LastColumn; column++)
		{
			if (name == NULL)
				fprintf(file, "UndefinedSuperman, ");
			else
				fprintf(file, "%s%s, ", strings::columns[column], name);
		}

		fprintf(file, "\n\t},\n");
	}

	fprintf(file, "\n};\n");
}

/* -------------------------------------------------------------------------- */

