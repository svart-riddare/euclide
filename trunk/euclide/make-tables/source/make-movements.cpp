#include "make-tables.h"
#include "strings.h"

#include "enumerations.h"
using namespace euclide;

#include <cstdlib>

/* -------------------------------------------------------------------------- */

void makeMovements(bool castling)
{	
	CodeFile file("movement-tables.cpp");

	fprintf(file, "const bool movements[NumGlyphs][NumSquares][NumSquares] =\n{\n");

	for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
	{
		fprintf(file, "\t{  /* %s */\n", strings::xglyphs[glyph]);

		for (Square from = FirstSquare; from <= LastSquare; from++)
		{
			fprintf(file, "\t\t{ ");

			for (Square to = FirstSquare; to <= LastSquare; to++)
			{
				bool isValid = false;

				int horizontalDelta = to.column() - from.column();
				int horizontalDistance = abs(horizontalDelta);
				
				int verticalDelta = to.row() - from.row();
				int verticalDistance = abs(verticalDelta);

				if ((glyph == WhiteKing) || (glyph == BlackKing))
					if (horizontalDistance <= 1)
						if (verticalDistance <= 1)
							isValid = true;

				if (castling)
					if ((glyph == WhiteKing) && (from == E1))
						if ((to == C1) || (to == G1))
							isValid = true;

				if (castling)
					if ((glyph == BlackKing) && (from == E8))
						if ((to == C8) || (to == G8))
							isValid = true;

				if ((glyph == WhiteQueen) || (glyph == BlackQueen))
					if (!horizontalDistance || !verticalDistance || (horizontalDistance == verticalDistance))
						isValid = true;

				if ((glyph == WhiteRook) || (glyph == BlackRook))
					if (!horizontalDistance || !verticalDistance)
						isValid = true;

				if ((glyph == WhiteBishop) || (glyph == BlackBishop))
					if (horizontalDistance == verticalDistance)
						isValid = true;

				if ((glyph == WhiteKnight) || (glyph == BlackKnight))
					if ((horizontalDistance * verticalDistance) == 2)
						isValid = true;

				if (glyph == WhitePawn)
					if ((verticalDelta == 1) && (from.row() != One))
						if (!horizontalDistance)
							isValid = true;

				if (glyph == WhitePawn)
					if ((from.row() == Two) && (to.row() == Four))
						if (!horizontalDistance)
							isValid = true;

				if (glyph == BlackPawn)
					if ((verticalDelta == -1) && (from.row() != Eight))
						if (!horizontalDistance)
							isValid = true;

				if (glyph == BlackPawn)
					if ((from.row() == Seven) && (to.row() == Five))
						if (!horizontalDistance)
							isValid = true;

				if (from == to)
					isValid = false;

				fprintf(file, "%s, ", isValid ? " true" : "false");
			}

			fprintf(file, "},\n");
		}

		fprintf(file, "\t},\n");
	}

	fprintf(file, "};\n");
	file.comment();

	fprintf(file, "const bool captures[NumGlyphs][NumSquares][NumSquares] =\n{\n");

	for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
	{
		fprintf(file, "\t{  /* %s */\n", strings::xglyphs[glyph]);

		for (Square from = FirstSquare; from <= LastSquare; from++)
		{
			fprintf(file, "\t\t{ ");

			for (Square to = FirstSquare; to <= LastSquare; to++)
			{
				bool isValid = false;

				int horizontalDelta = to.column() - from.column();
				int horizontalDistance = abs(horizontalDelta);
				
				int verticalDelta = to.row() - from.row();
				int verticalDistance = abs(verticalDelta);

				if (glyph.isKing())
					if (horizontalDistance <= 1)
						if (verticalDistance <= 1)
							isValid = true;

				if (glyph.isQueen())
					if (!horizontalDistance || !verticalDistance || (horizontalDistance == verticalDistance))
						isValid = true;

				if (glyph.isRook())
					if (!horizontalDistance || !verticalDistance)
						isValid = true;

				if (glyph.isBishop())
					if (horizontalDistance == verticalDistance)
						isValid = true;

				if (glyph.isKnight())
					if ((horizontalDistance * verticalDistance) == 2)
						isValid = true;

				if (glyph == WhitePawn)
					if (verticalDelta == 1)
						if (horizontalDistance == 1)
							if (from.row() != One)
								isValid = true;

				if (glyph == BlackPawn)
					if (verticalDelta == -1)
						if (horizontalDistance == 1)
							if (from.row() != Eight)
								isValid = true;

				if (from == to)
					isValid = false;

				fprintf(file, "%s, ", isValid ? " true" : "false");
			}

			fprintf(file, "},\n");
		}

		fprintf(file, "\t},\n");
	}

	fprintf(file, "};\n");
}

/* -------------------------------------------------------------------------- */

