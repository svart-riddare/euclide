#include "includes.h"
#include "position.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

class Euclide
{
	public :
		Euclide(const EUCLIDE_Configuration *pConfiguration, const EUCLIDE_Callbacks *pCallbacks);
		~Euclide();

		void solve(const EUCLIDE_Problem *problem);
		void analyseBasicConstraints();

		operator EUCLIDE_Deductions() const;

	private :
		EUCLIDE_Configuration configuration;
		EUCLIDE_Callbacks callbacks;

		Board *board;
		Problem *problem;

		Position *whitePosition;
		Position *blackPosition;
};

/* -------------------------------------------------------------------------- */

Euclide::Euclide(const EUCLIDE_Configuration *pConfiguration, const EUCLIDE_Callbacks *pCallbacks)
{
	/* -- Initialize configuration and callback structures -- */

	memset(&configuration, 0, sizeof(configuration));
	memset(&callbacks, 0, sizeof(callbacks));

	if (pConfiguration)
		configuration = *pConfiguration;
	if (pCallbacks)
		callbacks = *pCallbacks;

	/* -- Initialize other members -- */

	board = NULL;
	problem = NULL;

	whitePosition = NULL;
	blackPosition = NULL;

	/* -- Display copyright string -- */

	if (callbacks.displayCopyright)
		(*callbacks.displayCopyright)(callbacks.handle, constants::copyright);
}

/* -------------------------------------------------------------------------- */

Euclide::~Euclide()
{
	delete whitePosition;
	delete blackPosition;

	delete problem;
	delete board;
}

/* -------------------------------------------------------------------------- */

void Euclide::solve(const EUCLIDE_Problem *inputProblem)
{
	/* -- Initialize problem structure -- */

	problem = new Problem(inputProblem);

	/* -- Display problem -- */

	if (callbacks.displayProblem)
		(*callbacks.displayProblem)(callbacks.handle, inputProblem);

	/* -- Create position structures -- */

	whitePosition = new Position(*problem, White);
	blackPosition = new Position(*problem, Black);

	*whitePosition += *blackPosition;
	*blackPosition += *whitePosition;

	/* -- Create board structure -- */

	board = new Board(*whitePosition, *blackPosition, *problem);

	/* -- Initial partition split (for kings) -- */

	whitePosition->analysePartitions();
	blackPosition->analysePartitions();

	/* -- Analyse movements -- */

	analyseBasicConstraints();

	/* -- Analyse captures -- */

	whitePosition->analyseCaptures(*board, *blackPosition);
	blackPosition->analyseCaptures(*board, *whitePosition);
	
	analyseBasicConstraints();
}

/* -------------------------------------------------------------------------- */

void Euclide::analyseBasicConstraints()
{
	/* -- Analyse movements until there is no more deductions to be made -- */

	bool modified = true;
	while (modified)
	{
		modified = false;

		/* -- Compute number of required moves and captures -- */

		whitePosition->computeRequiredMoves(*board);
		whitePosition->computeRequiredCaptures(*board);

		blackPosition->computeRequiredMoves(*board);
		blackPosition->computeRequiredCaptures(*board);

		/* -- Analyse constraints given moves and captures -- */

		do
		{
			whitePosition->analyseMoveConstraints(problem->moves(White));
			whitePosition->analyseCaptureConstraints(problem->captures(White));
		}
		while (whitePosition->analysePartitions());

		do
		{
			blackPosition->analyseMoveConstraints(problem->moves(Black));
			blackPosition->analyseCaptureConstraints(problem->captures(Black));
		}
		while (blackPosition->analysePartitions());

		/* -- Optimize board movements -- */

		if (board->optimize())
			modified = true;

		/* -- Output current deductions -- */
	
		EUCLIDE_Deductions deductions = *this;

		if (callbacks.displayProgress)
			(*callbacks.displayProgress)(callbacks.handle, deductions.freeWhiteMoves, deductions.freeBlackMoves, deductions.complexity);

		if (callbacks.displayDeductions)
			(*callbacks.displayDeductions)(callbacks.handle, &deductions);
	}
}

/* -------------------------------------------------------------------------- */

