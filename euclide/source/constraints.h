#ifndef __EUCLIDE_CONSTRAINTS_H
#define __EUCLIDE_CONSTRAINTS_H

#include "includes.h"

namespace euclide
{

class Move;
class Piece;

/* -------------------------------------------------------------------------- */

/**
 * \class Constraint
 * Constraints imposed by a piece on a given move.
 *
 * \class Constraints
 * Constraints imposed by all pieces on a given move.
 */

/* -------------------------------------------------------------------------- */

class Constraint
{
	public :
		Constraint(const Piece *piece);

		bool mustFollow(const Move *move);
		bool mustPreceed(const Move *move);

	public :
		int earliest(int offset) const;
		int latest(int offset) const;

		bool fatal() const;

	private :
		const Piece *_piece;       /**< Constraining piece. */

		const Move *_follows;      /**< Constrained move must be played after this move. NULL if none. */
		const Move *_preceedes;    /**< Constrained move must be played before this move. NULL if none. */
};

/* -------------------------------------------------------------------------- */

class Constraints
{
	public :
		Constraints(Move *move);
		~Constraints();
		
		bool apply(void);

		bool mustFollow(const Piece *piece, const Move *move, bool recursive = true);
		bool mustPreceed(const Piece *piece, const Move *move, bool recursive = true);

	private :
		Constraint *constraint(const Piece *piece);

	private :
		Constraint *_constraints[NumColors][NumMen];    /**< Constraints. */
		Move *_move;                                    /**< Contrained move. */
};

/* -------------------------------------------------------------------------- */

}

#endif

