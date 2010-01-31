#include "constraints.h"
#include "moves.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

FollowsMandatoryMoveConstraint::FollowsMandatoryMoveConstraint(const Move *move)
{
	assert(move->mandatory());

	_move = move;
}

/* -------------------------------------------------------------------------- */

bool FollowsMandatoryMoveConstraint::apply(Move& move)
{
	int earliest = _move->earliest() + _move->offset(move);
	int latest = move.latest();

	if (earliest <= move.earliest())
		return false;

	move.bound(earliest, latest);
	return true;
}

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

PreceedesMandatoryMoveConstraint::PreceedesMandatoryMoveConstraint(const Move *move)
{
	assert(move->mandatory());

	_move = move;
}

/* -------------------------------------------------------------------------- */

bool PreceedesMandatoryMoveConstraint::apply(Move& move)
{
	int earliest = move.earliest();
	int latest = _move->latest() - _move->roffset(move);

	if (latest >= move.latest())
		return false;

	move.bound(earliest, latest);
	return true;
}

/* -------------------------------------------------------------------------- */

}