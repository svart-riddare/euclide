#include "position.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Pieces::Pieces(const Problem& problem, Color color)
	: destinations(problem, color), targets(problem, color), partitions(targets), color(color)
{
	/* -- Initialize move and capture requirements -- */

	requiredMoves = 0;
	requiredCaptures = 0;

	/* -- Copy problem glyphs -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
		glyphs[square] = problem[square];
}

/* -------------------------------------------------------------------------- */

bool Pieces::applyNonUbiquityPrinciple()
{
#if 1

	targets.update(destinations);

	/* -- Refine partitions if possible -- */

	if (!partitions.refine())
		return false;

	/* -- Aggregate lists of possible squares/shrines for each man -- */

	bitset<NumSquares> empty;

	array<bitset<NumSquares>, NumMen> squares(empty);
	array<bitset<NumSquares>, NumMen> shrines(empty);

	for (Targets::const_iterator target = targets.begin(); target != targets.end(); target++)
		for (Man man = FirstMan; man <= LastMan; man++)
			if (target->isMan(man))
				(target->isOccupied() ? squares : shrines)[man] |= target->squares();

	/* -- Remove destinations which do not satisfy these constraints -- */

	return destinations.setMenSquares(squares, shrines);

#else

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
#endif
}

/* -------------------------------------------------------------------------- */

bool Pieces::applyMoveConstraints(int availableMoves)
{
	/* -- Compute number of available moves -- */

	array<int, NumMen> availableMovesByMan(availableMoves - destinations.getRequiredMoves());
	array<int, NumSquares> availableMovesBySquare(availableMoves - targets.getRequiredMoves());
	array<int, NumSquares> availableMovesByShrine(availableMoves - targets.getRequiredMoves());

	const array<int, NumMen>& requiredMovesByMan = destinations.getRequiredMovesByMan();
	const array<int, NumSquares>& requiredMovesBySquare = destinations.getRequiredMovesBySquare(false);

	for (Man man = FirstMan; man <= LastMan; man++)
		availableMovesByMan[man] += requiredMovesByMan[man];

	for (Square square = FirstSquare; square <= LastSquare; square++)
		availableMovesBySquare[square] += requiredMovesBySquare[square];

	for (Targets::const_iterator target = targets.begin(); target != targets.end(); target++)
		if (!target->isOccupied())
			for (Square square = FirstSquare; square <= LastSquare; square++)
				if (target->isSquare(square))
					maximize(availableMovesByShrine[square], availableMovesBySquare[square] + target->getRequiredMoves());

	/* -- Apply these numbers to remove unreachable destinations -- */

	if (!destinations.setAvailableMoves(availableMovesByMan, availableMovesBySquare, availableMovesByShrine))
		return false;

	updateRequiredMoves(true);
	applyMoveConstraints(availableMoves);

	return true;
}

/* -------------------------------------------------------------------------- */

bool Pieces::applyCaptureConstraints(int availableCaptures)
{
	array<int, NumMen> availableCapturesByMan(availableCaptures - destinations.getRequiredCaptures());
	array<int, NumSquares> availableCapturesBySquare(availableCaptures - targets.getRequiredCaptures());
	array<int, NumSquares> availableCapturesByShrine(availableCaptures - targets.getRequiredCaptures());

	const array<int, NumMen>& requiredCapturesByMan = destinations.getRequiredCapturesByMan();
	const array<int, NumSquares>& requiredCapturesBySquare = destinations.getRequiredCapturesBySquare(false);

	for (Man man = FirstMan; man <= LastMan; man++)
		availableCapturesByMan[man] += requiredCapturesByMan[man];

	for (Square square = FirstSquare; square <= LastSquare; square++)
		availableCapturesBySquare[square] += requiredCapturesBySquare[square];

	for (Targets::const_iterator target = targets.begin(); target != targets.end(); target++)
		if (!target->isOccupied())
			for (Square square = FirstSquare; square <= LastSquare; square++)
				if (target->isSquare(square))
					maximize(availableCapturesByShrine[square], availableCapturesBySquare[square] + target->getRequiredCaptures());

	/* -- Apply these numbers to remove unreachable destinations -- */

	if (!destinations.setAvailableCaptures(availableCapturesByMan, availableCapturesBySquare, availableCapturesByShrine))
		return false;

	updateRequiredCaptures(true);
	applyCaptureConstraints(availableCaptures);

	return true;
}

/* -------------------------------------------------------------------------- */

void Pieces::computeRequiredMoves(const Board& board)
{
	/* -- Update number of required moves -- */

	destinations.computeRequiredMoves(board, castling);
	updateRequiredMoves(false);
}

/* -------------------------------------------------------------------------- */

void Pieces::computeRequiredCaptures(const Board &board)
{
	/* -- Update number of required captures -- */

	destinations.computeRequiredCaptures(board);
	updateRequiredCaptures(false);
}

/* -------------------------------------------------------------------------- */

void Pieces::updateRequiredMoves(bool updateDestinations)
{
	/* -- Update required moves for destinations and targets -- */

	if (updateDestinations)
		destinations.updateRequiredMoves();

	targets.update(destinations);

	/* -- Sum number of required moves for each man and targets -- */

	int requiredMovesByMen = destinations.getRequiredMoves();
	int requiredMovesByTargets = targets.getRequiredMoves();

	/* -- Keep maximum value -- */

	maximize(requiredMoves, requiredMovesByMen);
	maximize(requiredMoves, requiredMovesByTargets);
}

/* -------------------------------------------------------------------------- */

void Pieces::updateRequiredCaptures(bool updateDestinations)
{
	/* -- Update required captures for destinations and targets -- */

	if (updateDestinations)
		destinations.updateRequiredCaptures();

	targets.update(destinations);

	/* -- Sum number of required captures for each man and targets -- */

	int requiredCapturesByMen = destinations.getRequiredCaptures();
	int requiredCapturesByTargets = targets.getRequiredCaptures();

	/* -- Keep maximum value -- */

	maximize(requiredCaptures, requiredCapturesByMen);
	maximize(requiredCaptures, requiredCapturesByTargets);
}

/* -------------------------------------------------------------------------- */

void Pieces::analyseCaptures(const Board& board, const Pieces& pieces)
{
	targets.refine(board, pieces);
	targets.update(destinations);

	destinations.setShrines(targets.getCaptures());
}

/* -------------------------------------------------------------------------- */

bool Pieces::analyseStaticPieces(Board& board)
{
	bool modified = false;

	/* -- Lock pieces that have not moved -- */

	for (Man man = FirstMan; man <= LastMan; man++)
	{
		Glyph glyph = tables::supermanToGlyph[man][color];
		Square square = tables::initialSquares[man][color];

		if (glyphs[square] == glyph)
			modified = board.lock(man, color);		
	}

	/* -- Recursive deductions -- */

	if (modified)
		analyseStaticPieces(board);

	return modified;
}

/* -------------------------------------------------------------------------- */

int Pieces::getRequiredMoves(Man man) const
{
	return destinations.getRequiredMovesByMan()[man];
}

/* -------------------------------------------------------------------------- */

int Pieces::getNumDestinations(Man man) const
{
	return (int)std::count_if(destinations.begin(), destinations.end(), boost::bind(&Destination::isMan, _1, man));
}

/* -------------------------------------------------------------------------- */

const Destination& Pieces::getDestination(Man man) const
{
	Destinations::const_iterator destination = 
		std::find_if(destinations.begin(), destinations.end(), boost::bind(&Destination::isMan, _1, man));
	
	return *destination;
}

/* -------------------------------------------------------------------------- */

}
