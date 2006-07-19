#include "make-tables.h"
#include "strings.h"

#include "enumerations.h"
using namespace euclide;

/* -------------------------------------------------------------------------- */

void makeInitialSquares(void)
{	
	CodeFile file("initialsquares.cpp");

	fprintf(file, "const Square initialSquares[NumSupermen][NumColors] =\n{\n");

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
	{
		Square square = UndefinedSquare;

		if (superman == King)
			square = E1;
		else
		if (superman == Queen)
			square = D1;
		else
		if (superman == QueenRook)
			square = A1;
		else
		if (superman == KingRook)
			square = H1;
		else
		if (superman == QueenBishop)
			square = C1;
		else
		if (superman == KingBishop)
			square = F1;
		else
		if (superman == QueenKnight)
			square = B1;
		else
		if (superman == KingKnight)
			square = G1;
		else
			square = Square((column_t)(superman % NumColumns), (superman <= LastPawn) ? Two : Eight);

		fprintf(file, "\t{ %s, %s },  /* %s */\n", strings::squares[square], strings::squares[-square], strings::supermen[superman]);
	}

	fprintf(file, "};\n");
}

/* -------------------------------------------------------------------------- */
