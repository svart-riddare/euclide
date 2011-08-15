#ifndef __FORSYTHE_H
#define __FORSYTHE_H

#include "includes.h"

/* -------------------------------------------------------------------------- */

class ForsytheSymbols
{
	public : 
		ForsytheSymbols(const char *symbols);
		EUCLIDE_Glyph operator[](char symbol) const;

	private :
		char king;
		char queen;
		char rook;
		char bishop;
		char knight;
		char pawn;
};

/* -------------------------------------------------------------------------- */

class ForsytheString
{
	public :
		ForsytheString(const char *string, int numHalfMoves);
		operator const EUCLIDE_Problem *() const;

	private :
		EUCLIDE_Problem problem;
		bool valid;

	private :
		static list<ForsytheSymbols> symbols;
		static list<ForsytheSymbols> loadSymbols();
		static list<ForsytheSymbols> loadSymbols(const char *symbols);
		static list<ForsytheSymbols> loadSymbols(const wchar_t *symbols);
};

/* -------------------------------------------------------------------------- */

#endif
