#include "obstructions.h"
#include "moves.h"

namespace euclide 
{

/* -------------------------------------------------------------------------- */

Obstructions::Obstructions(Superman superman, Color color, Square square, Glyph glyph, Move movements[NumSquares][NumSquares])
{
	assert(superman.isValid());
	assert(color.isValid());
	assert(square.isValid());

	/* -- The blocking glyph is unimportant if of the same color than the blocked man -- */

	if (glyph.color() == color)
		glyph = NoGlyph;

	/* -- Allocate obstruction table from precomputed table -- */

	const tables::Obstruction *obstructions = tables::obstructions[superman.glyph(color)][square].obstructions;
	int numObstructions = tables::obstructions[superman.glyph(color)][square].numObstructions;
	
	_obstructions = new Move *[numObstructions];

	/* -- Soft obstructions are used for pieces captured on the obstruction square -- */

	_numSoftObstructions = 0;
	for (int k = 0; k < numObstructions; k++)
		if (obstructions[k].to != square)
			if (!obstructions[k].royal || glyph.isKing())
				if (!obstructions[k].check || tables::checks[obstructions[k].to][glyph][square])
					_obstructions[_numSoftObstructions++] = &movements[obstructions[k].from][obstructions[k].to];

	/* -- Hard obstructions are suitable only if the obstructing piece is not captured -- */

	_numHardObstructions = _numSoftObstructions;
	for (int k = 0; k < numObstructions; k++)
		if (obstructions[k].to == square)
			_obstructions[_numHardObstructions++] = &movements[obstructions[k].from][obstructions[k].to];
}

/* -------------------------------------------------------------------------- */

Obstructions::Obstructions(const Obstructions& obstructions)
{
	_numSoftObstructions = obstructions._numSoftObstructions;
	_numHardObstructions = obstructions._numHardObstructions;

	_obstructions = new Move *[_numHardObstructions];
	std::copy(obstructions._obstructions, obstructions._obstructions + _numHardObstructions, _obstructions);
}

/* -------------------------------------------------------------------------- */

Obstructions::~Obstructions()
{
	delete[] _obstructions;
}

/* -------------------------------------------------------------------------- */

Obstructions& Obstructions::operator&=(const Obstructions& obstructions)
{
	int k = 0;
	
	/* -- Find common obstructions -- */

	int m = 0;
	int n = obstructions._numSoftObstructions;

	for (int i = 0; i < _numHardObstructions; i++)
	{
		if (i == _numSoftObstructions)
		{
			m = 0;
			n = obstructions._numSoftObstructions;
		}

		while ((m < obstructions._numSoftObstructions) && (_obstructions[i] > obstructions._obstructions[m]))
			m++;

		while ((n < obstructions._numHardObstructions) && (_obstructions[i] > obstructions._obstructions[n]))
			n++;

		if (m < obstructions._numSoftObstructions)
			if (_obstructions[i] == obstructions._obstructions[m])
				_obstructions[k++] = obstructions._obstructions[m++];

		if (n < obstructions._numHardObstructions)
			if (_obstructions[i] == obstructions._obstructions[n])
				_obstructions[k++] = obstructions._obstructions[n++];
	}

	/* -- Common obstructions are labelled as 'hard' -- */

	_numSoftObstructions = 0;
	_numHardObstructions = k;
	return *this;
}

/* -------------------------------------------------------------------------- */

int Obstructions::block(bool captured) const
{
	int numObstructions = obstructions(captured);

	for (int k = 0; k < numObstructions; k++)
		_obstructions[k]->block();

	return numObstructions;
}

/* -------------------------------------------------------------------------- */

int Obstructions::unblock(bool captured) const
{
	int numObstructions = obstructions(captured);

	for (int k = 0; k < numObstructions; k++)
		_obstructions[k]->unblock();

	return numObstructions;
}

/* -------------------------------------------------------------------------- */

void Obstructions::optimize()
{
	int k, n;

	/* -- Remove from the obstruction table all useless entries -- */

	for (k = 0, n = 0; k < _numSoftObstructions; n += _obstructions[k++]->possible() ? 1 : 0)
		_obstructions[n] = _obstructions[k];

	_numSoftObstructions = n;

	for ( ; k < _numHardObstructions; n += _obstructions[k++]->possible() ? 1 : 0)
		_obstructions[n] = _obstructions[k];

	_numHardObstructions = n;
}

/* -------------------------------------------------------------------------- */

}