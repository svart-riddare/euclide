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

	/* -- Display number of free moves -- */

	int whiteMoves = problem->moves(White);
	int blackMoves = problem->moves(Black);

	int requiredWhiteMoves = whitePieces->getRequiredMoves();
	int requiredBlackMoves = blackPieces->getRequiredMoves();

	int freeWhiteMoves = whiteMoves - requiredWhiteMoves;
	int freeBlackMoves = blackMoves - requiredBlackMoves;

	if (callbacks.displayFreeMoves)
		(*callbacks.displayFreeMoves)(callbacks.handle, freeWhiteMoves, freeBlackMoves);
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
