#include "implications.h"
#include "assignations.h"
#include "partitions.h"
#include "position.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Implication::Implication()
{
	clear();
}

/* -------------------------------------------------------------------------- */

Implication::~Implication()
{
}

/* -------------------------------------------------------------------------- */

void Implication::clear()
{
	_assignedMoves = 0;
	_assignedCaptures = 0;

	_availableMoves = 0;
	_availableCaptures = 0;

	_unassignedMoves = infinity;
	_unassignedCaptures = infinity;

	_squares.reset();
	_supermen.reset();

	_captured = false;
	_alive = false;
}

/* -------------------------------------------------------------------------- */

void Implication::add(int requiredMoves, int unassignedMoves, int requiredCaptures, int unassignedCaptures, Square square, Superman superman, bool captured)
{
	maximize(_availableMoves, requiredMoves + unassignedMoves);
	maximize(_availableCaptures, requiredCaptures + unassignedCaptures);

	minimize(_unassignedMoves, unassignedMoves);
	minimize(_unassignedCaptures, unassignedCaptures);

	_assignedMoves = _availableMoves - _unassignedMoves;
	_assignedCaptures = _availableCaptures - _unassignedCaptures;

	_squares[square] = true;
	_supermen[superman] = true;

	(captured ? _captured : _alive) = true;

	assert(_assignedMoves + _unassignedMoves == _availableMoves);
	assert(_assignedCaptures + _unassignedCaptures == _availableCaptures);
}

/* -------------------------------------------------------------------------- */

void Implication::set(int requiredMoves, int availableMoves, int requiredCaptures, int availableCaptures)
{
	maximize(_assignedMoves, requiredMoves);
	minimize(_availableMoves, availableMoves);

	_unassignedMoves = _availableMoves - _assignedMoves;

	maximize(_assignedCaptures, requiredCaptures);
	minimize(_availableCaptures, availableCaptures);

	_unassignedCaptures = _availableCaptures - _assignedCaptures;
}

/* -------------------------------------------------------------------------- */

