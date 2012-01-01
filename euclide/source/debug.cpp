#include "includes.h"
#include "pieces.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

FILE *DebugFile()
{
	static FILE *file = NULL;

#ifndef NDEBUG
	if (!file)
		file = fopen("Debug.txt", "w");
#endif

	return file;
}

/* -------------------------------------------------------------------------- */

static
bool DebugHeader(const char *pszHeader, FILE *file)
{
	if (!file)
		return false;

	fprintf(file, "\n");
	fprintf(file, "--------------------------------------------------------------------------------\n");
	fprintf(file, "-- %s\n", pszHeader);
	fprintf(file, "--------------------------------------------------------------------------------\n");
	fprintf(file, "\n");

	return true;
}

/* -------------------------------------------------------------------------- */

static
const char *PrintMove(const Move *move)
{
	static char string[2][16];
	static int m = 0;

	m = (m + 1) % (sizeof(string) / sizeof(string[0]));

	sprintf(string[m], "%s%s-%s", strings::glyphs[move->glyph()], move->from().isValid() ? strings::squares[move->from()] : "??", move->to().isValid() ? strings::squares[move->to()] : "??");
	return string[m];
}

/* -------------------------------------------------------------------------- */

void DebugConstraints(const Board& board, FILE *file)
{
	if (!DebugHeader("Constraints", file))
		return;

	for (Color color = FirstColor; color < NumColors; color++)
	{
		for (Man man = FirstMan; man < NumMen; man++)
		{
			const Piece *piece = board.piece(color, man);
			if (!piece)
				continue;

			Squares from, to;

			int moves = piece->moves();
			for (int m = 0; m < moves + 2 * NumSquares; m++)
			{
				const Move *move = (m < moves) ? piece->move(m) : piece->move((m - moves < NumSquares) ? (square_t)(m - moves) : UndefinedSquare, ((m - moves < NumSquares) ? UndefinedSquare : (square_t)(m - moves - NumSquares)));
				if (!move->mandatory() || indeterminate(move->mandatory()))
					continue;

				if (!move->to().isValid() && from[move->from()])
					continue;
				if (!move->from().isValid() && to[move->to()])
					continue;

				if (move->from().isValid())
					from[move->from()] = true;
				if (move->to().isValid())
					to[move->to()] = true;

				const Constraints *constraints = move->constraints();
				if (!constraints)
					continue;
				
				fprintf(file, "%s move %s       [%2d, %2d]\n", strings::xcolors[piece->color()], PrintMove(move), move->earliest(), move->latest());

				for (Color xcolor = FirstColor; xcolor < NumColors; xcolor++)
				{
					for (Man xman = FirstMan; xman < NumMen; xman++)
					{
						const Constraint *constraint = constraints->constraint(xcolor, xman);
						if (!constraint)
							continue;

						if (constraint->follows())
							fprintf(file, "\tFollows  %s\n", PrintMove(constraint->follows()));

						if (constraint->precedes())
							fprintf(file, "\tPrecedes %s\n", PrintMove(constraint->precedes()));
					}
				}
			}
		}
	}

	fflush(file);
}


/* -------------------------------------------------------------------------- */

}
