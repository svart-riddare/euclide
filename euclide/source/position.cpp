#include "position.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Pieces::Pieces(const Problem& problem, Color color)
	: color(color)
{
	/* -- Member variable initialization -- */

	requiredMoves = 0;
	requiredMovesByMen = 0;
	requiredMovesBySquares = 0;

	requiredCaptures = 0;
	requiredCapturesByMen = 0;
	requiredCapturesBySquares = 0;

	/* -- Associate with each man a possible destination -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
	{
		Glyph glyph = problem[square];
		glyphs[square] = glyph;

		/* -- Associate glyphs to men -- */

		for (Man man = FirstMan; man <= LastMan; man++)
			if (tables::supermanToGlyph[man][color] == glyph)
				destinations += Destination(square, color, man, man, false);

		/* -- Handle promoted pieces -- */

		for (Superman superman = FirstPromotedMan; superman <= LastPromotedMan; superman++)
			if (tables::supermanToGlyph[superman][color] == glyph)
				for (Man man = FirstPawn; man <= LastPawn; man++)
					destinations += Destination(square, color, man, superman, false);

		/* -- Each man also have been captured on any square -- */

		for (Man man = FirstMan; man <= LastMan; man++)
			destinations += Destination(square, color, man, man, true);

		/* -- Pawns may also have been captured as promoted pieces -- */

		for (Man man = FirstPawn; man <= LastPawn; man++)
			for (Superman superman = FirstPromotedMan; superman <= LastPromotedMan; superman++)
				destinations += Destination(square, color, man, superman, true);
	}
}

/* -------------------------------------------------------------------------- */

bool Pieces::applyNonUbiquityPrinciple()
{
	array<bool, NumSquares> unique(false);
	array<Man, NumSquares> men(UndefinedMan);

	/* -- Scan the list of possible destinations -- */

	for (Destinations::const_iterator I = destinations.begin(); I != destinations.end(); I++)
	{
		/* -- Skip captures -- */

		if (I->captured())
			continue;

		/* -- Check if a single man can end alive on a given square -- */

		Square square = I->square();
		Man man = I->man();

		if (men[square] == UndefinedMan)
		{
			unique[square] = true;
			men[square] = man;
		}
		else
		if (men[square] != man)
			unique[square] = false;
	}

	bool modified = false;

	/* -- If a man is the only one that can end on a given occupied square, 
	      then this man must indeed end on that square -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
	{
		/* -- If there is no possible man for a given occupied square,
		      the problem has no solution -- */

		if (glyphs[square].isColor(color))
			if (men[square] == UndefinedMan)
				abort(NoSolution);

		/* -- Non ubiquity deduction -- */

		if (unique[square])
			if (destinations.setManSquare(men[square], square, false))
				modified = true;
	}	

	/* -- Make some further deductions if possible -- */

	if (modified)
		applyNonUbiquityPrinciple();

	return modified;
}

/* -------------------------------------------------------------------------- */

bool Pieces::applyMoveConstraints(int availableMoves)
{
	array<int, NumMen> availableMovesByMan(availableMoves - requiredMovesByMen);
	array<int, NumSquares> availableMovesByOccupiedSquare(availableMoves - requiredMovesBySquares);
	array<int, NumSquares> availableMovesByUnoccupiedSquare(availableMoves - requiredMovesBySquares);

	const array<int, NumMen>& requiredMovesByMan = destinations.getRequiredMovesByMan();
	const array<int, NumSquares>& requiredMovesByOccupiedSquare = destinations.getRequiredMovesBySquare(false);

	for (Man man = FirstMan; man <= LastMan; man++)
		availableMovesByMan[man] += requiredMovesByMan[man];

	for (Square square = FirstSquare; square <= LastSquare; square++)
		availableMovesByOccupiedSquare[square] += requiredMovesByOccupiedSquare[square];

	return destinations.setAvailableMoves(availableMovesByMan, availableMovesByOccupiedSquare, availableMovesByUnoccupiedSquare);
}

/* -------------------------------------------------------------------------- */

