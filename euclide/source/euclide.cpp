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

		operator EUCLIDE_Deductions() const;

	private :
		EUCLIDE_Configuration configuration;
		EUCLIDE_Callbacks callbacks;

		Board *board;
		Problem *problem;

		Pieces *whitePieces;
		Pieces *blackPieces;
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

	whitePieces = NULL;
	blackPieces = NULL;

	/* -- Display copyright string -- */

	if (callbacks.displayCopyright)
		(*callbacks.displayCopyright)(callbacks.handle, constants::copyright);
}

/* -------------------------------------------------------------------------- */

Euclide::~Euclide()
{
	delete whitePieces;
	delete blackPieces;

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

	/* -- Create board structure -- */

	board = new Board();

	/* -- Create position structures -- */

	whitePieces = new Pieces(*problem, White);
	blackPieces = new Pieces(*problem, Black);

	/* -- Make initial non-ubiquity deduction (for kings) -- */

	whitePieces->analysePartitions();
	blackPieces->analysePartitions();

	/* -- Compute initial number of required moves and captures -- */

	whitePieces->computeRequiredMoves(*board);
	whitePieces->computeRequiredCaptures(*board);

	blackPieces->computeRequiredMoves(*board);
	blackPieces->computeRequiredCaptures(*board);

	/* -- Make recursive deductions based on these results -- */

	do
	{
		whitePieces->analyseMoveConstraints(problem->moves(White));
		whitePieces->analyseCaptureConstraints(problem->captures(White));
	}
	while (whitePieces->analysePartitions());

	do
	{
		blackPieces->analyseMoveConstraints(problem->moves(Black));
		blackPieces->analyseCaptureConstraints(problem->captures(Black));
	}
	while (blackPieces->analysePartitions());

	/* -- Display actual deductions -- */
	
	EUCLIDE_Deductions deductions = *this;

	if (callbacks.displayFreeMoves)
		(*callbacks.displayFreeMoves)(callbacks.handle, deductions.freeWhiteMoves, deductions.freeBlackMoves);

	if (callbacks.displayDeductions)
		(*callbacks.displayDeductions)(callbacks.handle, &deductions);

	/* -- Analyse captures -- */

	whitePieces->analyseCaptures(*board, *blackPieces);
	blackPieces->analyseCaptures(*board, *whitePieces);

	do
	{
		whitePieces->analyseMoveConstraints(problem->moves(White));
		whitePieces->analyseCaptureConstraints(problem->captures(White));
	}
	while (whitePieces->analysePartitions());

	do
	{
		blackPieces->analyseMoveConstraints(problem->moves(Black));
		blackPieces->analyseCaptureConstraints(problem->captures(Black));
	}
	while (blackPieces->analysePartitions());

	deductions = *this;

	if (callbacks.displayFreeMoves)
		(*callbacks.displayFreeMoves)(callbacks.handle, deductions.freeWhiteMoves, deductions.freeBlackMoves);

	if (callbacks.displayDeductions)
		(*callbacks.displayDeductions)(callbacks.handle, &deductions);

	/* -- Block pieces on their initial squares -- */

	whitePieces->analyseStaticPieces(*board);
	blackPieces->analyseStaticPieces(*board);

	whitePieces->analyseCastling(*board);
	blackPieces->analyseCastling(*board);

	whitePieces->computeRequiredMoves(*board);
	whitePieces->computeRequiredCaptures(*board);

	blackPieces->computeRequiredMoves(*board);
	blackPieces->computeRequiredCaptures(*board);
	
	do
	{
		whitePieces->analyseMoveConstraints(problem->moves(White));
		whitePieces->analyseCaptureConstraints(problem->captures(White));
	}
	while (whitePieces->analysePartitions());

	do
	{
		blackPieces->analyseMoveConstraints(problem->moves(Black));
		blackPieces->analyseCaptureConstraints(problem->captures(Black));
	}
	while (blackPieces->analysePartitions());

	deductions = *this;

	if (callbacks.displayFreeMoves)
		(*callbacks.displayFreeMoves)(callbacks.handle, deductions.freeWhiteMoves, deductions.freeBlackMoves);

	if (callbacks.displayDeductions)
		(*callbacks.displayDeductions)(callbacks.handle, &deductions);
}

/* -------------------------------------------------------------------------- */

Euclide::operator EUCLIDE_Deductions() const
{
	EUCLIDE_Deductions _deductions;
	
	for (Color color = FirstColor; color <= LastColor; color++)
	{
		const Pieces *pieces = (color == White) ? whitePieces : blackPieces;
		EUCLIDE_Deduction *deductions = (color == White) ? _deductions.whitePieces : _deductions.blackPieces;

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

			deduction->captured = false;
		}

		/* -- Scan all destinations to fill deductions -- */

		for (Partitions::const_iterator partition = pieces->begin(); partition != pieces->end(); partition++)
		{
			bool generic = false;

			for (Partition::const_iterator target = partition->begin(); target != partition->end(); target++)
			{
				if (target->isGeneric())
				{
					if (generic)
						continue;

					generic = true;
				}

				for (Destinations::const_iterator destination = target->begin(); destination != target->end(); destination++)
				{
					EUCLIDE_Deduction *deduction = &deductions[destination->man()];

					minimize(deduction->requiredMoves, destination->getRequiredMoves());
					
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
	}

	_deductions.freeWhiteMoves = problem->moves(White) - whitePieces->getRequiredMoves();
	_deductions.freeBlackMoves = problem->moves(Black) - blackPieces->getRequiredMoves();

	return _deductions;
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
