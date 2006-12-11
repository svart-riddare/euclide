#include "position.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Pieces::Pieces(const Problem& problem, Color color)
	: _color(color)
{
	/* -- Initialize move and capture requirements -- */

	requiredMoves = 0;
	requiredCaptures = 0;

	/* -- Copy problem glyphs -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
		glyphs[square] = problem[square];

	/* -- Create single partition -- */

	reserve(NumMen);
	push_back(Partition(problem, color));
}

/* -------------------------------------------------------------------------- */

bool Pieces::analysePartitions()
{
	bool modified = false;
	int partitions = (int)size();

	/* -- Refine (split) partitions is possible -- */

	for (int k = 0; k < partitions; k++)
		while (at(k).refine(*this, NumMen))
			modified = true;
	
	return modified;
}

/* -------------------------------------------------------------------------- */

bool Pieces::analyseMoveConstraints(int availableMoves, bool quick)
{
	int freeMoves = availableMoves - getRequiredMoves();
	
	bool modified = false;
	for (iterator partition = begin(); partition != end(); partition++)
		if (partition->setAvailableMoves(partition->getRequiredMoves() + freeMoves))
			modified = true;

	if (!quick && !modified)
		for (iterator partition = begin(); partition != end(); partition++)
			if (partition->analyseAvailableMoves(partition->getRequiredMoves() + freeMoves))
				modified = true;

	if (!modified)
		return false;

	return true;
}

/* -------------------------------------------------------------------------- */

bool Pieces::analyseCaptureConstraints(int availableCaptures, bool quick)
{
	int freeCaptures = availableCaptures - getRequiredCaptures();

	bool modified = false;
	for (iterator partition = begin(); partition != end(); partition++)
		if (partition->setAvailableCaptures(partition->getRequiredCaptures() + freeCaptures))
			modified = true;

	if (!quick && !modified)
		for (iterator partition = begin(); partition != end(); partition++)
			if (partition->setAvailableCaptures(partition->getRequiredCaptures() + freeCaptures))
				modified = true;

	if (!modified)
		return false;

	return true;
}

/* -------------------------------------------------------------------------- */

bool Pieces::analyseCaptures(const Board& board, const Pieces& pieces)
{
	/* -- Scan opponent targets for required captures -- */

	for (const_iterator partition = pieces.begin(); partition != pieces.end(); partition++)
	{
		for (Partition::const_iterator target = partition->begin(); target != partition->end(); target++)
		{
			if (!target->getRequiredCaptures())
				continue;

			vector<Squares> captures(target->getRequiredCaptures());

			for (Target::const_iterator destination = target->begin(); destination != target->end(); destination++)
			{
				assert(destination->getRequiredCaptures() >= target->getRequiredCaptures());

				/* -- We seek the minimal number of captures -- */

				if (destination->getRequiredCaptures() > target->getRequiredCaptures())
					continue;

				/* -- Find squares for required captures -- */

				board.getCaptures(destination->man(), destination->superman(), destination->color(), destination->man().square(destination->color()), destination->square(), captures);
			}

			/* -- These captures will constitute a target for the captured piece -- */
		
			for (int k = 0; k < (int)captures.size(); k++)
			{
				Target::Cause cause(*target, k);

				/* -- Find target which is dedicated to this capture, if any -- */

				Target *dedicated = NULL;
				Target *candidate = NULL;

				for (const_iterator partition = begin(); partition != end(); partition++)
				{
					for (Partition::const_iterator target = partition->begin(); target != partition->end(); target++)
					{
						if (target->cause() == cause)
							dedicated = *target;

						if (target->isGeneric())
							candidate = *target;
					}
				}

				/* -- If there is no dedicated target, create one -- */

				if (!dedicated)
				{
					if (!candidate)
						abort(NoSolution);

					candidate->setCause(cause);
					dedicated = candidate;
				}

				/* -- Update list of possible squares for dedicated target -- */
					
				dedicated->setPossibleSquares(captures[k]);
			}
		}
	}

	updateRequiredMoves(true);
	updateRequiredCaptures(true);
	return true;
}

/* -------------------------------------------------------------------------- */

void Pieces::computeRequiredMoves(const Board& board)
{
	int requiredMoves = 0;
	for (iterator partition = begin(); partition != end(); partition++)
		requiredMoves += partition->computeRequiredMoves(board);

	maximize(this->requiredMoves, requiredMoves);
}

/* -------------------------------------------------------------------------- */

void Pieces::computeRequiredCaptures(const Board& board)
{
	int requiredCaptures = 0;
	for (iterator partition = begin(); partition != end(); partition++)
		requiredCaptures += partition->computeRequiredCaptures(board);

	maximize(this->requiredCaptures, requiredCaptures);
}

/* -------------------------------------------------------------------------- */

int Pieces::updateRequiredMoves(bool recursive)
{
	int requiredMoves = 0;
	for (iterator partition = begin(); partition != end(); partition++)
		requiredMoves += recursive ? partition->updateRequiredMoves(true) : partition->getRequiredMoves();

	return maximize(this->requiredMoves, requiredMoves);
}

/* -------------------------------------------------------------------------- */

int Pieces::updateRequiredCaptures(bool recursive)
{
	int requiredCaptures = 0;
	for (iterator partition = begin(); partition != end(); partition++)
		requiredCaptures += recursive ? partition->updateRequiredCaptures(true) : partition->getRequiredCaptures();

	return maximize(this->requiredCaptures, requiredCaptures);
}

/* -------------------------------------------------------------------------- */

}
