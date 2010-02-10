#include "pieces.h"
#include "moves.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Move::Move()
{
	_piece = NULL;

	_from = UndefinedSquare;
	_to = UndefinedSquare;
	_superman = UndefinedSuperman;
	_glyph = UndefinedGlyph;
	_color = UndefinedColor;

	_mandatory = indeterminate;
	_capture = indeterminate;

	_earliest = 0;
	_latest = 0;

	_obstructions = infinity;

	_constraints = NULL;
}

/* -------------------------------------------------------------------------- */

Move::Move(Square from, Square to, Piece *piece, int moves)
{
	initialize(from, to, piece, moves);
}

/* -------------------------------------------------------------------------- */

Move::~Move()
{
	delete _constraints;
}

/* -------------------------------------------------------------------------- */

void Move::initialize(Square from, Square to, Piece *piece, int moves)
{
	assert(from.isValid() || to.isValid());

	_piece = piece;

	_from = from;
	_to = to;
	_superman = piece->superman();
	_glyph = piece->glyph();
	_color = piece->color();
	
	_mandatory = indeterminate;
	_capture = indeterminate;

	_earliest = 1;
	_latest = moves;

	/* -- Finish initialization for true movements -- */

	if (from.isValid() && to.isValid())
	{
		/* -- Is this a valid move or not ? -- */

		_obstructions = (tables::movements[_glyph][from][to] || tables::captures[_glyph][from][to]) ? 0 : infinity;

		/* -- If so, is capture possible or mandatory ? -- */

		if (tables::captures[_glyph][from][to] && !tables::movements[_glyph][from][to])
			_capture = true;
		else
		if (!tables::captures[_glyph][from][to] && tables::movements[_glyph][from][to])
			_capture = false;

		/* -- Store squares that should be free -- */

		const tables::Constraints *constraints = &tables::constraints[_glyph][from][to];
		for	(int k = 0; k < constraints->numConstraints; k++)
			_squares[constraints->constraints[k]] = true;
	}
	else
	if (from.isValid())
	{
		for (Square square = FirstSquare; square <= LastSquare; square++)
			if (tables::movements[_glyph][from][square] || tables::captures[_glyph][from][square])
				_obstructions = 0;
	}
	else
	if (to.isValid())
	{
		for (Square square = FirstSquare; square <= LastSquare; square++)
			if (tables::movements[_glyph][square][to] || tables::captures[_glyph][square][to])
				_obstructions = 0;
	}

	/* -- Departure square should always be free -- */

	if (from.isValid())
		_squares[from] = true;
}

/* -------------------------------------------------------------------------- */

void Move::validate()
{
	_mandatory = true;
}

/* -------------------------------------------------------------------------- */

void Move::invalidate()
{
	_obstructions = infinity;

	if (_mandatory)
		abort(NoSolution);
}

/* -------------------------------------------------------------------------- */

void Move::block()
{
	_obstructions += 1;
}

/* -------------------------------------------------------------------------- */

void Move::unblock()
{
	assert(_obstructions >= 1);
	_obstructions -= 1;
}

/* -------------------------------------------------------------------------- */

void Move::bound(int earliest, int latest)
{
	maximize(_earliest, earliest);
	minimize(_latest, latest);

	if (_earliest > _latest)
		invalidate();
}

/* -------------------------------------------------------------------------- */

Constraints *Move::constraints()
{
	if (!_constraints)
		_constraints = new Constraints(this);

	return _constraints;
}

/* -------------------------------------------------------------------------- */

bool Move::constrain()
{
	if (!_constraints)
		return false;

	return _constraints->apply();
}

/* -------------------------------------------------------------------------- */

int Move::distance() const
{
	if (_to == UndefinedSquare)
		return _piece->distance(_from) + 1;

	return _piece->distance(_to);
}

/* -------------------------------------------------------------------------- */

int Move::rdistance() const
{
	if (_from == UndefinedSquare)
		return _piece->rdistance(_to) + 1;

	return _piece->rdistance(_from);
}

/* -------------------------------------------------------------------------- */

}
