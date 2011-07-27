#include "hashtables.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

MiniHashEntry::MiniHashEntry()
{
	_nvisits = 0;
}

/* -------------------------------------------------------------------------- */

bool MiniHashEntry::visited(const int moves[2], int *requiredMoves) const
{
	assert(moves[0] <= 127);
	assert(moves[1] <= 127);

	/* -- Can we deduce the nmber of required moves from a previous visit ? -- */

	for (int k = 0; k < _nvisits; k++)
	{
		if ((moves[0] >= _visits[k].first[0]) && (moves[1] >= _visits[k].first[1]))
		{
			minimize(*requiredMoves, (int)_visits[k].second);
			return true;
		}
	}

	/* -- Seems not ! -- */

	return false;
}

/* -------------------------------------------------------------------------- */

void MiniHashEntry::visited(const int moves[2], int requiredMoves)
{
	/* -- Update an already existing entry -- */

	for (int k = 0; k < _nvisits; k++)
	{
		if ((moves[0] <= _visits[k].first[0]) && (moves[1] <= _visits[k].first[0]))
		{
			_visits[k].first[0] = (int8_t)moves[0];
			_visits[k].first[1] = (int8_t)moves[1];
			minimize(_visits[k].second, (int16_t)requiredMoves);
			return;
		}
	}

	/* -- Get a slot to save information -- */

	int m = _nvisits;
	
	if (m >= (int)_visits.size())
	{
		m = 0;
		for (int k = 1; k < _nvisits; k++)
			if ((_visits[k].first[0] + _visits[k].first[1]) > (_visits[m].first[0] + _visits[m].first[1]))
				m = k;
	}
	else
	{
		_nvisits += 1;
	}

	/* -- Save information -- */

	_visits[m].first[0] = (int8_t)moves[0];
	_visits[m].first[1] = (int8_t)moves[1];
	_visits[m].second = (int16_t)requiredMoves;
}

/* -------------------------------------------------------------------------- */

}