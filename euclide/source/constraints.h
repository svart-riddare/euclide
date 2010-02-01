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
		Constraint(Piece *piece);

		bool mustFollow(Move *move);
		bool mustPreceed(Move *move);
		bool mustFollow(int moves);
		bool mustPreceed(int moves);

	public :
		int earliest(int offset) const;
		int latest(int offset) const;

	private :
		Piece *_piece;       /**< Constraining piece. */

		Move *_follows;      /**< Constrained move must be played after this move. NULL if none. */
		Move *_preceedes;    /**< Constrained move must be played before this move. NULL if none. */

		int _moves;          /**< Number of moves the constrained piece must have played, 0 if none. */
		int _rmoves;         /**< Number of moves the constrained piece must still have to play, 0 if none. */
};

/* -------------------------------------------------------------------------- */

class Constraints
{
	public :
		Constraints(Move *move);
		~Constraints();
		
		bool apply(void);

		bool mustFollow(Piece *piece, Move *move);
		bool mustPreceed(Piece *piece, Move *move);
		bool mustFollow(Piece *piece, int moves);
		bool mustPreceed(Piece *piece, int moves);

	private :
		Constraint *constraint(Piece *piece);

	private :
		Constraint *_constraints[NumColors][NumMen];    /**< Constraints. */
		Move *_move;                                    /**< Contrained move. */
};

/* -------------------------------------------------------------------------- */

}

#endif

