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

	/* -- Display number of free moves -- */

	int whiteMoves = problem->moves(White);
	int blackMoves = problem->moves(Black);

	int requiredWhiteMoves = whitePieces->getRequiredMoves();
	int requiredBlackMoves = blackPieces->getRequiredMoves();

	int freeWhiteMoves = whiteMoves - requiredWhiteMoves;
	int freeBlackMoves = blackMoves - requiredBlackMoves;

	if (callbacks.displayFreeMoves)
		(*callbacks.displayFreeMoves)(callbacks.handle, freeWhiteMoves, freeBlackMoves);

	/* -- Display actual deductions -- */
	
	EUCLIDE_Deductions deductions = *this;

	if (callbacks.displayDeductions)
		(*callbacks.displayDeductions)(callbacks.handle, &deductions);

	/* -- Find squares where required captures have occured -- */

	whitePieces->getCaptureSquares(*board, *blackPieces);
	blackPieces->getCaptureSquares(*board, *whitePieces);

	/* -- Recompute move requirements -- */

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

	/* -- Display actual deductions -- */
	
	deductions = *this;

	if (callbacks.displayFreeMoves)
		(*callbacks.displayFreeMoves)(callbacks.handle, problem->moves(White) - whitePieces->getRequiredMoves(), problem->moves(Black) - blackPieces->getRequiredMoves());

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
			const FinalSquares& squares = (color == White) ? (*whitePieces)[man] : (*blackPieces)[man];

			deduction->initialGlyph = tables::supermanToGlyph[man][color].glyph_c();
			deduction->promotionGlyph = deduction->initialGlyph;

			deduction->initialSquare = tables::initialSquares[man][color];
			deduction->finalSquare = -1;

			deduction->requiredMoves = squares.getRequiredMoves();
			deduction->numSquares = (int)((const finalsquares_t&)squares).size();

			deduction->captured = 0;

			if (((const finalsquares_t&)squares).size() == 1)
			{
				const FinalSquare& square = ((const finalsquares_t&)squares)[0];
				Superman superman = /*(Superman)*/square;

				deduction->promotionGlyph = tables::supermanToGlyph[superman][color].glyph_c();
				deduction->finalSquare = (Square)square;
				deduction->captured = square.isEmpty();
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
