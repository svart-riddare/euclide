#include "make-tables.h"
#include "strings.h"

#include "enumerations.h"
using namespace euclide;

#include <cstdlib>

/* -------------------------------------------------------------------------- */

void makeChecks(bool castling)
{	
	CodeFile file("check-tables.cpp");

	fprintf(file, "const bool checks[NumSquares][NumGlyphs][NumSquares] =\n{\n");

	for (Square king = FirstSquare; king <= LastSquare; king++)
	{
		fprintf(file, "\t{  /* %s */\n", strings::squares[king]);

		for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
		{
			fprintf(file, "\t\t{ ", strings::glyphs[glyph]);

			for (Square square = FirstSquare; square <= LastSquare; square++)
			{
				bool isCheck = true;

				if (glyph == NoGlyph)
					isCheck = false;

				if (square == king)
					isCheck = false;

				int horizontalDelta = king.column() - square.column();
				int horizontalDistance = abs(horizontalDelta);
				
				int verticalDelta = king.row() - square.row();
				int verticalDistance = abs(verticalDelta);

				if (!glyph.isKnight())
					if ((horizontalDistance > 1) || (verticalDistance > 1))
						isCheck = false;

				if (glyph.isKnight())
					if ((horizontalDistance * verticalDistance) != 2)
						isCheck = false;

				if (glyph.isRook())
					if (horizontalDistance && verticalDistance)
						isCheck = false;

				if (glyph.isBishop() || glyph.isPawn())
					if (!horizontalDistance || !verticalDistance)
						isCheck = false;

				if (glyph == WhitePawn)
					if ((square.row() == One) || (verticalDelta < 0))
						isCheck = false;

				if (glyph == BlackPawn)
					if ((square.row() == Eight) || (verticalDelta > 0))
						isCheck = false;

				fprintf(file, "%s, ", isCheck ? " true" : "false");
			}

			fprintf(file, " },\n");
		}

		fprintf(file, "\t},\n");
	}

	fprintf(file, "};\n");
}

/* -------------------------------------------------------------------------- */

