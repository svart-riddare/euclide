#include "obstructions.h"

namespace euclide 
{

/* -------------------------------------------------------------------------- */

Obstructions::Obstructions(Superman superman, Color color, Square square, Glyph glyph, int movements[NumSquares][NumSquares])
{
	assert(superman.isValid());
	assert(color.isValid());
	assert(square.isValid());

	/* -- The blocking glyph is unimportant if of the same color than the blocked man -- */

	if (glyph.color() == color)
		glyph = NoGlyph;

	/* -- Allocate obstruction table from precomputed table -- */

	const tables::Obstruction *_obstructions = tables::obstructions[superman.glyph(color)][square].obstructions;
	int numObstructions = tables::obstructions[superman.glyph(color)][square].numObstructions;
	
	obstructions = new int *[numObstructions];

	/* -- Soft obstructions are used for pieces captured on the obstruction square -- */

	numSoftObstructions = 0;
	for (int k = 0; k < numObstructions; k++)
		if (_obstructions[k].to != square)
			if (!_obstructions[k].royal || glyph.isKing())
				if (!_obstructions[k].check || tables::checks[_obstructions[k].to][glyph][square])
					obstructions[numSoftObstructions++] = &movements[_obstructions[k].from][_obstructions[k].to];

	/* -- Hard obstructions are suitable only if the obstructing piece is not captured -- */

	numHardObstructions = numSoftObstructions;
	for (int k = 0; k < numObstructions; k++)
		if (_obstructions[k].to == square)
			obstructions[numHardObstructions++] = &movements[_obstructions[k].from][_obstructions[k].to];
}

/* -------------------------------------------------------------------------- */

Obstructions::Obstructions(const Obstructions& obstructions)
{
	numSoftObstructions = obstructions.numSoftObstructions;
	numHardObstructions = obstructions.numHardObstructions;

	this->obstructions = new int *[numHardObstructions];
	std::copy(obstructions.obstructions, obstructions.obstructions + numHardObstructions, this->obstructions);
}

/* -------------------------------------------------------------------------- */

Obstructions::~Obstructions()
{
	delete[] obstructions;
}

/* -------------------------------------------------------------------------- */

Obstructions& Obstructions::operator&=(const Obstructions& obstructions)
{
	int k = 0;
	
	/* -- Find common obstructions -- */

	int m = 0;
	int n = obstructions.numSoftObstructions;

	for (int i = 0; i < this->numHardObstructions; i++)
	{
		if (i == this->numSoftObstructions)
		{
			m = 0;
			n = obstructions.numSoftObstructions;
		}

		while ((m < obstructions.numSoftObstructions) && (this->obstructions[i] > obstructions.obstructions[m]))
			m++;

		while ((n < obstructions.numHardObstructions) && (this->obstructions[i] > obstructions.obstructions[n]))
			n++;

		if (m < obstructions.numSoftObstructions)
			if (this->obstructions[i] == obstructions.obstructions[m])
				this->obstructions[k++] = obstructions.obstructions[m++];

		if (n < obstructions.numHardObstructions)
			if (this->obstructions[i] == obstructions.obstructions[n])
				this->obstructions[k++] = obstructions.obstructions[n++];
	}

	/* -- Common obstructions are labelled as 'hard' -- */

	numSoftObstructions = 0;
	numHardObstructions = k;
	return *this;
}

/* -------------------------------------------------------------------------- */

void Obstructions::block(bool soft) const
{
	int numObstructions = soft ? numSoftObstructions : numHardObstructions;

	for (int k = 0; k < numObstructions; k++)
		*(obstructions[k]) += 1;
}

/* -------------------------------------------------------------------------- */

void Obstructions::unblock(bool soft) const
{
	int numObstructions = soft ? numSoftObstructions : numHardObstructions;

	for (int k = 0; k < numObstructions; k++)
		*(obstructions[k]) -= 1;
}

/* -------------------------------------------------------------------------- */

void Obstructions::optimize()
{
	int k, n;

	/* -- Remove from the obstruction table all useless entries -- */

	for (k = 0, n = 0; k < numSoftObstructions; n += *obstructions[k++] ? 0 : 1)
		obstructions[n] = obstructions[k];

	numSoftObstructions = n;

	for ( ; k < numHardObstructions; n += *obstructions[k++] ? 0 : 1)
		obstructions[n] = obstructions[k];
}

/* -------------------------------------------------------------------------- */

int Obstructions::numObstructions(bool soft) const
{
	return soft ? numSoftObstructions : numHardObstructions;
}

/* -------------------------------------------------------------------------- */

}