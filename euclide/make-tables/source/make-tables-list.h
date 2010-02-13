#ifndef __EUCLIDE_MAKETABLESLIST_H
#define __EUCLIDE_MAKETABLESLIST_H

#include <cstddef>

// ----------------------------------------------------------------------------

void makeObstructions(bool castling);
void makeConstraints(bool castling);
void makeMovements(bool castling);
void makeChecks(bool castling);

// ----------------------------------------------------------------------------

typedef void (*makefunction)(bool castling);

const makefunction MakeFunctions[] =
{
	makeObstructions,
	makeConstraints,
	makeMovements,
	makeChecks,
	NULL
};

// ----------------------------------------------------------------------------

#endif
