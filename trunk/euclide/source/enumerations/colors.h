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

} color_t;

/* -------------------------------------------------------------------------- */

class Color 
{
	public :
		inline Color() {}
		inline Color(color_t color)
			{ this->color = color; }

		/* ---------------------------------- */

		inline operator color_t() const
			{ return color; }		
		inline operator color_t&()
			{ return color; }

		/* ---------------------------------- */

		inline Color operator++(int)
			{ return (color_t)((int&)color)++; }
		inline Color operator--(int)
			{ return (color_t)((int&)color)--; }

		inline Color& operator++()
			{ ++(int&)color; return *this; }
		inline Color& operator--()
			{ --(int&)color; return *this; }

		/* ---------------------------------- */

		inline Color operator!() const
		{
			if (color == White)
				return Black;

			if (color == Black)
				return White;

			return UndefinedColor;
		}
		
		/* ---------------------------------- */

		bool isValid() const
		{
			return ((color >= FirstColor) && (color <= LastColor));
		}

		/* ---------------------------------- */

	private :
		color_t color;
};

/* -------------------------------------------------------------------------- */

}

#endif
