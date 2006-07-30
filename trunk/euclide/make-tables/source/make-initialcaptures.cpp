#include "make-tables.h"
#include "strings.h"

#include "enumerations.h"
#include "constants.h"
using namespace euclide;
using namespace constants;

#include <cstdlib>
#include <algorithm>

/* -------------------------------------------------------------------------- */

void makeInitialCaptures(void)
{	
	CodeFile file("initialcaptures.cpp");

	fprintf(file, "const int initialCaptures[NumSupermen][NumSquares][NumColors] =\n{\n");

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
	{
		fprintf(file, "\t{  /* %s */\n", strings::supermen[superman]);

		for (Square square = FirstSquare; square <= LastSquare; square++)
		{
			int captures = 0;

			if (superman.isPawn())
				captures = abs(square.column() - (superman % NumColumns));

			fprintf(file, "\t\t{ %1d, %1d },  /* %s */\n", captures, captures, strings::squares[square]);
		}

		fprintf(file, "\t},\n");
	}

	fprintf(file, "};\n");
}

/* -------------------------------------------------------------------------- */
