#include "make-tables.h"
#include "strings.h"

#include "enumerations.h"
#include "constants.h"
using namespace euclide;
using namespace constants;

#include <cstdlib>
#include <algorithm>

/* -------------------------------------------------------------------------- */

void makeObstructions(bool castling)
{	
	CodeFile file("obstruction-tables.cpp");

	int numObstructions[NumGlyphs][NumSquares];

	fprintf(file, "const Obstruction _obstructions[] =\n{\n");

	for (Glyph glyph = FirstGlyph; glyph <= LastGlyph; glyph++)
	{
		for (Square square = FirstSquare; square <= LastSquare; square++)
		{
			numObstructions[glyph][square] = 0;

			if (glyph != NoGlyph)
				fprintf(file, "\t/* %s, %s */\n", strings::glyphs[glyph], strings::squares[square]);

			for (Square from = FirstSquare; from <= LastSquare; from++)
			{
				for (Square to = FirstSquare; to <= LastSquare; to++)
				{
					bool blocked = false;
					bool royal = false;
					bool check = false;

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
								if ((square == to) || (square == from))
									blocked = true;

					if (castling)
						if ((glyph == WhiteKing) && (from == E1) && (to == C1))
							if ((square == B1) || (square == C1) || (square == D1) || (square == E1))
								blocked = true;

					if (castling)
						if ((glyph == WhiteKing) && (from == E1) && (to == G1))
							if ((square == E1) || (square == F1) || (square == G1))
								blocked = true;

					if (castling)
						if ((glyph == BlackKing) && (from == E8) && (to == C8))
							if ((square == B8) || (square == C8) || (square == D8) || (square == E8))
								blocked = true;

					if (castling)
						if ((glyph == BlackKing) && (from == E8) && (to == G8))
							if ((square == E8) || (square == F8) || (square == G8))
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
							if ((square == to) || (square == from))
								blocked = true;
					
					if (glyph.isPawn())
						if (verticalDelta == (glyph.isWhite() ? 1 : -1))
							if (from.row() != (glyph.isWhite() ? One : Eight))
								if (horizontalDistance <= 1)
									if ((square == to) || (square == from))
										blocked = true;

					if (glyph.isPawn())
						if (from.row() == (glyph.isWhite() ? Two : Seven))
							if ((verticalDistance == 2) && !horizontalDistance)
								if ((square == from) || (square == to) || (square == (from + to) / 2))
									blocked = true;

					if (!blocked)
					{
						if (glyph.isKing())
							if ((abs(column - to.column()) <= 1) && (abs(row - to.row()) <= 1))
								if ((horizontalDistance <= 1) && (verticalDistance <= 1))
									blocked = true;

						if (castling)
							if ((abs(column - to.column()) <= 1) && (abs(row - to.row()) <= 1))
								if ((glyph == WhiteKing) && (from == E1) && ((to == C1) || (to == G1)))
									blocked = true;

						if (castling)
							if ((abs(column - to.column()) <= 1) && (abs(row - to.row()) <= 1))
								if ((glyph == BlackKing) && (from == E8) && ((to == C8) || (to == G8)))
									blocked = true;
							
						if (glyph.isKing())
							if ((abs(column - to.column()) * abs(row - to.row())) == 2)
								if ((horizontalDistance <= 1) && (verticalDistance <= 1))
									blocked = true;

						if (castling)
							if ((abs(column - to.column()) * abs(row - to.row())) == 2)
								if ((glyph == WhiteKing) && (from == E1) && ((to == C1) || (to == G1)))
									blocked = true;

						if (castling)
							if ((abs(column - to.column()) * abs(row - to.row())) == 2)
								if ((glyph == BlackKing) && (from == E8) && ((to == C8) || (to == G8)))
									blocked = true;

						if (blocked)
							check = true;
					}

					if (!blocked)
					{
						if (glyph.isKing())
							if ((abs(column - from.column()) <= 1) && (abs(row - from.row()) <= 1))
								if ((horizontalDistance <= 1) && (verticalDistance <= 1))
									blocked = true;

						if (castling)
							if ((glyph == WhiteKing) && (from == E1) && (to == C1))
								if ((square == A2) || ((abs(column - from.column()) <= 1) && (abs(row - from.row()) <= 1)))
									blocked = true;

						if (castling)
							if ((glyph == WhiteKing) && (from == E1) && (to == G1))
								if ((square == H2) || ((abs(column - from.column()) <= 1) && (abs(row - from.row()) <= 1)))
									blocked = true;

						if (castling)
							if ((glyph == BlackKing) && (from == E8) && (to == C8))
								if ((square == A7) || ((abs(column - from.column()) <= 1) && (abs(row - from.row()) <= 1)))
									blocked = true;

						if (castling)
							if ((glyph == BlackKing) && (from == E8) && (to == G8))
								if ((square == H7) || ((abs(column - from.column()) <= 1) && (abs(row - from.row()) <= 1)))
									blocked = true;

						if (glyph.isQueen())
							if ((abs(column - from.column()) <= 1) && (abs(row - from.row()) <= 1))
								if ((horizontalDistance == verticalDistance) || !horizontalDistance || !verticalDistance)
									blocked = true;

						if (glyph.isRook())
							if ((abs(column - from.column()) + abs(row - from.row())) == 1)
								if (!horizontalDistance || !verticalDistance)
									blocked = true;

						if (glyph.isBishop())
							if ((abs(column - from.column()) == 1) && (abs(row - from.row()) == 1))
								if (horizontalDistance == verticalDistance)
									blocked = true;

						if (glyph.isKnight())
							if ((abs(column - from.column()) * abs(row - from.row())) == 2)
								if (horizontalDistance * verticalDistance == 2)
									blocked = true;

						if (glyph == WhitePawn)
							if ((abs(column - from.column()) == 1) && ((row - from.row()) == 1))
								if (from.row() != One)
									if (((horizontalDistance <= 1) && (verticalDelta == 1)) || ((from.row() == Two) && !horizontalDistance && (verticalDelta == 2)))
										blocked = true;

						if (glyph == BlackPawn)
							if ((abs(column - from.column()) == 1) && ((row - from.row()) == -1))
								if (from.row() != Eight)
									if (((horizontalDistance <= 1) && (verticalDelta == -1)) || ((from.row() == Seven) && !horizontalDistance && (verticalDelta == -2)))
										blocked = true;

						if (blocked)
							royal = true;
					}

					if (from == to)
						blocked = false;

					if (blocked)
						fprintf(file, "\t{ %s, %s, %s, %s },\n", strings::squares[from], strings::squares[to], royal ? " true" : "false", check ? " true" : "false"), numObstructions[glyph][square]++;
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
			fprintf(file, "\t\t{ &_obstructions[%5d], %3d },  /* %s */\n", index, numObstructions[glyph][square], strings::squares[square]);
			index += numObstructions[glyph][square];
		}

		fprintf(file, "\t},\n");
	}

	fprintf(file, "};\n");
}

/* -------------------------------------------------------------------------- */
