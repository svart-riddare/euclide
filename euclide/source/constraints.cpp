#include "constraints.h"
#include "pieces.h"
#include "moves.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Constraint::Constraint(const Piece *piece)
	: _piece(piece)
{
	_follows = NULL;
	_precedes = NULL;
}

/* -------------------------------------------------------------------------- */

bool Constraint::mustFollow(const Move *move)
{
	assert(move->mandatory());

	/* -- Check if we already know that -- */

	if (_follows)
	{
		if (move->distance() < _follows->distance())
			return false;

		if (move->distance() == _follows->distance())
			if (move->incomplete() || !_follows->incomplete())
				return false;
	}

	/* -- If not, let's not forget it -- */

	_follows = move;

	/* -- Done -- */

	return true;
}

/* -------------------------------------------------------------------------- */

bool Constraint::mustPreceed(const Move *move)
{
	assert(move->mandatory());

	/* -- Check if we already know that -- */

	if (_precedes)
	{
		if (move->rdistance() < _precedes->rdistance())
			return false;

		if (move->rdistance() == _precedes->rdistance())
			if (move->incomplete() || !_precedes->incomplete())
				return false;
	}

	/* -- If not, let's not forget it -- */

	_precedes = move;

	/* -- Done -- */

	return true;
}

/* -------------------------------------------------------------------------- */

int Constraint::earliest(int offset) const
{
	return _follows ? _piece->earliest() + _follows->distance() - 1 + offset : 1;
}

/* -------------------------------------------------------------------------- */

int Constraint::latest(int offset) const
{ 
	return _precedes ? _piece->latest() - _precedes->rdistance() + 1 - offset : infinity;
}

/* -------------------------------------------------------------------------- */

bool Constraint::fatal() const
{
	return (_follows && _precedes && (_follows == _precedes)) ? true : false;
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

	/* -- Early exit -- */

	if (!_move->possible())
		return false;

	/* -- Check for impossibilities -- */

	for (Color color = FirstColor; color <= LastColor; color++)
		for (Man man = FirstMan; man <= LastMan; man++)
			if (_constraints[color][man] && _constraints[color][man]->fatal())
				_move->invalidate();

	if (!_move->possible())
		return true;

	/* -- Compute contraints -- */

	for (Color color = FirstColor; color <= LastColor; color++)
	{
		int offset = color.offset(_move->color());
		int roffset = _move->color().offset(color);

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

bool Constraints::mustFollow(const Piece *piece, const Move *move, bool recursive)
{
	assert(piece == move->piece());
	
	/* -- Set constraint -- */

	bool modified = constraint(piece)->mustFollow(move);
	
	/* -- Set symetrical constraint -- */

	if (recursive && _move->mandatory())
		if (const_cast<Move *>(move)->constraints()->mustPreceed(_move->piece(), _move, false))
			modified = true;

	/* -- Done -- */

	return modified;
}

/* -------------------------------------------------------------------------- */

bool Constraints::mustPreceed(const Piece *piece, const Move *move, bool recursive)
{
	assert(piece == move->piece());
	
	bool modified = constraint(piece)->mustPreceed(move);
	if (recursive && _move->mandatory())
		if (const_cast<Move *>(move)->constraints()->mustFollow(_move->piece(), _move, false))
			modified = true;

	return modified;
}

/* -------------------------------------------------------------------------- */

Constraint *Constraints::constraint(const Piece *piece)
{
	if (!_constraints[piece->color()][piece->man()])
		_constraints[piece->color()][piece->man()] = new Constraint(piece);

	return _constraints[piece->color()][piece->man()];
}

/* -------------------------------------------------------------------------- */

}