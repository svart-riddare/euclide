#include "forsythe.h"
#include "strings.h"

/* -------------------------------------------------------------------------- */
/* -- ForsytheSymbols                                                      -- */
/* -------------------------------------------------------------------------- */

ForsytheSymbols::ForsytheSymbols(const wchar_t *symbols)
{
	std::fill_n(_glyphs, countof(_glyphs), EUCLIDE_GLYPH_NONE);

	assert(wcslen(symbols) >= 6);
	_glyphs[toupper(symbols[0])] = EUCLIDE_GLYPH_WHITE_KING;
	_glyphs[tolower(symbols[0])] = EUCLIDE_GLYPH_BLACK_KING;
	_glyphs[toupper(symbols[1])] = EUCLIDE_GLYPH_WHITE_QUEEN;
	_glyphs[tolower(symbols[1])] = EUCLIDE_GLYPH_BLACK_QUEEN;
	_glyphs[toupper(symbols[2])] = EUCLIDE_GLYPH_WHITE_ROOK;
	_glyphs[tolower(symbols[2])] = EUCLIDE_GLYPH_BLACK_ROOK;
	_glyphs[toupper(symbols[3])] = EUCLIDE_GLYPH_WHITE_BISHOP;
	_glyphs[tolower(symbols[3])] = EUCLIDE_GLYPH_BLACK_BISHOP;
	_glyphs[toupper(symbols[4])] = EUCLIDE_GLYPH_WHITE_KNIGHT;
	_glyphs[tolower(symbols[4])] = EUCLIDE_GLYPH_BLACK_KNIGHT;
	_glyphs[toupper(symbols[5])] = EUCLIDE_GLYPH_WHITE_PAWN;
	_glyphs[tolower(symbols[5])] = EUCLIDE_GLYPH_BLACK_PAWN;
}

/* -------------------------------------------------------------------------- */
/* -- ForsytheString                                                       -- */
/* -------------------------------------------------------------------------- */

ForsytheString::ForsytheString(const Strings& strings, const char *string, int numHalfMoves)
{
	/* -- Reset problem -- */

	memset(&_problem, 0, sizeof(_problem));
	_valid = false;

	/* -- Basic coherency checks -- */

	if (!string || (numHalfMoves <= 0))
		return;

	/* -- Check multiple languages -- */

	for (const wchar_t *symbols = strings[Strings::ForsytheSymbols]; *symbols && !_valid; symbols += 7)
	{
		const ForsytheSymbols forsythe(symbols);

		/* -- Parse Forsythe string -- */

		int numWhiteKings = 0;
		int numBlackKings = 0;

		int column = 0;
		int row = 7;

		bool spaces = true;
		bool valid = true;

		for (const char *s = string; *s && valid; s++)
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

				if (!row--)
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
				const EUCLIDE_Glyph glyph = forsythe[*s];

				if (glyph == EUCLIDE_GLYPH_WHITE_KING)
					numWhiteKings++;

				if (glyph == EUCLIDE_GLYPH_BLACK_KING)
					numBlackKings++;

				if (glyph == EUCLIDE_GLYPH_NONE)
					valid = false;

				spaces = false;
				if (column > 7)
					return;

				_problem.diagram[8 * column + row] = glyph;
				column++;
			}
		}

		if (valid && (numWhiteKings == 1) && (numBlackKings == 1))
			_valid = true;
	}
}

/* -------------------------------------------------------------------------- */

