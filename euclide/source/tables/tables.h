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

extern const int initialDistances[NumSupermen][NumSquares][NumColors];
extern const int initialCaptures[NumSupermen][NumSquares][NumColors];

/* -------------------------------------------------------------------------- */

extern const bool validMovements[NumGlyphs][NumSquares][NumSquares];
extern const bool validCaptures[NumGlyphs][NumSquares][NumSquares];

/* -------------------------------------------------------------------------- */

extern const bool *obstructions[];
extern const bool *whiteObstructions[];
extern const bool *blackObstructions[];

extern const int numObstructions;
extern const int numWhiteObstructions;
extern const int numBlackObstructions;

/* -------------------------------------------------------------------------- */

}}

#endif
