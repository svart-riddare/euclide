#ifndef __EUCLIDE_TABLES_H
#define __EUCLIDE_TABLES_H

#include "enumerations.h"

namespace euclide
{
namespace tables
{

/* -------------------------------------------------------------------------- */

extern const Glyph supermanToGlyph[NumSupermen][NumColors];

/* -------------------------------------------------------------------------- */

extern const Square initialSquares[NumSupermen][NumColors];

/* -------------------------------------------------------------------------- */

extern const bool validMovements[NumGlyphs][NumSquares][NumSquares];

/* -------------------------------------------------------------------------- */

}}

#endif
