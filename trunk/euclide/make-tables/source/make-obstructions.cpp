#include "make-tables.h"
#include "strings.h"

#include "enumerations.h"
#include "constants.h"
using namespace euclide;
using namespace constants;

#include <cstdlib>
#include <algorithm>

/* -------------------------------------------------------------------------- */

void makeObstructions(void)
{	
	CodeFile file("obstructions.cpp");

	int numObstructions[NumGlyphs][NumSquares];

	fprintf(file, "const Obstruction _obstructions[] =\n{\n");

	for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
	{
		for (Square square = FirstSquare; square <= LastSquare; square++)
		{
			numObstructions[glyph][square] = NULL;

			if (glyph != NoGlyph)
				fprintf(file, "\t/* %s, %s */\n", strings::glyphs[glyph], strings::squares[square]);

			for (Square from = FirstSquare; from <= LastSquare; from++)
			{
				for (Square to = FirstSquare; to <= LastSquare; to++)
				{
					bool blocked = false;

					int horizontalDelta = to.column() - from.column();
					int horizontalDistance = abs(horizontalDelta);
				
					int verticalDelta = to.row() - from.row();
					int verticalDistance = abs(verticalDelta);

					int minColumn = std::min(from.column(), to.column());
					int maxColumn = std::max(from.column(), to.column());
					int minRow = std::min(from.row(), to.row());
					int maxRow = std::max(from.row(), to.row());

					int column = square.column();
					int row = square.row();

					if (glyph.isKing())
						if (horizontalDistance <= 1)
							if (verticalDistance <= 1)
								if (square == to)
									blocked = true;
						
					if (glyph.isRook() || glyph.isQueen())
						if (!horizontalDistance)
							if (column == minColumn)
							if ((minRow <= row) && (row <= maxRow))
								blocked = true;
									
					if (glyph.isRook() || glyph.isQueen())
						if (!verticalDistance)
							if (row == minRow)
							if ((minColumn <= column) && (column <= maxColumn))
								blocked = true;
					
					if (glyph.isBishop() || glyph.isQueen())
						if (horizontalDistance == verticalDistance)
							if (abs(column - from.column()) == abs(row - from.row()))
								if (abs(column - to.column()) == abs(row - to.row()))
									if ((minRow <= row) && (row <= maxRow))
										blocked = true;
					
					if (glyph.isKnight())
						if ((horizontalDistance * verticalDistance) == 2)
							if (square == to)
							blocked = true;
					
					if (glyph.isPawn())
						if (verticalDelta == (glyph.isWhite() ? 1 : -1))
							if (from.row() != (glyph.isWhite() ? One : Eight))
								if (horizontalDistance <= 1)
									if (square == to)
									blocked = true;

					if (glyph.isPawn())
						if (from.row() == (glyph.isWhite() ? Two : Seven))
							if ((verticalDistance == 2) && !horizontalDistance)
								if (square == Square(from.column(), (row_t)(from.row() - verticalDistance / 2)))
									blocked = true;

					if (from == square)
						blocked = false;

					if (blocked)
						fprintf(file, "\tObstruction(%s, %s),\n", strings::squares[from], strings::squares[to]), numObstructions[glyph][square]++;
				}
			}
		}
	}

	fprintf(file, "};\n");
	file.comment();

	fprintf(file, "const Obstructions obstructions[NumGlyphs][NumSquares] =\n{\n");

	int index = 0;
	for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
	{
		fprintf(file, "\t{  /* %s */\n", strings::glyphs[glyph]);

		for (Square square = FirstSquare; square <= LastSquare; square++)
		{
			fprintf(file, "\t\tObstructions(&_obstructions[%5d], %3d),  /* %s */\n", index, numObstructions[glyph][square], strings::squares[square]);
			index += numObstructions[glyph][square];
		}

		fprintf(file, "\t},\n");
	}

	fprintf(file, "};\n");
}

/* -------------------------------------------------------------------------- */
