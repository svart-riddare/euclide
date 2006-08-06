#ifndef __EUCLIDE_MAKETABLESLIST_H
#define __EUCLIDE_MAKETABLESLIST_H

#include <cstddef>

// ----------------------------------------------------------------------------

void makeSupermanToGlyph(void);
void makeInitialDistances(void);
void makeInitialCaptures(void);
void makeInitialSquares(void);
void makeObstructions(void);
void makeMovements(void);

// ----------------------------------------------------------------------------

typedef void (*makefunction)(void);

const makefunction MakeFunctions[] =
{
	makeSupermanToGlyph,
	makeInitialDistances,
	makeInitialCaptures,
	makeInitialSquares,
	makeObstructions,
	makeMovements,
	NULL
};

// ----------------------------------------------------------------------------

#endif
