#include "position.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Pieces::Pieces(const Problem& problem, Color color)
{
	/* -- Member variable initialization -- */

	this->color = color;

	requiredMoves = 0;
	requiredCaptures = 0;

	/* -- Associate with each man a possible final square -- */

	for (Square square = FirstSquare; square <= LastSquare; square++)
	{
		Glyph glyph = problem[square];
		glyphs[square] = glyph;

		/* -- Associate glyphs to men -- */

		for (Man man = FirstMan; man <= LastMan; man++)
			if (tables::supermanToGlyph[man][color] == glyph)
				squares[man] += FinalSquare(square, man, man, false);

		/* -- Handle promoted pieces -- */

		for (Superman superman = FirstPromotedMan; superman <= LastPromotedMan; superman++)
			if (tables::supermanToGlyph[superman][color] == glyph)
				for (Man man = FirstPawn; man <= LastPawn; man++)
					squares[man] += FinalSquare(square, man, superman, false);

		/* -- Each man also have been captured on any square -- */

		for (Man man = FirstMan; man <= LastMan; man++)
			squares[man] += FinalSquare(square, man, man, true);

		/* -- Pawns may also have been captured as promoted pieces -- */

		for (Man man = FirstPawn; man <= LastPawn; man++)
			for (Superman superman = FirstPromotedMan; superman <= LastPromotedMan; superman++)
				squares[man] += FinalSquare(square, man, superman, true);
	}
}

/* -------------------------------------------------------------------------- */

bool Pieces::applyNonUbiquityPrinciple()
{
	array<bool, NumSquares> unique;
	array<Man, NumSquares> men;

	unique.assign(false);
	men.assign(UndefinedMan);

	/* -- Scan, for each man, the list of possible final squares -- */

	for (Man man = FirstMan; man <= LastMan; man++)
	{
		const finalsquares_t& squares = this->squares[man];

		for (finalsquares_t::const_iterator I = squares.begin(); I != squares.end(); I++)
		{
			/* -- Skip captures -- */

			if (I->isEmpty())
				continue;

			/* -- Check if it is the only man that can end alive on this square -- */

			Square square = *I;

			if (men[square] == UndefinedMan)
			{
				unique[square] = true;
				men[square] = man;
			}
			else
			if (men[square] != man)
				unique[square] = false;
		}
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
			if (squares[men[square]].applyDeduction(square, false))
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
	bool modified = false;

	/* -- Compute number of assigned moves to men -- */

	int requiredMoves = 0;
	for (Man man = FirstMan; man <= LastMan; man++)
		requiredMoves += squares[man].getRequiredMoves();

	/* -- Apply move constraint to each man -- */

	for (Man man = FirstMan; man <= LastMan; man++)
		if (squares[man].applyDeduction(squares[man].getRequiredMoves() + availableMoves - requiredMoves, infinity))
			modified = true;

	/* -- Compute number of assigned moves to each square -- */

	requiredMoves = 0;
	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (glyphs[square].isColor(color))
			requiredMoves += assignedMoves[square];

	/* -- Compute number of available moves for each square -- */

	array<int, NumSquares> unassignedMoves;
	array<int, NumSquares> unassignedCaptures;
	
	unassignedMoves.assign(availableMoves - requiredMoves);
	unassignedCaptures.assign(infinity);

	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (glyphs[square].isColor(color))
			unassignedMoves[square] += assignedMoves[square];

	/* -- Apply move constraints -- */

	for (Man man = FirstMan; man <= LastMan; man++)
		if (squares[man].applyDeduction(unassignedMoves, unassignedCaptures))
			modified = true;

	return modified;
}

/* -------------------------------------------------------------------------- */

bool Pieces::applyCaptureConstraints(int availableCaptures)
{
	bool modified = false;

	/* -- Compute number of assigned captures to men -- */

	int requiredCaptures = 0;
	for (Man man = FirstMan; man <= LastMan; man++)
		requiredCaptures += squares[man].getRequiredCaptures();

	/* -- Apply capture constraint to each man -- */

	for (Man man = FirstMan; man <= LastMan; man++)
		if (squares[man].applyDeduction(infinity, squares[man].getRequiredCaptures() + availableCaptures - requiredCaptures))
			modified = true;

	/* -- Compute number of assigned captures to each square -- */

	requiredCaptures = 0;
	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (glyphs[square].isColor(color))
			requiredCaptures += assignedCaptures[square];

	/* -- Compute number of available moves for each square -- */

	array<int, NumSquares> unassignedMoves;
	array<int, NumSquares> unassignedCaptures;
	
	unassignedCaptures.assign(availableCaptures - requiredCaptures);
	unassignedMoves.assign(infinity);

	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (glyphs[square].isColor(color))
			unassignedCaptures[square] += assignedCaptures[square];

	/* -- Apply capture constraints -- */

	for (Man man = FirstMan; man <= LastMan; man++)
		if (squares[man].applyDeduction(unassignedMoves, unassignedCaptures))
			modified = true;

	return modified;
}

/* -------------------------------------------------------------------------- */

int Pieces::computeRequiredMoves(const Board& board)
{
	int requiredMoves = 0;

	/* -- Sum number of required moves for each man -- */

	for (Man man = FirstMan; man <= LastMan; man++)
		requiredMoves += squares[man].computeRequiredMoves(board, color, castling);

	/* -- Keep result -- */

	if (requiredMoves > this->requiredMoves)
		this->requiredMoves = requiredMoves;

	/* -- Sum required moves for each occupied square -- */

	assignedMoves.assign(infinity);

	for (Man man = FirstMan; man <= LastMan; man++)
		squares[man].getRequiredMoves(assignedMoves);

	requiredMoves = 0;
	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (glyphs[square].isColor(color))
			requiredMoves += assignedMoves[square];

	/* -- Keep result -- */

	if (requiredMoves > this->requiredMoves)
		this->requiredMoves = requiredMoves;

	/* -- Return number of required moves -- */

	return getRequiredMoves();
}

/* -------------------------------------------------------------------------- */

int Pieces::getRequiredMoves() const
{
	return requiredMoves;
}

/* -------------------------------------------------------------------------- */

int Pieces::computeRequiredCaptures(const Board &board)
{
	int requiredCaptures = 0;

	/* -- Sum number of required captures for each man -- */

	for (Man man = FirstMan; man <= LastMan; man++)
		requiredCaptures += squares[man].computeRequiredCaptures(board, color);

	/* -- Keep result -- */

	if (requiredCaptures > this->requiredCaptures)
		this->requiredCaptures = requiredCaptures;

	/* -- Sum required captures for each occupied square -- */

	assignedCaptures.assign(infinity);

	for (Man man = FirstMan; man <= LastMan; man++)
		squares[man].getRequiredCaptures(assignedCaptures);

	requiredCaptures = 0;
	for (Square square = FirstSquare; square <= LastSquare; square++)
		if (glyphs[square].isColor(color))
			requiredCaptures += assignedCaptures[square];

	/* -- Keep result -- */

	if (requiredCaptures > this->requiredCaptures)
		this->requiredCaptures = requiredCaptures;

	/* -- Return number of required captures -- */

	return getRequiredCaptures();
}

/* -------------------------------------------------------------------------- */

int Pieces::getRequiredCaptures() const
{
	return requiredCaptures;
}

/* -------------------------------------------------------------------------- */

const FinalSquares& Pieces::operator[](Man man) const
{
	return squares[man];
}

/* -------------------------------------------------------------------------- */

}