bool Pieces::applyCaptureConstraints(int availableCaptures)
{
	array<int, NumMen> availableCapturesByMan(availableCaptures - requiredCapturesByMen);
	array<int, NumSquares> availableCapturesByOccupiedSquare(availableCaptures - requiredCapturesBySquares);
	array<int, NumSquares> availableCapturesByUnoccupiedSquare(availableCaptures - requiredCapturesBySquares);

	const array<int, NumMen>& requiredCapturesByMan = destinations.getRequiredCapturesByMan();
	const array<int, NumSquares>& requiredCapturesByOccupiedSquare = destinations.getRequiredCapturesBySquare(false);

	for (Man man = FirstMan; man <= LastMan; man++)
		availableCapturesByMan[man] += requiredCapturesByMan[man];

	for (Square square = FirstSquare; square <= LastSquare; square++)
		availableCapturesByOccupiedSquare[square] += requiredCapturesByOccupiedSquare[square];

	return destinations.setAvailableCaptures(availableCapturesByMan, availableCapturesByOccupiedSquare, availableCapturesByUnoccupiedSquare);
}

/* -------------------------------------------------------------------------- */

int Pieces::computeRequiredMoves(const Board& board)
{
	/* -- Update number of required moves -- */

	destinations.updateRequiredMoves(board, castling);

	/* -- Sum number of required moves for each man -- */
	
	const array<int, NumMen>& requiredMovesByMan =
		destinations.getRequiredMovesByMan();

	maximize(requiredMovesByMen, std::accumulate(requiredMovesByMan.begin(), requiredMovesByMan.end(), 0));

	/* -- Sum number of required moves for each occupied square -- */

	const array<int, NumSquares>& requiredMovesBySquare =
		destinations.getRequiredMovesBySquare(false);

	int _requiredMovesBySquares = 0;
	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (glyphs[square].isColor(color))
			_requiredMovesBySquares += requiredMovesBySquare[square];

	maximize(requiredMovesBySquares, _requiredMovesBySquares);

	/* -- Maximize both values -- */

	requiredMoves = std::max(requiredMovesByMen, requiredMovesBySquares);

	/* -- Return value -- */

	return requiredMoves;
}

/* -------------------------------------------------------------------------- */

int Pieces::computeRequiredCaptures(const Board &board)
{
	/* -- Update number of required captures -- */

	destinations.updateRequiredCaptures(board);

	/* -- Sum number of required captures for each man -- */
	
	const array<int, NumMen>& requiredCapturesByMan =
		destinations.getRequiredCapturesByMan();

	maximize(requiredCapturesByMen, std::accumulate(requiredCapturesByMan.begin(), requiredCapturesByMan.end(), 0));

	/* -- Sum number of required captures for each occupied square -- */

	const array<int, NumSquares>& requiredCapturesBySquare =
		destinations.getRequiredCapturesBySquare(false);

	int _requiredCapturesBySquares = 0;
	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (glyphs[square].isColor(color))
			_requiredCapturesBySquares += requiredCapturesBySquare[square];

	maximize(requiredCapturesBySquares, _requiredCapturesBySquares);

	/* -- Maximize both values -- */

	requiredCaptures = std::max(requiredCapturesByMen, requiredCapturesBySquares);

	/* -- Return value -- */

	return requiredCaptures;
}

/* -------------------------------------------------------------------------- */

int Pieces::getRequiredMoves(Man man) const
{
	return destinations.getRequiredMovesByMan()[man];
}

/* -------------------------------------------------------------------------- */

int Pieces::getNumDestinations(Man man) const
{
	return (int)std::count_if(destinations.begin(), destinations.end(), std::bind2nd(std::mem_fun_ref(&Destination::isMan), man));
}

/* -------------------------------------------------------------------------- */

const Destination& Pieces::getDestination(Man man) const
{
	Destinations::const_iterator destination = 
		std::find_if(destinations.begin(), destinations.end(), std::bind2nd(std::mem_fun_ref(&Destination::isMan), man));
	
	return *destination;
}

/* -------------------------------------------------------------------------- */

}