Euclide::operator EUCLIDE_Deductions() const
{
	EUCLIDE_Deductions globalDeductions;
	
	for (Color color = FirstColor; color <= LastColor; color++)
	{
		const Position *position = (color == White) ? whitePosition : blackPosition;
		EUCLIDE_Deduction *deductions = (color == White) ? globalDeductions.whitePieces : globalDeductions.blackPieces;

		Supermen supermen[NumMen];

		/* -- Initialize deductions for each man -- */
		
		for (Man man = FirstMan; man <= LastMan; man++)
		{
			EUCLIDE_Deduction *deduction = &deductions[man];
		
			deduction->initialGlyph = man.glyph(color).glyph_c();
			deduction->promotionGlyph = deduction->initialGlyph;

			deduction->initialSquare = man.square(color);
			deduction->finalSquare = -1;

			deduction->requiredMoves = INT_MAX;
			deduction->numSquares = 0;
			deduction->numMoves = board->moves(man, color);

			deduction->captured = false;
		}

		/* -- Scan all destinations to fill deductions -- */

		for (Partitions::const_iterator partition = position->begin(); partition != position->end(); partition++)
		{
			Target *previous = NULL;

			for (Partition::const_iterator target = partition->begin(); target != partition->end(); target++)
			{
				if (previous)
					if (*previous == **target)
						continue;

				previous = *target;

				for (Destinations::const_iterator destination = target->begin(); destination != target->end(); destination++)
				{
					EUCLIDE_Deduction *deduction = &deductions[destination->man()];

					minimize(deduction->requiredMoves, destination->requiredMoves());
					supermen[destination->man()][destination->superman()] = true;
					
					if (++deduction->numSquares == 1)
					{
						deduction->promotionGlyph = destination->superman().glyph(destination->color()).glyph_c();
						deduction->finalSquare = destination->square();
						deduction->captured = destination->captured();
					}
					else
					{
						deduction->promotionGlyph = deduction->initialGlyph;
						deduction->finalSquare = -1;
						deduction->captured = false;
					}
				}
			}
		}

		/* -- Compute sum of possible moves -- */

		for (Man man = FirstMan; man <= LastMan; man++)
			for (Superman superman = FirstPromotedMan; superman <= LastPromotedMan; superman++)
				if (supermen[man][superman])
						deductions[man].numMoves += board->moves(superman, color);
	}

	/* -- Get number of unassigned moves -- */

	globalDeductions.freeWhiteMoves = problem->moves(White) - whitePosition->requiredMoves();
	globalDeductions.freeBlackMoves = problem->moves(Black) - blackPosition->requiredMoves();

	/* -- Estimate complexity -- */

	double complexity = 0.0;
	for (Color color = FirstColor; color <= LastColor; color++)
	{
		EUCLIDE_Deduction *deductions = (color == White) ? globalDeductions.whitePieces : globalDeductions.blackPieces;

		for (Man man = FirstMan; man <= LastMan; man++)
		{
			EUCLIDE_Deduction *deduction = &deductions[man];

			complexity += std::log(0.0 + deduction->numSquares);
			if (deduction->numMoves > deduction->requiredMoves)
				complexity += std::log(0.0 + deduction->numMoves - deduction->requiredMoves);
		}
	}

	complexity += std::log(1.0 + globalDeductions.freeWhiteMoves);
	complexity += std::log(1.0 + globalDeductions.freeBlackMoves);

	globalDeductions.complexity = complexity / std::log(2.0);

	/* -- Done -- */

	return globalDeductions;
}

/* -------------------------------------------------------------------------- */

}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

extern "C"
EUCLIDE_Status EUCLIDE_solve(const EUCLIDE_Configuration *pConfiguration, const EUCLIDE_Problem *problem, const EUCLIDE_Callbacks *pCallbacks)
{
	EUCLIDE_Status status = EUCLIDE_STATUS_OK;
	euclide::Euclide *euclide = NULL;

	try
	{
		euclide = new euclide::Euclide(pConfiguration, pCallbacks);
		euclide->solve(problem);
	}
	catch (euclide::error_t error)
	{
		status = euclide::getStatus(error); 
	}
	catch (std::bad_alloc)
	{
		status = EUCLIDE_STATUS_OUT_OF_MEMORY_ERROR;
	}

	delete euclide;
	return status;
}

/* -------------------------------------------------------------------------- */
