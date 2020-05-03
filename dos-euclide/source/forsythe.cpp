#include "forsythe.h"
#include "strings.h"

/* -------------------------------------------------------------------------- */
/* -- ForsytheSymbols                                                      -- */
/* -------------------------------------------------------------------------- */

ForsytheSymbols::ForsytheSymbols(const wchar_t *symbols)
{
	std::fill_n(m_glyphs, countof(m_glyphs), EUCLIDE_GLYPH_NONE);

	assert(wcslen(symbols) >= 6);
	m_glyphs[toupper(symbols[0])] = EUCLIDE_GLYPH_WHITE_KING;
	m_glyphs[tolower(symbols[0])] = EUCLIDE_GLYPH_BLACK_KING;
	m_glyphs[toupper(symbols[1])] = EUCLIDE_GLYPH_WHITE_QUEEN;
	m_glyphs[tolower(symbols[1])] = EUCLIDE_GLYPH_BLACK_QUEEN;
	m_glyphs[toupper(symbols[2])] = EUCLIDE_GLYPH_WHITE_ROOK;
	m_glyphs[tolower(symbols[2])] = EUCLIDE_GLYPH_BLACK_ROOK;
	m_glyphs[toupper(symbols[3])] = EUCLIDE_GLYPH_WHITE_BISHOP;
	m_glyphs[tolower(symbols[3])] = EUCLIDE_GLYPH_BLACK_BISHOP;
	m_glyphs[toupper(symbols[4])] = EUCLIDE_GLYPH_WHITE_KNIGHT;
	m_glyphs[tolower(symbols[4])] = EUCLIDE_GLYPH_BLACK_KNIGHT;
	m_glyphs[toupper(symbols[5])] = EUCLIDE_GLYPH_WHITE_PAWN;
	m_glyphs[tolower(symbols[5])] = EUCLIDE_GLYPH_BLACK_PAWN;
}

/* -------------------------------------------------------------------------- */
/* -- ForsytheString                                                       -- */
/* -------------------------------------------------------------------------- */

ForsytheString::ForsytheString(const Strings& strings, const char *string, int numHalfMoves)
{
	/* -- Reset problem -- */

	memset(&m_problem, 0, sizeof(m_problem));
	m_valid = false;

	/* -- Basic coherency checks -- */

	if (!string || (numHalfMoves <= 0))
		return;

	/* -- Check multiple languages for diagram description -- */

	for (const wchar_t *symbols = strings[Strings::ForsytheSymbols]; *symbols && !m_valid; symbols += 7)
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

				m_problem.diagram[8 * column + row] = glyph;
				column++;
			}
		}

		if (valid && (numWhiteKings == 1) && (numBlackKings == 1))
			m_valid = true;
	}

	/* -- Set number of moves -- */

	m_problem.numHalfMoves = numHalfMoves;
}

/* -------------------------------------------------------------------------- */

