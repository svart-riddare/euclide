#ifndef __EUCLIDE_CONSTRAINTS_H
#define __EUCLIDE_CONSTRAINTS_H

#include "includes.h"

namespace euclide
{

class Move;

/* -------------------------------------------------------------------------- */

/**
 * \class Constraint
 * Abstract representation of a constraint imposed by a move.
 *
 * \class FollowsMandatoryMoveConstraint
 * Constrained move must be played after another mandatory move.
 *
 * \class PreceedesMandatoryMoveConstraint
 * Constrained move must be played before another mandatory move.
 */

/* -------------------------------------------------------------------------- */

class Constraint
{
	public : 
		virtual bool apply(Move& move) = 0;
};

/* -------------------------------------------------------------------------- */

typedef vector_ptr<Constraint> Constraints;

/* -------------------------------------------------------------------------- */

class FollowsMandatoryMoveConstraint : public Constraint
{
	public :
		FollowsMandatoryMoveConstraint(const Move *move);
		
		virtual bool apply(Move& move);

	private :
		const Move *_move;    /**< Move that should have already been played. */
};

/* -------------------------------------------------------------------------- */

class PreceedesMandatoryMoveConstraint : public Constraint
{
	public :
		PreceedesMandatoryMoveConstraint(const Move *move);
		
		virtual bool apply(Move& move);

	private :
		const Move *_move;    /**< Move that must not already have been played. */
};

/* -------------------------------------------------------------------------- */

}

#endif

