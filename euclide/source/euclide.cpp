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

	/* -- Compute required moves and captures and
	      apply non ubiquity principle as often as needed -- */

	while (whitePieces->applyNonUbiquityPrinciple())
	{
		whitePieces->computeRequiredMoves(*board);
		whitePieces->applyMoveConstraints(problem->moves(White));
		
		whitePieces->computeRequiredCaptures(*board);
		whitePieces->applyCaptureConstraints(problem->captures(White));
	}

	while (blackPieces->applyNonUbiquityPrinciple())
	{
		blackPieces->computeRequiredMoves(*board);
		blackPieces->applyMoveConstraints(problem->moves(Black));
		
		blackPieces->computeRequiredCaptures(*board);
		blackPieces->applyCaptureConstraints(problem->captures(Black));
	}

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
		whitePieces->computeRequiredMoves(*board);
		whitePieces->applyMoveConstraints(problem->moves(White));
		
		whitePieces->computeRequiredCaptures(*board);
		whitePieces->applyCaptureConstraints(problem->captures(White));
	}
	while (whitePieces->applyNonUbiquityPrinciple());

	do
	{
		blackPieces->computeRequiredMoves(*board);
		blackPieces->applyMoveConstraints(problem->moves(Black));
		
		blackPieces->computeRequiredCaptures(*board);
		blackPieces->applyCaptureConstraints(problem->captures(Black));
	}
	while (blackPieces->applyNonUbiquityPrinciple());

	deductions = *this;

	if (callbacks.displayFreeMoves)
		(*callbacks.displayFreeMoves)(callbacks.handle, deductions.freeWhiteMoves, deductions.freeBlackMoves);

	if (callbacks.displayDeductions)
		(*callbacks.displayDeductions)(callbacks.handle, &deductions);
}

/* -------------------------------------------------------------------------- */

Euclide::operator EUCLIDE_Deductions() const
{
	EUCLIDE_Deductions deductions;
	
	for (Color color = FirstColor; color <= LastColor; color++)
	{
		for (Man man = FirstMan; man <= LastMan; man++)
		{
			EUCLIDE_Deduction *deduction = (color == White) ? &deductions.whitePieces[man] : &deductions.blackPieces[man];
			const Pieces *pieces = (color == White) ? whitePieces : blackPieces;

			deduction->initialGlyph = tables::supermanToGlyph[man][color].glyph_c();
			deduction->promotionGlyph = deduction->initialGlyph;

			deduction->initialSquare = tables::initialSquares[man][color];
			deduction->finalSquare = -1;

			deduction->requiredMoves = pieces->getRequiredMoves(man);
			deduction->numSquares = pieces->getNumDestinations(man);

			deduction->captured = false;

			if (deduction->numSquares == 1)
			{
				const Destination& destination = pieces->getDestination(man);

				deduction->promotionGlyph = tables::supermanToGlyph[destination.superman()][destination.color()].glyph_c();
				deduction->finalSquare = destination.square();
				deduction->captured = destination.captured();
			}
		}
	}

	deductions.freeWhiteMoves = problem->moves(White) - whitePieces->getRequiredMoves();
	deductions.freeBlackMoves = problem->moves(Black) - blackPieces->getRequiredMoves();

	return deductions;
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
