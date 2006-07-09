#ifndef __EUCLIDE_MAKETABLESLIST_H
#define __EUCLIDE_MAKETABLESLIST_H

#include <cstddef>

// ----------------------------------------------------------------------------

void makeGlyphToMan(void);
void makeManToGlyph(void);
void makeInitialSquares(void);
void makeMovements(void);

// ----------------------------------------------------------------------------

typedef void (*makefunction)(void);

const makefunction MakeFunctions[] =
{
	makeGlyphToMan,
	makeManToGlyph,
	makeInitialSquares,
	makeMovements,
	NULL
};

// ----------------------------------------------------------------------------

#endif