void Implication::sub(int moves, int captures)
{
	if (moves > 0)
	{
		_unassignedMoves = std::max(0, _unassignedMoves - moves);
		_availableMoves = _assignedMoves + _unassignedMoves;
	}

	if (captures > 0)
	{
		_unassignedCaptures = std::max(0, _unassignedCaptures - captures);
		_availableCaptures = _assignedCaptures + _unassignedCaptures;
	}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

Implications::Implications()
{
	_assignedMoves = NULL;
	_assignedCaptures = NULL;
}

/* -------------------------------------------------------------------------- */

Implications::Implications(const Position& position)
{
	update(position, false);

	_assignedMoves = NULL;
	_assignedCaptures = NULL;
}

/* -------------------------------------------------------------------------- */

Implications::Implications(const Position& position, const Assignations& assignedMoves, const Assignations& assignedCaptures)
{
	update(position, false);
	update(assignedMoves, assignedCaptures);

	_assignedMoves = &assignedMoves;
	_assignedCaptures = &assignedCaptures;
}

/* -------------------------------------------------------------------------- */

Implications::~Implications()
{
}

/* -------------------------------------------------------------------------- */

void Implications::update(const Position& position, bool clear)
{
	/* -- Reset -- */

	if (clear)
		for (Man man = FirstMan; man <= LastMan; man++)
			_implications[man].clear();

	/* -- Loop through partitions, targets and destinations to collect the information -- */

	for	(Partitions::const_iterator partition = position.begin(); partition != position.end(); partition++)
	{
		int unassignedMoves = position.availableMoves() - position.requiredMoves() + partition->requiredMoves() - partition->assignedMoves();
		int unassignedCaptures = position.availableCaptures() - position.requiredCaptures() + partition->requiredCaptures() - partition->assignedCaptures();

		for (Targets::const_iterator target = partition->begin(); target != partition->end(); target++)
			for (Destinations::const_iterator destination = target->begin(); destination != target->end(); destination++)
				_implications[destination->man()].add(target->requiredMoves(), unassignedMoves, target->requiredCaptures(), unassignedCaptures, destination->square(), destination->superman(), destination->captured());
	}
}

/* -------------------------------------------------------------------------- */

void Implications::update(const Assignations& assignedMoves, const Assignations& assignedCaptures)
{
	/* -- Process assignations, for moves and then captures -- */

	int minExtraMoves = 0;
	int minExtraCaptures = 0;

	array<int, NumMen> extraMoves(0);
	array<int, NumMen> extraCaptures(0);

	for (Assignations::const_iterator assigned = assignedMoves.begin(); assigned != assignedMoves.end(); assigned++)
	{
		int moves = assigned->assigned();
		Men men = assigned->men();
		
		for (Man man = FirstMan; man <= LastMan; man++)
			if (men[man])
				moves -= this->assignedMoves(man);

		if (moves <= 0)
			continue;
		
		int minMoves = assigned->assigned();
		for (Man man = FirstMan; man <= LastMan; man++)
		{
			if (men[man])
			{
				minimize(minMoves, moves - extraMoves[man]);
				maximize(extraMoves[man], moves);
			}
		}

		if (minMoves > 0)
			minExtraMoves += minMoves;
	}

	for (Assignations::const_iterator assigned = assignedCaptures.begin(); assigned != assignedCaptures.end(); assigned++)
	{
		int captures = assigned->assigned();
		Men men = assigned->men();
		
		for (Man man = FirstMan; man <= LastMan; man++)
			if (men[man])
				captures -= this->assignedCaptures(man);

		if (captures <= 0)
			continue;
		
		int minCaptures = assigned->assigned();
		for (Man man = FirstMan; man <= LastMan; man++)
		{
			if (men[man])
			{
				minimize(minCaptures, captures - extraCaptures[man]);
				maximize(extraCaptures[man], captures);
			}
		}

		if (minCaptures > 0)
			minExtraCaptures += minCaptures;
	}

	/* -- Update implications -- */

	for (Man man = FirstMan; man <= LastMan; man++)
		_implications[man].sub(minExtraMoves - extraMoves[man], minExtraCaptures - extraCaptures[man]);
}

/* -------------------------------------------------------------------------- */

int Implications::assignedMoves(Man man, Man xman) const
{
	if (_assignedMoves)
	{
		Men men = ((1 << man) | (1 << xman));

		Assignations::const_iterator assignation = std::find_if(_assignedMoves->begin(), _assignedMoves->end(), boost::bind(&Assignation::isMen, _1, cref(men)));
		if (assignation != _assignedMoves->end())
			return assignation->assigned();
	}

	return assignedMoves(man) + assignedMoves(xman);
}

/* -------------------------------------------------------------------------- */

int Implications::assignedCaptures(Man man, Man xman) const
{
	if (_assignedCaptures)
	{
		Men men = ((1 << man) | (1 << xman));

		Assignations::const_iterator assignation = std::find_if(_assignedCaptures->begin(), _assignedCaptures->end(), boost::bind(&Assignation::isMen, _1, cref(men)));
		if (assignation != _assignedCaptures->end())
			return assignation->assigned();
	}		

	return assignedCaptures(man) + assignedCaptures(xman);
}

/* -------------------------------------------------------------------------- */

int Implications::availableMoves(Man man, Man xman) const
{
	return availableMoves(man) + availableMoves(xman) - std::min(unassignedMoves(man), unassignedMoves(xman));
}

/* -------------------------------------------------------------------------- */

int Implications::availableCaptures(Man man, Man xman) const
{
	return availableCaptures(man) + availableCaptures(xman) - std::min(unassignedCaptures(man), unassignedCaptures(xman));
}

/* -------------------------------------------------------------------------- */

int Implications::unassignedMoves(Man man, Man xman) const
{
	return std::max(unassignedMoves(man), unassignedMoves(xman));
}

/* -------------------------------------------------------------------------- */

int Implications::unassignedCaptures(Man man, Man xman) const
{
	return std::max(unassignedMoves(man), unassignedMoves(xman));
}

/* -------------------------------------------------------------------------- */

int Implications::assignedMoves(Men men) const
{
	int assignedMoves = 0;
	
	for (Man man = FirstMan; man <= LastMan; man++)
		if (men[man])
			assignedMoves += _implications[man].assignedMoves();

	return assignedMoves;
}

/* -------------------------------------------------------------------------- */

int Implications::assignedCaptures(Men men) const
{
	int assignedCaptures = 0;

	for (Man man = FirstMan; man <= LastMan; man++)
		if (men[man])
			assignedCaptures += _implications[man].assignedCaptures();

	return assignedCaptures;
}

/* -------------------------------------------------------------------------- */

int Implications::availableMoves(Men men) const
{
	int availableMoves = 0;
	int unassignedMoves = infinity;

	for (Man man = FirstMan; man <= LastMan; man++)
	{
		if (men[man])
		{
			availableMoves += _implications[man].availableMoves();
			minimize(unassignedMoves, _implications[man].unassignedMoves());
		}
	}

	assert(men.any());
	return availableMoves - unassignedMoves;
}

/* -------------------------------------------------------------------------- */

int Implications::availableCaptures(Men men) const
{
	int availableCaptures = 0;
	int unassignedCaptures = infinity;

	for (Man man = FirstMan; man <= LastMan; man++)
	{
		if (men[man])
		{
			availableCaptures += _implications[man].availableCaptures();
			minimize(unassignedCaptures, _implications[man].unassignedCaptures());
		}
	}

	assert(men.any());
	return availableCaptures - unassignedCaptures;
}

/* -------------------------------------------------------------------------- */

int Implications::unassignedMoves(Men men) const
{
	int unassignedMoves = 0;

	for (Man man = FirstMan; man <= LastMan; man++)
		if (men[man])
			maximize(unassignedMoves, _implications[man].unassignedMoves());

	assert(men.any());
	return unassignedMoves;
}

/* -------------------------------------------------------------------------- */

int Implications::unassignedCaptures(Men men) const
{
	int unassignedCaptures = 0;

	for (Man man = FirstMan; man <= LastMan; man++)
		if (men[man])
			maximize(unassignedCaptures, _implications[man].unassignedCaptures());

	assert(men.any());
	return unassignedCaptures;
}

/* -------------------------------------------------------------------------- */

Squares Implications::squares(Men men) const
{
	Squares squares;

	for (Man man = FirstMan; man <= LastMan; man++)
		if (men[man])
			squares |= _implications[man].squares();

	return squares;
}

/* -------------------------------------------------------------------------- */

}