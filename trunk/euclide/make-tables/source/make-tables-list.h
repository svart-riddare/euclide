#ifndef __EUCLIDE_MAKETABLESLIST_H
#define __EUCLIDE_MAKETABLESLIST_H

#include <cstddef>

// ----------------------------------------------------------------------------

void makeObstructions(void);
void makeMovements(void);
void makeChecks(void);

// ----------------------------------------------------------------------------

typedef void (*makefunction)(void);

const makefunction MakeFunctions[] =
{
	makeObstructions,
	makeMovements,
	makeChecks,
	NULL
};

// ----------------------------------------------------------------------------

#endif
