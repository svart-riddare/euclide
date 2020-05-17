#ifndef __FORSYTHE_H
#define __FORSYTHE_H

#include "includes.h"

/* -------------------------------------------------------------------------- */

class ForsytheSymbols
{
	public:
		ForsytheSymbols(const wchar_t *symbols);

		inline EUCLIDE_Glyph operator[](char symbol) const
			{ return m_glyphs[std::max<int>('\0', symbol)]; }

	private:
		EUCLIDE_Glyph m_glyphs[CHAR_MAX + 1];    /**< Glyphs associated to characters. */
};

/* -------------------------------------------------------------------------- */

class ForsytheString
{
	public:
		ForsytheString(const Strings& strings, const char *string, int numHalfMoves);

	public:
		inline operator const EUCLIDE_Problem *() const
			{ return &m_problem; }
		inline bool operator!() const
			{ return !m_valid; }

	private:
		EUCLIDE_Problem m_problem;    /**< Problem description. */
		bool m_valid;                 /**< Set if the problem is valid. */
};

/* -------------------------------------------------------------------------- */

#endif
