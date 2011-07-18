#include "implications.h"
#include "partitions.h"
#include "position.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Implication::Implication()
{
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

Implication::~Implication()
{
}

/* -------------------------------------------------------------------------- */

void Implication::add(int requiredMoves, int unassignedMoves, int requiredCaptures, int unassignedCaptures, Square square, Superman superman, bool captured)
{
	maximize(_availableMoves, requiredMoves + unassignedMoves);
	maximize(_availableCaptures, requiredCaptures + unassignedCaptures);

	minimize(_unassignedMoves, unassignedMoves);
	minimize(_unassignedCaptures, unassignedCaptures);

	_squares[square] = true;
	_supermen[superman] = true;

	(captured ? _captured : _alive) = true;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

Implications::Implications(const Position& position)
{
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

Implications::~Implications()
{
}

/* -------------------------------------------------------------------------- */

}