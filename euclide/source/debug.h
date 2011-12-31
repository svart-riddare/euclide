#ifndef __EUCLIDE_DEBUG_H
#define __EUCLIDE_DEBUG_H

#include "includes.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

FILE *DebugFile();

void DebugConstraints(const Board& board, FILE *file);

/* -------------------------------------------------------------------------- */

}

#endif
