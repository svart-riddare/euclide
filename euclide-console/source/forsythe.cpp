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

ForsytheString::ForsytheString(const Strings& strings, const char *string, int numHalfMoves, const char *options)
{
	/* -- Reset problem -- */

	memset(&m_problem, 0, sizeof(m_problem));
	m_valid = false;

	/* -- Basic coherency checks -- */

	if (!string || (numHalfMoves <= 0))
		return;

	/* -- Fetch diagram position and optional initial position -- */

	const char *arrow = strstr(string, "->");
	if (arrow)
		m_valid = parse(strings, string, m_problem.initial) && parse(strings, arrow + 2, m_problem.diagram);
	else
		m_valid = parse(strings, string, m_problem.diagram);
	
	if (!m_valid)
		return;

	/* -- Set number of moves -- */

	m_problem.numHalfMoves = numHalfMoves;

	/* -- Handle options -- */

	if (strstr(options, strings[Strings::Grasshoppers]))
		m_problem.pieces[EUCLIDE_GLYPH_WHITE_QUEEN] = m_problem.pieces[EUCLIDE_GLYPH_BLACK_QUEEN] = EUCLIDE_PIECE_GRASSHOPPER;
	if (strstr(options, strings[Strings::Nightriders]))
		m_problem.pieces[EUCLIDE_GLYPH_WHITE_KNIGHT] = m_problem.pieces[EUCLIDE_GLYPH_BLACK_KNIGHT] = EUCLIDE_PIECE_NIGHTRIDER;

	if (strstr(options, strings[Strings::Knighted]))
	{
		m_problem.pieces[EUCLIDE_GLYPH_WHITE_QUEEN] = m_problem.pieces[EUCLIDE_GLYPH_BLACK_QUEEN] = EUCLIDE_PIECE_AMAZON;
		m_problem.pieces[EUCLIDE_GLYPH_WHITE_ROOK] = m_problem.pieces[EUCLIDE_GLYPH_BLACK_ROOK] = EUCLIDE_PIECE_EMPRESS;
		m_problem.pieces[EUCLIDE_GLYPH_WHITE_BISHOP] = m_problem.pieces[EUCLIDE_GLYPH_BLACK_BISHOP] = EUCLIDE_PIECE_PRINCESS;
	}

	if (strstr(options, strings[Strings::Alfils]))
		m_problem.pieces[EUCLIDE_GLYPH_WHITE_BISHOP] = m_problem.pieces[EUCLIDE_GLYPH_BLACK_BISHOP] = EUCLIDE_PIECE_ALFIL;
	if (strstr(options, strings[Strings::Camels]))
		m_problem.pieces[EUCLIDE_GLYPH_WHITE_KNIGHT] = m_problem.pieces[EUCLIDE_GLYPH_BLACK_KNIGHT] = EUCLIDE_PIECE_CAMEL;
	if (strstr(options, strings[Strings::Zebras]))
		m_problem.pieces[EUCLIDE_GLYPH_WHITE_KNIGHT] = m_problem.pieces[EUCLIDE_GLYPH_BLACK_KNIGHT] = EUCLIDE_PIECE_ZEBRA;

	if (strstr(options, strings[Strings::Chinese]))
	{
		m_problem.pieces[EUCLIDE_GLYPH_WHITE_QUEEN] = m_problem.pieces[EUCLIDE_GLYPH_BLACK_QUEEN] = EUCLIDE_PIECE_LEO;
		m_problem.pieces[EUCLIDE_GLYPH_WHITE_ROOK] = m_problem.pieces[EUCLIDE_GLYPH_BLACK_ROOK] = EUCLIDE_PIECE_PAO;
		m_problem.pieces[EUCLIDE_GLYPH_WHITE_BISHOP] = m_problem.pieces[EUCLIDE_GLYPH_BLACK_BISHOP] = EUCLIDE_PIECE_VAO;
		m_problem.pieces[EUCLIDE_GLYPH_WHITE_KNIGHT] = m_problem.pieces[EUCLIDE_GLYPH_BLACK_KNIGHT] = EUCLIDE_PIECE_MAO;
	}

	if (strstr(options, strings[Strings::Monochromatic]))
		m_problem.variant = EUCLIDE_VARIANT_MONOCHROMATIC;
	else
	if (strstr(options, strings[Strings::Bichromatic]))
		m_problem.variant = EUCLIDE_VARIANT_BICHROMATIC;
	else
	if (strstr(options, strings[Strings::Grid]))
		m_problem.variant = EUCLIDE_VARIANT_GRID_CHESS;
	else
	if (strstr(options, strings[Strings::Cylinder]))
		m_problem.variant = EUCLIDE_VARIANT_CYLINDER_CHESS;
	else
	if (strstr(options, strings[Strings::Glasgow]))
		m_problem.variant = EUCLIDE_VARIANT_GLASGOW_CHESS;
}

/* -------------------------------------------------------------------------- */

bool ForsytheString::parse(const Strings& strings, const char *string, EUCLIDE_Glyph position[64])
{
	/* -- Check multiple languages for diagram description -- */

	assert(wcslen(strings[Strings::ForsytheSymbols]) % 7 == 0);
	for (const wchar_t *symbols = strings[Strings::ForsytheSymbols]; *symbols; symbols += 7)
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
					return false;
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
					return false;

				position[8 * column + row] = glyph;
				column++;
			}
		}

		if (valid && (numWhiteKings == 1) && (numBlackKings == 1))
			return true;
	}

	return false;
}

/* -------------------------------------------------------------------------- */
