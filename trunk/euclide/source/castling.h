#ifndef __EUCLIDE_CASTLING_H
#define __EUCLIDE_CASTLING_H

#include "includes.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

class Castling
{
	public :
		Castling();

		bool isNonePossible(Man man) const;
		bool isKingsidePossible(Man man) const;
		bool isQueensidePossible(Man man) const;

		Square kingsideSquare(Man man, Color color) const;
		Square queensideSquare(Man man, Color color) const;

		void setKingsidePossible(bool possible);
		void setQueensidePossible(bool possible);

	private :
		tribool kingside;
		tribool queenside;
};

/* -------------------------------------------------------------------------- */

}

#endif

