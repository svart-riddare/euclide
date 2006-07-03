#ifndef __EUCLIDE_COLORS_H
#define __EUCLIDE_COLORS_H

#include "errors.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

typedef enum
{
	White, Black,

	NumColors, UndefinedColor = -1,
	FirstColor = White, LastColor = Black,

} Color;

/* -------------------------------------------------------------------------- */

inline bool isValidColor(Color color)
{
	return ((color >= FirstColor) && (color <= LastColor));
}

/* -------------------------------------------------------------------------- */

}

#endif
