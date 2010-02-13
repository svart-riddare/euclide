#include "make-tables.h"
#include "strings.h"

#include "enumerations.h"
#include "constants.h"
using namespace euclide;
using namespace constants;

#include <cstdlib>
#include <algorithm>

/* -------------------------------------------------------------------------- */

void makeConstraints(bool castling)
{	
	CodeFile file("constraint-tables.cpp");

	int numConstraints[NumGlyphs][NumSquares][NumSquares];

	fprintf(file, "const Square _constraints[] =\n{\n");

	for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
	{
		for (Square from = FirstSquare; from <= LastSquare; from++)
		{
			for (Square to = FirstSquare; to <= LastSquare; to++)
			{
				numConstraints[glyph][from][to] = 0;

				bool interference = false;
		
				int horizontalDelta = to.column() - from.column();
				int horizontalDistance = abs(horizontalDelta);
				int horizontalIncrement = horizontalDistance ? horizontalDelta / horizontalDistance : 0;
				
				int verticalDelta = to.row() - from.row();
				int verticalDistance = abs(verticalDelta);
				int verticalIncrement = verticalDistance ? verticalDelta / verticalDistance : 0;

				if (glyph.isKing() && castling)
					if ((glyph == WhiteKing) && (from == E1) && ((to == C1) || (to == G1)))
						interference = true;

				if (glyph.isKing() && castling)
					if ((glyph == BlackKing) && (from == E8) && ((to == C8) ||(to == G8)))
						interference = true;

				if (glyph.isRook() || glyph.isQueen())
					if (!horizontalDistance && (verticalDistance > 1))
						interference = true;

				if (glyph.isRook() || glyph.isQueen())
					if (!verticalDistance && (horizontalDistance > 1))
						interference = true;

				if (glyph.isBishop() || glyph.isQueen())
					if ((horizontalDistance == verticalDistance) && (horizontalDistance > 1))
						interference = true;

				if (glyph.isPawn())
					if (from.row() == (glyph.isWhite() ? Two : Seven))
						if ((verticalDistance == 2) && !horizontalDistance)
							interference = true;

				if (!interference)
					continue;

				fprintf(file, "\t/* %s, %s -> %s */\n", strings::glyphs[glyph], strings::squares[from], strings::squares[to]);

				numConstraints[glyph][from][to] = std::max(horizontalDistance, verticalDistance) - 1;
				assert(numConstraints[glyph][from][to] > 0);			

				for (Square square = from; square != to; square = Square((column_t)(square.column() + horizontalIncrement), (row_t)(square.row() + verticalIncrement)))
				{
					if (square == from)
						fprintf(file, "\t");
					else
						fprintf(file, "%s, ", strings::squares[square]);
				}

				fprintf(file, "\n");
			}
		}
	}

	fprintf(file, "};\n");
	file.comment();

	fprintf(file, "const Constraints constraints[NumGlyphs][NumSquares][NumSquares] =\n{\n");

	int index = 0;
	for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
	{
		fprintf(file, "\t{  /* %s */\n", strings::glyphs[glyph]);

		for (Square from = FirstSquare; from <= LastSquare; from++)
		{
			fprintf(file, "\t\t{ ");

			for (Square to = FirstSquare; to <= LastSquare; to++)
			{
				if (numConstraints[glyph][from][to] > 0)
					fprintf(file, "{ &_constraints[%5d], %1d }%s", index, numConstraints[glyph][from][to], (to == LastSquare) ? " }, \n" : ", ");
				else
					fprintf(file, "{                 NULL, 0 }%s", (to == LastSquare) ? " },\n" : ", ");

				index += numConstraints[glyph][from][to];
			}
		}

		fprintf(file, "\t},\n");
	}

	fprintf(file, "};\n");
}

/* -------------------------------------------------------------------------- */
