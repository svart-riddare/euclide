#include "position.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Position::Position(const Problem& problem, Color color)
	: _color(color)
{
	/* -- Initialize move and capture requirements -- */

	_requiredMoves = 0;
	_requiredCaptures = 0;

	/* -- Create single partition -- */

	reserve(NumMen);
	push_back(new Partition(problem, color));
}

/* -------------------------------------------------------------------------- */

Position& Position::operator+=(const Position& position)
{
	assert(position.color() != _color);

	for (const_iterator partition = begin(); partition != end(); partition++)
		for (Partition::const_iterator target = partition->begin(); target != partition->end(); target++)
			if (!target->alive())
				captures.push_back(Capture(*target));

	return *this;
}

/* -------------------------------------------------------------------------- */

Position::~Position()
{
	for (iterator partition = begin(); partition != end(); partition++)
		delete *partition;
}

/* -------------------------------------------------------------------------- */

bool Position::analysePartitions()
{
	bool modified = false;
	int partitions = (int)size();

	/* -- Refine (split) partitions is possible -- */

	for (int k = 0; k < partitions; k++)
		while (at(k)->refine(*this, NumMen))
			modified = true;
	
	return modified;
}

/* -------------------------------------------------------------------------- */

bool Position::analyseMoveConstraints(int availableMoves, bool quick)
{
	int freeMoves = availableMoves - requiredMoves();
	if (freeMoves < 0)
		abort(NoSolution);
	
	bool modified = false;
	for (iterator partition = begin(); partition != end(); partition++)
		if (partition->setAvailableMoves(partition->requiredMoves() + freeMoves))
			modified = true;

	if (!quick && !modified)
		for (iterator partition = begin(); partition != end(); partition++)
			if (partition->analyseAvailableMoves(partition->requiredMoves() + freeMoves))
				modified = true;

	if (!modified)
		return false;

	return true;
}

/* -------------------------------------------------------------------------- */

bool Position::analyseCaptureConstraints(int availableCaptures, bool quick)
{
	int freeCaptures = availableCaptures - requiredCaptures();
	if (freeCaptures < 0)
		abort(NoSolution);

	bool modified = false;
	for (iterator partition = begin(); partition != end(); partition++)
		if (partition->setAvailableCaptures(partition->requiredCaptures() + freeCaptures))
			modified = true;

	if (!quick && !modified)
		for (iterator partition = begin(); partition != end(); partition++)
			if (partition->analyseAvailableCaptures(partition->requiredCaptures() + freeCaptures))
				modified = true;

	if (!modified)
		return false;

	return true;
}

/* -------------------------------------------------------------------------- */

bool Position::analyseCaptures(const Board& board, Position& position)
{
	bool modified = false;

	/* -- List required captures -- */

	for (const_iterator partition = begin(); partition != end(); partition++)
	{
		for (Partition::const_iterator target = partition->begin(); target != partition->end(); target++)
		{
			if (!target->requiredCaptures())
				continue;

			vector<Squares> captures(NumMen);

			for (Target::const_iterator destination = target->begin(); destination != target->end(); destination++)
			{
				assert(destination->requiredCaptures() >= target->requiredCaptures());

				/* -- Find squares for required captures -- */

				board.getCaptures(destination->man(), destination->superman(), destination->color(), destination->man().square(destination->color()), destination->square(), captures);
			}

			captures.resize(target->requiredCaptures());

			/* -- Reserve captures -- */

			for (int k = 0; k < (int)captures.size(); k++)
			{
				Capture *capture = &*std::find_if(position.captures.begin(), position.captures.end(), !boost::bind(&Capture::assigned, _1));
				if (!capture)
					abort(NoSolution);

				capture->setPossibleSquares(captures[k]);
				capture->assign(*target, true);
				modified = true;
			}
		}
	}

	if (!modified)
		return false;

	updateRequiredMoves(true);
	updateRequiredCaptures(true);

	return true;
}

/* -------------------------------------------------------------------------- */

void Position::computeRequiredMoves(const Board& board)
{
	int requiredMoves = 0;
	for (iterator partition = begin(); partition != end(); partition++)
		requiredMoves += partition->computeRequiredMoves(board);

	maximize(_requiredMoves, requiredMoves);
}

/* -------------------------------------------------------------------------- */

void Position::computeRequiredCaptures(const Board& board)
{
	int requiredCaptures = 0;
	for (iterator partition = begin(); partition != end(); partition++)
		requiredCaptures += partition->computeRequiredCaptures(board);

	maximize(_requiredCaptures, requiredCaptures);
}

/* -------------------------------------------------------------------------- */

int Position::updateRequiredMoves(bool recursive)
{
	int requiredMoves = 0;
	for (iterator partition = begin(); partition != end(); partition++)
		requiredMoves += recursive ? partition->updateRequiredMoves(true) : partition->requiredMoves();

	return maximize(_requiredMoves, requiredMoves);
}

/* -------------------------------------------------------------------------- */

int Position::updateRequiredCaptures(bool recursive)
{
	int requiredCaptures = 0;
	for (iterator partition = begin(); partition != end(); partition++)
		requiredCaptures += recursive ? partition->updateRequiredCaptures(true) : partition->requiredCaptures();

	return maximize(_requiredCaptures, requiredCaptures);
}

/* -------------------------------------------------------------------------- */

}
