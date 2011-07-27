#ifndef __EUCLIDE_OBSTRUCTIONS_H
#define __EUCLIDE_OBSTRUCTIONS_H

#include "includes.h"

namespace euclide
{

class Move;

/* -------------------------------------------------------------------------- */

/**
 * \class Obstructions
 * List of obstructions to movements of a given piece caused by the blocking
 * of a given square by a given figure. The figure causing the blocking does
 * not mather except when it is a king as unmaskable checks are taken into
 * accounts.
 */

/* -------------------------------------------------------------------------- */

class Obstructions
{
	public :
		Obstructions(Superman superman, Color color, Square square, Glyph glyph, Move movements[NumSquares][NumSquares]);
		Obstructions(const Obstructions& obstructions);
		~Obstructions();

		Obstructions& operator&=(const Obstructions& obstructions);

		int block(bool captured = false) const;
		int unblock(bool captured = false) const;

		void optimize();

	public :
		inline int obstructions(bool captured = false) const
			{ return captured ? _numSoftObstructions : _numHardObstructions; }
		inline Move *operator[](int obstruction) const 
			{ return _obstructions[obstruction]; }

	private :
		int _numHardObstructions;    /**< Number of obstructions, except moves by a piece of opposite color that ends on the obstruction square (thus allowing capture of the blocking figure). */
		int _numSoftObstructions;    /**< Total number of obstructions. */

		Move **_obstructions;        /**< Obstruction list. An obstruction is represented as a pointer in the valid movements table of the blocked piece. */
};

/* -------------------------------------------------------------------------- */

}

#endif