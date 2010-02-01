#include "constraints.h"
#include "pieces.h"
#include "moves.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Constraint::Constraint(Piece *piece)
	: _piece(piece)
{
	_follows = NULL;
	_preceedes = NULL;

	_moves = 0;
	_rmoves = 0;
}

/* -------------------------------------------------------------------------- */

bool Constraint::mustFollow(Move *move)
{
	assert(move->mandatory());

	/* -- Check if we already know that -- */

	if (_follows)
		if (move->distance() <= _follows->distance())
			return false;

	/* -- If not, let's not forget it -- */

	_follows = move;

	/* -- Update also the move rank -- */

	mustFollow(move->distance());
	return true;
}

/* -------------------------------------------------------------------------- */

bool Constraint::mustPreceed(Move *move)
{
	assert(move->mandatory());

	/* -- Check if we already know that -- */

	if (_preceedes)
		if (move->rdistance() <= _preceedes->rdistance())
			return false;

	/* -- If not, let's not forget it -- */

	_preceedes = move;

	/* -- Update also the move rank -- */

	mustPreceed(move->rdistance());
	return true;
}

/* -------------------------------------------------------------------------- */

bool Constraint::mustFollow(int moves)
{
	assert(moves > 0);

	/* -- Check if we already know that -- */

	if (moves <= _moves)
		return false;

	/* -- Update state -- */

	_moves = moves;
	return true;
}

/* -------------------------------------------------------------------------- */

bool Constraint::mustPreceed(int moves)
{
	assert(moves > 0);

	/* -- Check if we already know that -- */

	if (moves <= _rmoves)
		return false;

	/* -- Update state -- */

	_rmoves = moves;
	return true;
}

/* -------------------------------------------------------------------------- */

int Constraint::earliest(int offset) const
{ 
	return _moves ? _piece->earliest() + _moves - 1 + offset : 1;
}

/* -------------------------------------------------------------------------- */

int Constraint::latest(int offset) const
{ 
	return _rmoves ? _piece->latest() - _rmoves + 1 - offset : infinity;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

Constraints::Constraints(Move *move)
	: _move(move)
{
	/* -- Initialize content -- */

	for (Color color = FirstColor; color <= LastColor; color++)
		for (Man man = FirstMan; man <= LastMan; man++)
			_constraints[color][man] = NULL;
}

/* -------------------------------------------------------------------------- */

Constraints::~Constraints()
{
	/* -- Delete constraints -- */

	for (Color color = FirstColor; color <= LastColor; color++)
		for (Man man = FirstMan; man <= LastMan; man++)
			delete _constraints[color][man];
}

/* -------------------------------------------------------------------------- */

bool Constraints::apply(void)
{
	int earliest = _move->earliest();
	int latest = _move->latest();

	/* -- Compute contraints -- */

	for (Color color = FirstColor; color <= LastColor; color++)
	{
		int offset = color.offset(_move->color());  // Peut-être le contraire !
		int roffset = _move->color().offset(color);  // Là aussi c'est peut-être le contraire !

		for (Man man = FirstMan; man <= LastMan; man++)
		{
			if (_constraints[color][man])
			{
				maximize(earliest, _constraints[color][man]->earliest(offset));
				minimize(latest, _constraints[color][man]->latest(roffset));
			}
		}
	}

	/* -- Apply constraints -- */

	if ((earliest <= _move->earliest()) && (latest >= _move->latest()))
		return false;

	_move->bound(earliest, latest);
	return true;
}

/* -------------------------------------------------------------------------- */

bool Constraints::mustFollow(Piece *piece, Move *move)
{
	assert(piece == move->piece());
	return constraint(piece)->mustFollow(move);
}

/* -------------------------------------------------------------------------- */

bool Constraints::mustPreceed(Piece *piece, Move *move)
{
	assert(piece == move->piece());
	return constraint(piece)->mustPreceed(move);
}

/* -------------------------------------------------------------------------- */

bool Constraints::mustFollow(Piece *piece, int moves)
{
	return constraint(piece)->mustFollow(moves);
}

/* -------------------------------------------------------------------------- */

bool Constraints::mustPreceed(Piece *piece, int moves)
{
	return constraint(piece)->mustPreceed(moves);
}

/* -------------------------------------------------------------------------- */

Constraint *Constraints::constraint(Piece *piece)
{
	if (!_constraints[piece->color()][piece->man()])
		_constraints[piece->color()][piece->man()] = new Constraint(piece);

	return _constraints[piece->color()][piece->man()];
}

/* -------------------------------------------------------------------------- */

}