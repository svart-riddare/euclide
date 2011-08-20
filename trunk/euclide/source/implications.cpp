#include "implications.h"
#include "assignations.h"
#include "partitions.h"
#include "position.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Implication::Implication()
{
	_assignedMoves = infinity;
	_assignedCaptures = infinity;

	_requiredMoves = infinity;
	_requiredCaptures = infinity;

	_availableMoves = infinity;
	_availableCaptures = infinity;

	_squares.reset();
	_supermen.reset();

	_captured = false;
	_alive = false;
}

/* -------------------------------------------------------------------------- */

void Implication::add(int assignedMoves, int assignedCaptures, Square square, Superman superman, bool captured)
{
	minimize(_assignedMoves, assignedMoves);
	minimize(_assignedCaptures, assignedCaptures);

	minimize(_requiredMoves, assignedMoves);
	minimize(_requiredCaptures, assignedCaptures);

	_squares[square] = true;
	_supermen[superman] = true;

	(captured ? _captured : _alive) = true;
}

/* -------------------------------------------------------------------------- */

void Implication::max(int requiredMoves, int requiredCaptures)
{
	maximize(_requiredMoves, requiredMoves);
	maximize(_requiredCaptures, requiredCaptures);
}

/* -------------------------------------------------------------------------- */

void Implication::set(int extraMoves, int extraCaptures)
{
	minimize(_availableMoves, _requiredMoves + extraMoves);
	minimize(_availableCaptures, _requiredCaptures + extraCaptures);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

Implications::Implications(const Position& position)
{
	_availableMoves = position.availableMoves();
	_availableCaptures = position.availableCaptures();

	constructor(position);
}

/* -------------------------------------------------------------------------- */

Implications::Implications(const Position& position, const Assignations& assignedMoves, const Assignations& assignedCaptures)
{
	_availableMoves = position.availableMoves();
	_availableCaptures = position.availableCaptures();

	constructor(position, &assignedMoves, &assignedCaptures);
}

/* -------------------------------------------------------------------------- */

Implications::~Implications()
{
}

/* -------------------------------------------------------------------------- */

void Implications::constructor(const Position& position, const Assignations *assignedMoves, const Assignations *assignedCaptures)
{
	/* -- Loop through partitions, targets and destinations to collect basic information -- */

	for	(Partitions::const_iterator partition = position.begin(); partition != position.end(); partition++)
		for (Targets::const_iterator target = partition->begin(); target != partition->end(); target++)
			for (Destinations::const_iterator destination = target->begin(); destination != target->end(); destination++)
				_implications[destination->man()].add(destination->requiredMoves(), destination->requiredCaptures(), destination->square(), destination->superman(), destination->captured());
	
	/* -- Process single-piece assignations -- */

	if (assignedMoves)		
		for (Assignations::const_iterator assignation = assignedMoves->begin(); assignation != assignedMoves->end(); assignation++)
			if (assignation->men().count() == 1)
				_implications[assignation->man()].max(assignation->assigned(), 0);

	if (assignedCaptures)
		for (Assignations::const_iterator assignation = assignedCaptures->begin(); assignation != assignedCaptures->end(); assignation++)
			if (assignation->men().count() == 1)
				_implications[assignation->man()].max(0, assignation->assigned());

	/* -- Consider partitions as assignations -- */

	for (Partitions::const_iterator partition = position.begin(); partition != position.end(); partition++)
	{
		int requiredMoves = 0;
		int requiredCaptures = 0;
		
		for (Man man = FirstMan; man <= LastMan; man++)
		{
			if (partition->men()[man])
			{
				requiredMoves += this->requiredMoves(man);
				requiredCaptures += this->requiredCaptures(man);
			}
		}

		_movePartitions.push_back(Assignation(partition->men(), partition->color(), std::max(requiredMoves, partition->requiredMoves())));
		_capturePartitions.push_back(Assignation(partition->men(), partition->color(), std::max(requiredCaptures, partition->requiredCaptures())));
	}

	/* -- Process assignations -- */

	if (assignedMoves)
	{
		for (Assignations::const_iterator assignation = assignedMoves->begin(); assignation != assignedMoves->end(); assignation++)
		{
			Men men = assignation->men();

			/* -- Merge partitions if necessary -- */

			vector<Assignation>::iterator partition = _movePartitions.begin();
			while ((partition->men().to_ulong() & men.to_ulong()) == 0)
				partition++;

			for (vector<Assignation>::size_type p = 0; p < _movePartitions.size(); p++)
			{
				if ((_movePartitions[p].men().to_ulong() & men.to_ulong()) != 0)
				{
					if (_movePartitions[p].men().to_ulong() != partition->men().to_ulong())
					{
						partition->merge(_movePartitions[p]);
						_movePartitions[p--] = _movePartitions.back();
						_movePartitions.pop_back();
					}
				}
			}

			/* -- Add additionnal information to partition -- */

			partition->merge(*assignation);

			/* -- Save assignation if it concerns exactly two men -- */

			if (men.count() == 2)
				_assignedMoves.push_back(*assignation);
		}
	}

	/* -- Same for captures -- */

	if (assignedCaptures)
	{
		for (Assignations::const_iterator assignation = assignedCaptures->begin(); assignation != assignedCaptures->end(); assignation++)
		{
			Men men = assignation->men();

			/* -- Merge partitions if necessary -- */

			vector<Assignation>::iterator partition = _movePartitions.begin();
			while ((partition->men().to_ulong() & men.to_ulong()) == 0)
				partition++;

			for (vector<Assignation>::size_type p = 0; p < _movePartitions.size(); p++)
			{
				if ((_movePartitions[p].men().to_ulong() & men.to_ulong()) != 0)
				{
					if (_movePartitions[p].men().to_ulong() != partition->men().to_ulong())
					{
						partition->merge(_movePartitions[p]);
						_movePartitions[p--] = _movePartitions.back();
						_movePartitions.pop_back();
					}
				}
			}

			/* -- Add additionnal information to partition -- */

			partition->merge(*assignation);

			/* -- Save assignation if it concerns exactly two men -- */

			if (men.count() == 2)
				_assignedCaptures.push_back(*assignation);
		}
	}

	/* -- Compute minimum required moves and captures for partitions -- */

	_requiredMoves = 0;
	for (vector<Assignation>::iterator partition = _movePartitions.begin(); partition != _movePartitions.end(); partition++)
	{
		Men men = partition->men();

		int minimumMoves = 0;
		for (Man man = FirstMan; man <= LastMan; man++)
			if (men[man])
				minimumMoves += this->requiredMoves(man);

		partition->minimum(minimumMoves);

		/* -- Save partition if it concerns exactly two men -- */

		if ((partition->minimum() < partition->assigned()) && (men.count() == 2))
			_assignedMoves.push_back(*partition);

		/* -- Sum up number of required moves -- */

		_requiredMoves += partition->assigned();
	}

	_requiredCaptures = 0;
	for (vector<Assignation>::iterator partition = _capturePartitions.begin(); partition != _capturePartitions.end(); partition++)
	{
		Men men = partition->men();

		int minimumCaptures = 0;
		for (Man man = FirstMan; man <= LastMan; man++)
			if (men[man])
				minimumCaptures += this->requiredCaptures(man);

		partition->minimum(minimumCaptures);

		/* -- Save partition if it concerns exactly two men -- */

		if ((partition->minimum() < partition->assigned()) && (men.count() == 2))
			_assignedCaptures.push_back(*partition);

		/* -- Sum up number of required captures -- */

		_requiredCaptures += partition->assigned();
	}

	/* -- Coherency check -- */

	if (_requiredMoves > _availableMoves)
		abort(NoSolution);

	if (_requiredCaptures > _availableCaptures)
		abort(NoSolution);

	/* -- Dispatch available moves and captures -- */

	for (Man man = FirstMan; man <= LastMan; man++)
	{
		vector<Assignation>::const_iterator moves = _movePartitions.begin();
		while (!moves->men()[man])
			moves++;

		vector<Assignation>::const_iterator captures = _capturePartitions.begin();
		while (!captures->men()[man])
			captures++;

		_implications[man].set(moves->assigned() - moves->minimum() + _availableMoves - _requiredMoves, captures->assigned() - captures->minimum() + _availableCaptures - _requiredCaptures);
	}
}

/* -------------------------------------------------------------------------- */

int Implications::requiredMoves(Man manA, Man manB) const
{
	int requiredMoves = this->requiredMoves(manA) + this->requiredMoves(manB);

	Men men = (1 << manA) | (1 << manB);
	Assignations::const_iterator assignation = _assignedMoves.begin();
	while ((assignation = std::find_if<Assignations::const_iterator>(assignation, _assignedMoves.end(), boost::bind(&Assignation::isMen, _1, cref(men)))) != _assignedMoves.end())
	{
		maximize(requiredMoves, assignation->assigned());
		assignation++;
	}

	return requiredMoves;
}

/* -------------------------------------------------------------------------- */

int Implications::requiredCaptures(Man manA, Man manB) const
{
	int requiredCaptures = this->requiredCaptures(manA) + this->requiredCaptures(manB);

	Men men = (1 << manA) | (1 << manB);
	Assignations::const_iterator assignation = _assignedCaptures.begin();
	while ((assignation = std::find_if<Assignations::const_iterator>(assignation, _assignedCaptures.end(), boost::bind(&Assignation::isMen, _1, cref(men)))) != _assignedCaptures.end())
	{
		maximize(requiredCaptures, assignation->assigned());
		assignation++;
	}

	return requiredCaptures;
}

/* -------------------------------------------------------------------------- */

int Implications::availableMoves(Man manA, Man manB) const
{
	int moves = _availableMoves - _requiredMoves;

	vector<Assignation>::const_iterator movesA = _movePartitions.begin();
	vector<Assignation>::const_iterator movesB = _movePartitions.begin();

	while (!movesA->men()[manA])
		movesA++;
	while (!movesB->men()[manB])
		movesB++;

	if (movesA->men() == movesB->men())
	{
		/* -- Both are in the same partition -- */

		moves += movesA->assigned();

		for (Man man = FirstMan; man <= LastMan; man++)
			if (movesA->men()[man] && (man != manA) && (man != manB))
				moves -= this->requiredMoves(man);
	}
	else
	{
		/* -- Both are in two different partitions -- */

		moves += movesA->assigned() - movesA->minimum() + this->requiredMoves(manA);
		moves += movesB->assigned() - movesB->minimum() + this->requiredMoves(manB);
	}
	
	/* -- Done -- */

	return moves;
}

/* -------------------------------------------------------------------------- */

int Implications::availableCaptures(Man manA, Man manB) const
{
	int captures = _availableCaptures - _requiredCaptures;

	vector<Assignation>::const_iterator capturesA = _movePartitions.begin();
	vector<Assignation>::const_iterator capturesB = _movePartitions.begin();

	while (!capturesA->men()[manA])
		capturesA++;
	while (!capturesB->men()[manB])
		capturesB++;

	if (capturesA->men() == capturesB->men())
	{
		/* -- Both are in the same partition -- */

		captures += capturesA->assigned();

		for (Man man = FirstMan; man <= LastMan; man++)
			if (capturesA->men()[man] && (man != manA) && (man != manB))
				captures -= this->requiredCaptures(man);
	}
	else
	{
		/* -- Both are in two different partitions -- */

		captures += capturesA->assigned() - capturesA->minimum() + this->requiredCaptures(manA);
		captures += capturesB->assigned() - capturesB->minimum() + this->requiredCaptures(manB);
	}
	
	/* -- Done -- */

	return captures;
}

/* -------------------------------------------------------------------------- */

Squares Implications::squares(const Men& men) const
{
	Squares squares;

	for (Man man = FirstMan; man <= LastMan; man++)
		if (men[man])
			squares |= _implications[man].squares();

	return squares;
}

/* -------------------------------------------------------------------------- */

}