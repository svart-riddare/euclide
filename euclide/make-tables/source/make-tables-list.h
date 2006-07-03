#ifndef __EUCLIDE_MAKETABLESLIST_H
#define __EUCLIDE_MAKETABLESLIST_H

#include <cstddef>

// ----------------------------------------------------------------------------

void makeGlyphToMan(void);

// ----------------------------------------------------------------------------

typedef void (*makefunction)(void);

const makefunction MakeFunctions[] =
{
	makeGlyphToMan,
	NULL
};

// ----------------------------------------------------------------------------

#endif
