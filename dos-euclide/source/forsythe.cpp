#include "forsythe.h"

/* -------------------------------------------------------------------------- */

list<ForsytheSymbols> ForsytheString::symbols = ForsytheString::loadSymbols();

/* -------------------------------------------------------------------------- */

list<ForsytheSymbols> ForsytheString::loadSymbols()
{
	char symbols[256];
	symbols[0] = '\0';

	LoadStringA(NULL, IDS_FORSYTHE, symbols, sizeof(symbols) / sizeof(symbols[0]));

	return loadSymbols(symbols);
}

/* -------------------------------------------------------------------------- */

list<ForsytheSymbols> ForsytheString::loadSymbols(const char *symbols)
{
	list<ForsytheSymbols> list;
	
	while (symbols && (strlen(symbols) >= 6))
	{
		bool valid = true;

		for (int i = 0; i < 6; i++)
			if (!isalpha(symbols[i]))
				valid = false;

		if (valid)
			list.push_back(ForsytheSymbols(symbols));

		symbols += 6;
		if (*symbols)
			symbols += 1;
	}

	return list;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

ForsytheSymbols::ForsytheSymbols(const char *symbols)
{
	king = queen = rook = bishop = knight = pawn = '\0';

	if (symbols)
		if ((king = toupper(symbols[0])) != '\0')
			if ((queen = toupper(symbols[1])) != '\0')
				if ((rook = toupper(symbols[2])) != '\0')
					if ((bishop = toupper(symbols[3])) != '\0')
						if ((knight = toupper(symbols[4])) != '\0')
							pawn = toupper(symbols[5]);
}

/* -------------------------------------------------------------------------- */

EUCLIDE_Glyph ForsytheSymbols::operator[](char symbol) const
{
	if (toupper(symbol) == king)
		return (symbol == king) ? EUCLIDE_GLYPH_WHITE_KING : EUCLIDE_GLYPH_BLACK_KING;

	if (toupper(symbol) == queen)
		return (symbol == queen) ? EUCLIDE_GLYPH_WHITE_QUEEN : EUCLIDE_GLYPH_BLACK_QUEEN;

	if (toupper(symbol) == rook)
		return (symbol == rook) ? EUCLIDE_GLYPH_WHITE_ROOK : EUCLIDE_GLYPH_BLACK_ROOK;

	if (toupper(symbol) == bishop)
		return (symbol == bishop) ? EUCLIDE_GLYPH_WHITE_BISHOP : EUCLIDE_GLYPH_BLACK_BISHOP;

	if (toupper(symbol) == knight)
		return (symbol == knight) ? EUCLIDE_GLYPH_WHITE_KNIGHT : EUCLIDE_GLYPH_BLACK_KNIGHT;

	if (toupper(symbol) == pawn)
		return (symbol == pawn) ? EUCLIDE_GLYPH_WHITE_PAWN : EUCLIDE_GLYPH_BLACK_PAWN;

	return EUCLIDE_GLYPH_NONE;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

ForsytheString::ForsytheString(const char *string, int numHalfMoves)
{
	valid = false;

	problem.numHalfMoves = numHalfMoves;
	std::fill_n(problem.glyphs, 64, EUCLIDE_GLYPH_NONE);

	if (numHalfMoves <= 0)
		return;

	if (!string)
		return;

	for (list<ForsytheSymbols>::const_iterator I = symbols.begin(); (I != symbols.end()) && !valid; I++)
	{
		const ForsytheSymbols symbols = *I;
		const char *s = string;

		int numWhiteKings = 0;
		int numBlackKings = 0;

		int column = 0;
		int row = 7;

		bool spaces = true;
		bool valid = true;

		while (*s && valid)
		{
			if (isdigit(*s))
			{
				column += (*s - '0');
				spaces = false;
			}
			else
			if (*s == '/')
			{
				spaces = false;
				column = 0;

				if (row-- == 0)
					return;
			}
			else
			if (isspace(*s))
			{
				if (!spaces)
					break;
			}
			else
			if (isalpha(*s))
			{
				EUCLIDE_Glyph glyph = symbols[*s];
				int square = 8 * column + row;

				if (glyph == EUCLIDE_GLYPH_WHITE_KING)
					numWhiteKings++;

				if (glyph == EUCLIDE_GLYPH_BLACK_KING)
					numBlackKings++;

				if (glyph == EUCLIDE_GLYPH_NONE)
					valid = false;

				spaces = false;
				if (column > 7)
					return;

				problem.glyphs[8 * column + row] = glyph;
				column++;
			}

			s++;
		}

		if (valid && (numWhiteKings == 1) && (numBlackKings == 1))
			this->valid = true;
	}
}

/* -------------------------------------------------------------------------- */

ForsytheString::operator const EUCLIDE_Problem *() const
{
	if (!valid)
		return NULL;

	return &problem;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

