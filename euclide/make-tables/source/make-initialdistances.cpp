#include "make-tables.h"
#include "strings.h"

#include "enumerations.h"
#include "constants.h"
using namespace euclide;
using namespace constants;

#include <cstdlib>
#include <algorithm>

/* -------------------------------------------------------------------------- */

void makeInitialDistances(void)
{	
	CodeFile file("initialdistances.cpp");

	fprintf(file, "const int initialDistances[NumSupermen][NumSquares][NumColors] =\n{\n");

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
	{
		fprintf(file, "\t{  /* %s */\n", strings::supermen[superman]);

		for (Square square = FirstSquare; square <= LastSquare; square++)
		{
			int whiteDistance = infinity;
			int blackDistance = infinity;

			Square whiteSquare = UndefinedSquare;
			Square blackSquare = UndefinedSquare;
			
			if (superman == King)
				whiteSquare = E1;
			else
			if (superman == Queen)
				whiteSquare = D1;
			else
			if (superman == QueenRook)
				whiteSquare = A1;
			else
			if (superman == KingRook)
				whiteSquare = H1;
			else
			if (superman == QueenBishop)
				whiteSquare = C1;
			else
			if (superman == KingBishop)
				whiteSquare = F1;
			else
			if (superman == QueenKnight)
				whiteSquare = B1;
			else
			if (superman == KingKnight)
				whiteSquare = G1;
			else
			if (superman.isPawn())
				whiteSquare = Square((column_t)(superman % 8), Two);
			else
				whiteSquare = Square((column_t)(superman % 8), Eight);

			blackSquare = -whiteSquare;

			int whiteHorizontal = abs(whiteSquare.column() - square.column());
			int blackHorizontal = abs(blackSquare.column() - square.column());
			int whiteVertical = abs(whiteSquare.row() - square.row());
			int blackVertical = abs(blackSquare.row() - square.row());

			bool isWhiteMonochrome = (((whiteHorizontal + whiteVertical) % 2) == 0);
			bool isBlackMonochrome = (((blackHorizontal + blackVertical) % 2) == 0);

			if (superman.isKing())
			{
				whiteDistance = std::max(whiteHorizontal, whiteVertical);
				blackDistance = std::max(blackHorizontal, blackVertical);
			}

			if (superman.isQueen())
			{
				whiteDistance = 2;
				blackDistance = 2;

				if (!whiteHorizontal || !whiteVertical || (whiteHorizontal == whiteVertical))
					whiteDistance = 1;
				if (!blackHorizontal || !blackVertical || (blackHorizontal == blackVertical))
					blackDistance = 1;
			}

			if (superman.isRook())
			{
				whiteDistance = 2;
				blackDistance = 2;

				if (!whiteHorizontal || !whiteVertical)
					whiteDistance = 1;
				if (!blackHorizontal || !blackVertical)
					blackDistance = 1;
			}

			if (superman.isBishop())
			{
				if (isWhiteMonochrome)
					whiteDistance = 2;
				if (isBlackMonochrome)
					blackDistance = 2;

				if (whiteHorizontal == whiteVertical)
					whiteDistance = 1;
				if (blackHorizontal == blackVertical)
					blackDistance = 1;
			}

			if (superman.isKnight())
			{
				whiteDistance = 5;
				blackDistance = 5;

				if (isWhiteMonochrome)
					whiteDistance = 4;
				if (isBlackMonochrome)
					blackDistance = 4;

				if (!isWhiteMonochrome && (whiteHorizontal < 7) && (whiteVertical < 7))
					whiteDistance = 3;
				if (!isBlackMonochrome && (blackHorizontal < 7) && (blackVertical < 7))
					blackDistance = 3;

				if (isWhiteMonochrome && (whiteHorizontal < 5) && (whiteVertical < 5))
					if ((whiteHorizontal != 2) || (whiteVertical != 2))
						whiteDistance = 2;
				if (isBlackMonochrome && (blackHorizontal < 5) && (blackVertical < 5))
					if ((blackHorizontal != 2) || (blackVertical != 2))
						blackDistance = 2;

				if ((whiteHorizontal * whiteVertical) == 2)
					whiteDistance = 1;
				if ((blackHorizontal * blackVertical) == 2)
					blackDistance = 1;

				if (whiteSquare == square)
					whiteDistance = 0;
				if (blackSquare == square)
					blackDistance = 0;
			}

			if (superman.isPawn())
			{
				if ((square.row() - whiteSquare.row()) >= whiteHorizontal)
					whiteDistance = whiteVertical - ((whiteVertical > (whiteHorizontal + 1)) ? 1 : 0);
				if ((blackSquare.row() - square.row()) >= blackHorizontal)
					blackDistance = blackVertical - ((blackVertical > (blackHorizontal + 1)) ? 1 : 0);
			}

			if (whiteSquare == square)
				whiteDistance = 0;
			if (blackSquare == square)
				blackDistance = 0;

			if ((whiteDistance >= infinity) && (blackDistance >= infinity))
				fprintf(file, "\t\t{ infinity, infinity },  /* %s */\n", strings::squares[square]);
			else
			if (whiteDistance >= infinity)
				fprintf(file, "\t\t{ infinity, %8d },  /* %s */\n", blackDistance, strings::squares[square]);
			else
			if (blackDistance >= infinity)
				fprintf(file, "\t\t{ %8d, infinity },  /* %s */\n", whiteDistance, strings::squares[square]);
			else
				fprintf(file, "\t\t{ %8d, %8d },  /* %s */\n", whiteDistance, blackDistance, strings::squares[square]);
		}

		fprintf(file, "\t},\n");
	}

	fprintf(file, "};\n");
}

/* -------------------------------------------------------------------------- */
