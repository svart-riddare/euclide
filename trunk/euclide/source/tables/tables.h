#ifndef __EUCLIDE_TABLES_H
#define __EUCLIDE_TABLES_H

#include "enumerations.h"
#include <utility>

namespace euclide
{
namespace tables
{

/* -------------------------------------------------------------------------- */

extern const bool movements[NumGlyphs][NumSquares][NumSquares];
extern const bool captures[NumGlyphs][NumSquares][NumSquares];

/* -------------------------------------------------------------------------- */

typedef struct
{
	square_t from;
	square_t to;

} Obstruction;

typedef struct
{
	const Obstruction *obstructions;
	int numObstructions;

} Obstructions;

extern const Obstructions obstructions[NumGlyphs][NumSquares];

/* -------------------------------------------------------------------------- */

}}

#endif
