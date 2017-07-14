#ifndef __FORSYTHE_H
#define __FORSYTHE_H

#include "includes.h"

/* -------------------------------------------------------------------------- */

class ForsytheSymbols
{
	public : 
		ForsytheSymbols(const wchar_t *symbols);		

		inline EUCLIDE_Glyph operator[](char symbol) const
			{ return _glyphs[std::max('\0', symbol)]; }

	private :
		EUCLIDE_Glyph _glyphs[CHAR_MAX + 1];    /**< Glyphs associated to characters. */
};

/* -------------------------------------------------------------------------- */

class ForsytheString
{
	public :
		ForsytheString(const Strings& strings, const char *string, int numHalfMoves);
		
	public :
		inline operator const EUCLIDE_Problem *() const
			{ return &_problem; }
		inline bool operator!() const
			{ return !_valid; }

	private :
		EUCLIDE_Problem _problem;    /**< Problem description. */
		bool _valid;                 /**< Set if the problem is valid. */
};

/* -------------------------------------------------------------------------- */

#endif
