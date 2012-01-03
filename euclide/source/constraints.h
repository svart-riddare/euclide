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

	public :
		inline const Piece *piece() const
			{ return _piece; }
		inline const Move *follows() const
			{ return _follows; }
		inline const Move *precedes() const
			{ return _precedes; }

	private :
		const Piece *_piece;       /**< Constraining piece. */

		const Move *_follows;      /**< Constrained move must be played after this move. NULL if none. */
		const Move *_precedes;     /**< Constrained move must be played before this move. NULL if none. */
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

	public :
		inline const Constraint *constraint(Color color, Man man) const
			{ return _constraints[color][man]; }
		inline const Move *follows(Color color, Man man) const
			{ return constraint(color, man) ? constraint(color, man)->follows() : NULL; }
		inline const Move *precedes(Color color, Man man) const
			{ return constraint(color, man) ? constraint(color, man)->precedes() : NULL; }

		inline const Move *move() const
			{ return _move; }

	private :
		Constraint *constraint(const Piece *piece);

	private :
		Constraint *_constraints[NumColors][NumMen];    /**< Constraints. */
		Move *_move;                                    /**< Contrained move. */
};

/* -------------------------------------------------------------------------- */

}

#endif

 