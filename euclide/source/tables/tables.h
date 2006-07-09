#ifndef __EUCLIDE_TABLES_H
#define __EUCLIDE_TABLES_H

#include "enumerations.h"

namespace euclide
{
namespace tables
{

/* -------------------------------------------------------------------------- */

extern const bool validGlyphManColor[NumGlyphs][NumMen][NumColors];
extern const Man glyphToMan[NumGlyphs][NumColumns];

/* -------------------------------------------------------------------------- */

extern const Glyph manToGlyph[NumSupermen][NumColors];

/* -------------------------------------------------------------------------- */

extern const Square initialSquares[NumSupermen][NumColors];

/* -------------------------------------------------------------------------- */

extern const bool validMovements[NumGlyphs][NumSquares][NumSquares];

/* -------------------------------------------------------------------------- */

}}

#endif
