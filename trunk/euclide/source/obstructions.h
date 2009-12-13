#ifndef __EUCLIDE_OBSTRUCTIONS_H
#define __EUCLIDE_OBSTRUCTIONS_H

#include "includes.h"

namespace euclide
{

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
		Obstructions(Superman superman, Color color, Square square, Glyph glyph, int movements[NumSquares][NumSquares]);
		Obstructions(const Obstructions& obstructions);
		~Obstructions();

		Obstructions& operator&=(const Obstructions& obstructions);

		void block(bool soft) const;
		void unblock(bool soft) const;

		void optimize();

	public :
		int numObstructions(bool soft) const;

	private :
		int numHardObstructions;    /**< Number of obstructions, except moves by a piece of opposite color that ends on the obstruction square (thus allowing capture of the blocking figure). */
		int numSoftObstructions;    /**< Total number of obstructions. */

		int **obstructions;         /**< Obstruction list. An obstruction is represented as a pointer in the valid movements table of the blocked piece. */
};

/* -------------------------------------------------------------------------- */

}

#endif
