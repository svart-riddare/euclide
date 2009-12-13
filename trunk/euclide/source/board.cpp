#include "board.h"
#include "position.h"
#include "pieces.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Board::Board()
{
	optimized = true;

	/* -- Initialize movement tables -- */

	for (Color color = FirstColor; color <= LastColor; color++)
		for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
			pieces[color][superman] = new Piece(superman, color);
}

/* -------------------------------------------------------------------------- */

Board::~Board()
{
	/* -- Release movement tables -- */

	for (Color color = FirstColor; color <= LastColor; color++)
		for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
			delete pieces[color][superman];
}

/* -------------------------------------------------------------------------- */

int Board::moves() const
{
	int moves = 0;

	for (Color color = FirstColor; color <= LastColor; color++)
		for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
			if (pieces[color][superman])
				moves += pieces[color][superman]->moves();

	return moves;
}

/* -------------------------------------------------------------------------- */

int Board::moves(Color color) const
{
	assert(color.isValid());

	int moves = 0;

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		if (pieces[color][superman])
			moves += pieces[color][superman]->moves();

	return moves;
}

/* -------------------------------------------------------------------------- */

int Board::moves(Superman superman, Color color) const
{
	assert(superman.isValid());
	assert(color.isValid());

	return pieces[color][superman] ? pieces[color][superman]->moves() : 0;
}

/* -------------------------------------------------------------------------- */

int Board::moves(Man man, Superman superman, Color color) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());
	assert(pieces[color][superman] != NULL);

	int moves = pieces[color][man]->moves();

	if (superman != man)
		moves += pieces[color][superman]->moves();

	return moves;
}

/* -------------------------------------------------------------------------- */

int Board::distance(Man man, Superman superman, Color color, Square from, Square to) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());
	assert(pieces[color][superman] != NULL);

	/* -- No promotion case -- */

	if (man == superman)
		return pieces[color][man]->distance(from, to);

	/* -- Handle promotion -- */

	Square square = superman.square(color);
	return pieces[color][man]->distance(from, square) + pieces[color][superman]->distance(square, to);
}

/* -------------------------------------------------------------------------- */

int Board::distance(Man man, Superman superman, Color color, Square to) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());
	assert(pieces[color][superman] != NULL);

	/* -- Use extended algorithms if board pieces have been altered -- */

	if (!optimized)
		return distance(man, superman, color, man.square(color), to);
			
	/* -- No promotion case -- */

	if (man == superman)
		return pieces[color][man]->distance(to);

	/* -- Handle promotion -- */

	return pieces[color][man]->distance(superman.square(color)) + pieces[color][superman]->distance(to);
}

/* -------------------------------------------------------------------------- */

int Board::captures(Man man, Superman superman, Color color, Square from, Square to) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());
	assert(pieces[color][superman] != NULL);

	/* -- No promotion case -- */

	if (man == superman)
		return pieces[color][man]->captures(from, to);

	/* -- Handle promotion -- */

	Square square = superman.square(color);
	return pieces[color][man]->captures(from, square) + pieces[color][superman]->captures(square, to);
}

/* -------------------------------------------------------------------------- */

int Board::captures(Man man, Superman superman, Color color, Square to) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());
	assert(pieces[color][superman] != NULL);

	/* -- Use extended algorithms if board pieces have been altered -- */

	if (!optimized)
		return captures(man, superman, color, man.square(color), to);
			
	/* -- No promotion case -- */

	if (man == superman)
		return pieces[color][man]->captures(to);

	/* -- Handle promotion -- */

	return pieces[color][man]->captures(superman.square(color)) + pieces[color][superman]->captures(to);
}

/* -------------------------------------------------------------------------- */

int Board::getCaptures(Man man, Superman superman, Color color, Square from, Square to, vector<Squares>& captures) const
{
	assert(man.isValid());
	assert(superman.isValid());
	assert(color.isValid());
	assert(pieces[color][superman] != NULL);

	/* -- No promotion case -- */

	if (man == superman)
		return pieces[color][man]->getCaptures(from, to, captures);

	/* -- Handle promotion -- */

	Square square = superman.square(color);
	vector<Squares> _captures;
	
	pieces[color][man]->getCaptures(from, square, captures);
	pieces[color][superman]->getCaptures(square, to, _captures);

	captures.insert(captures.end(), _captures.begin(), _captures.end());
	return (int)captures.size();
}

/* -------------------------------------------------------------------------- */

bool Board::getUniquePath(Man man, Superman superman, Color color, Square to, vector<Square>& squares) const
{
	assert(man.isValid());
	assert(superman.isValid());
	assert(color.isValid());
	assert(pieces[color][superman] != NULL);

	/* -- No promotion case -- */

	if (man == superman)
		return pieces[color][man]->getUniquePath(man.square(color), to, squares);

	/* -- Handle promotion -- */

	Square square = superman.square(color);
	vector<Square> _squares;

	if (!pieces[color][man]->getUniquePath(man.square(color), square, squares))
		return false;
	if (!pieces[color][superman]->getUniquePath(square, to, _squares))
		return false;

	squares.insert(squares.end(), _squares.begin(), _squares.end());
	return true;
}

/* -------------------------------------------------------------------------- */

void Board::block(Superman man, Color color, Square square, bool captured)
{
	assert(man.isValid());
	assert(color.isValid());
	assert(square.isValid());

	Glyph glyph = man.glyph(color);
	optimized = false;

	/* -- Block pieces for our side -- */

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		if ((superman != man) && pieces[color][superman])
			pieces[color][superman]->block(square, glyph, false);

	/* -- Block pieces for opponent pieces -- */

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		if (pieces[!color][superman])
			pieces[!color][superman]->block(square, glyph, captured);
}

/* -------------------------------------------------------------------------- */

void Board::unblock(Superman man, Color color, Square square, bool captured)
{
	assert(man.isValid());
	assert(color.isValid());
	assert(square.isValid());

	Glyph glyph = man.glyph(color);
	optimized = false;

	/* -- Unblock pieces for our side -- */

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		if ((superman != man) && pieces[color][superman])
			pieces[color][superman]->unblock(square, glyph, false);

	/* -- Unblock pieces for opponent pieces -- */

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		if (pieces[!color][superman])
			pieces[!color][superman]->unblock(square, glyph, captured);
}

/* -------------------------------------------------------------------------- */

void Board::transblock(Superman superman, Color color, Square from, Square to, bool captured)
{
	unblock(superman, color, from);
	block(superman, color, to, captured);
}

/* -------------------------------------------------------------------------- */

void Board::block(Man man, Superman superman, Color color)
{
	assert(man.isValid());
	assert(superman.isValid());
	assert(color.isValid());
	assert(pieces[color][superman] != NULL);

	/* -- Find squares on which the piece must lie -- */

	Squares squares = pieces[color][man]->squares();

	if (man != superman)
		squares |= pieces[color][superman]->squares();

	block(man, superman, color, squares);
}

/* -------------------------------------------------------------------------- */

void Board::block(Man man, Superman _superman, Color color, const Squares& squares)
{
	assert(man.isValid());
	assert(_superman.isValid());

	Glyph glyph = (man == _superman) ? man.glyph(color) : NoGlyph;
	optimized = false;
	
	/* -- Block pieces for our side -- */

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		if ((superman != man) && (superman != _superman) && pieces[color][superman])
			pieces[color][superman]->block(squares, glyph);

	/* -- Block pieces for opponent pieces -- */

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		if (pieces[!color][superman])
			pieces[!color][superman]->block(squares, glyph);
}

/* -------------------------------------------------------------------------- */

void Board::reduce(Man man, Superman superman, Color color, Square square, int availableMoves, int availableCaptures)
{
	assert(man.isValid());
	assert(superman.isValid());
	assert(color.isValid());
	assert(pieces[color][superman] != NULL);

	optimized = false;

	/* -- No promotion case -- */

	if (man == superman)
		pieces[color][man]->reduce(square, availableMoves, availableCaptures);

	/* -- Handle promotion -- */

	else
	{
		Square promotion = superman.square(color);

		int requiredMoves = pieces[color][man]->distance(promotion);
		int requiredCaptures = pieces[color][man]->captures(promotion);

		int promotionMoves = pieces[color][superman]->distance(square);
		int promotionCaptures = pieces[color][superman]->captures(square);

		pieces[color][man]->reduce(promotion, availableMoves - promotionMoves, availableCaptures - promotionCaptures);
		pieces[color][superman]->reduce(square, availableMoves - requiredMoves, availableCaptures - requiredCaptures);
	}
}

/* -------------------------------------------------------------------------- */

void Board::reduce(Man man, Superman superman, Color color, const Squares& squares, int availableMoves, int availableCaptures)
{
	assert(man.isValid());
	assert(superman.isValid());
	assert(color.isValid());
	assert(pieces[color][superman] != NULL);

	optimized = false;

	/* -- No promotion case -- */

	if (man == superman)
		pieces[color][man]->reduce(squares, availableMoves, availableCaptures);

	/* -- Handle promotion -- */

	else
	{
		Square promotion = superman.square(color);

		int requiredMoves = pieces[color][man]->distance(promotion);
		int requiredCaptures = pieces[color][man]->captures(promotion);
		int promotionMoves = pieces[color][superman]->distance(squares);
		int promotionCaptures = pieces[color][superman]->captures(squares);

		pieces[color][man]->reduce(promotion, availableMoves - promotionMoves, availableCaptures - promotionCaptures);
		pieces[color][superman]->reduce(squares, availableMoves - requiredMoves, availableCaptures - requiredCaptures);
	}
}

/* -------------------------------------------------------------------------- */

void Board::reduce(Man man, const Supermen& supermen, Color color, const Squares& squares, int availableMoves, int availableCaptures)
{
	assert(man.isValid());
	assert(color.isValid());
	assert(supermen.count() >= 1);

	optimized = false;

	/* -- Single promotion case -- */

	if (supermen.count() == 1)
	{
		Superman superman = man;
		while (!supermen[superman])
			superman++;

		reduce(man, superman, color, squares, availableMoves, availableCaptures);
	}

	/* -- Handle promotions -- */

	else
	{
		/* -- Compute available moves/captures before promotion -- */

		Squares promotions;
		if (supermen[man])
			promotions = squares;

		int promotionMoves = supermen[man] ? 0 : infinity;
		int promotionCaptures = supermen[man] ? 0 : infinity;

		for (Superman superman = FirstPromotedMan; superman <= LastPromotedMan; superman++)
		{
			if (supermen[superman])
			{
				assert(pieces[color][superman] != NULL);

				promotions[superman.square(color)] = true;
				minimize(promotionMoves, pieces[color][superman]->distance(squares));
				minimize(promotionCaptures, pieces[color][superman]->captures(squares));
			}
		}

		pieces[color][man]->reduce(promotions, availableMoves - promotionMoves, availableCaptures - promotionCaptures); 

		/* -- Handle each possible promotion -- */

		for (Superman superman = FirstPromotedMan; superman <= LastPromotedMan; superman++)
		{
			if (supermen[superman])
			{
				Square promotion = superman.square(color);

				int requiredMoves = pieces[color][man]->distance(promotion);
				int requiredCaptures = pieces[color][man]->captures(promotion);

				assert(pieces[color][superman] != NULL);
				pieces[color][superman]->reduce(squares, availableMoves - requiredMoves, availableCaptures - requiredCaptures);
			}
		}
	}
}

/* -------------------------------------------------------------------------- */

void Board::setCaptureSquares(Man man, Superman superman, Color color, const Squares& squares)
{
	assert(man.isValid());
	assert(superman.isValid());
	assert(color.isValid());
	assert(pieces[color][superman] != NULL);

	optimized = false;

	pieces[color][man]->setCaptureSquares(squares);

	if (man != superman)
		pieces[color][superman]->setCaptureSquares(squares);
}

/* -------------------------------------------------------------------------- */

void Board::optimizeLevelOne(const Position& position, Color color, int availableMoves, int availableCaptures)
{
	/* -- Initialize information tied with each man -- */

	struct 
	{
		int availableMoves;
		int availableCaptures;

		Squares squares;

		Supermen supermen;
		Superman superman;

		bool block;
			
	} men[NumMen];

	for (Man man = FirstMan; man <= LastMan; man++)
	{
		men[man].squares.reset();
		men[man].supermen.reset();

		men[man].availableMoves = 0;
		men[man].availableCaptures = 0;

		men[man].superman = man;

		men[man].block = true;
	}

	/* -- Loop through partitions, targets and destinations to collect the information -- */

	for	(Partitions::const_iterator partition = position.begin(); partition != position.end(); partition++)
	{
		int unassignedMoves = availableMoves - position.requiredMoves() + partition->requiredMoves() - partition->assignedMoves();
		int unassignedCaptures = availableCaptures - position.requiredCaptures() + partition->requiredCaptures() - partition->assignedCaptures();

		for (Targets::const_iterator target = partition->begin(); target != partition->end(); target++)
		{
			for (Destinations::const_iterator destination = target->begin(); destination != target->end(); destination++)
			{
				Man man = destination->man();
				Superman superman = destination->superman();
				Square square = destination->square();

				maximize(men[man].availableMoves, target->requiredMoves() + unassignedMoves);
				maximize(men[man].availableCaptures, target->requiredCaptures() + unassignedCaptures);

				men[man].squares[square] = true;

				if (superman != man)
				{
					men[man].superman = superman;
					men[man].supermen[superman] = true;
				}

				if (destination->captured())
					men[man].block = false;
			}
		}
	}

	/* -- Find possible capture squares -- */

	Squares captures;
	for (Man man = FirstMan; man <= LastMan; man++)
		if (!men[man].block)
			captures |= men[man].squares;

	/* -- Apply capture restrictions -- */

	for (Man man = FirstMan; man <= LastMan; man++)
		setCaptureSquares(man, man, !color, captures);

	/* -- Synchronize castling -- */

	pieces[color][King]->synchronizeCastling(*pieces[color][KingRook], *pieces[color][QueenRook]);

	/* -- Apply path reductions and obstructions for each man -- */

	for (Man man = FirstMan; man <= LastMan; man++)
	{
		/* -- Handle case where the promotion piece is known -- */

		if (men[man].supermen.count() <= 1)
		{
			reduce(man, men[man].superman, color, men[man].squares, men[man].availableMoves, men[man].availableCaptures);

			if (men[man].block)
				block(man, men[man].superman, color);
		}

		/* -- Handle general case -- */

		else
		{
			men[man].supermen[man] = true;
			reduce(man, men[man].supermen, color, men[man].squares, men[man].availableMoves, men[man].availableCaptures);
		}
	}

	/* -- Handle supermen that never came to be -- */

	for (Superman superman = FirstPromotedMan; superman <= LastPromotedMan; superman++)
	{
		if (!men[superman.man()].supermen[superman])
		{
			delete pieces[color][superman];
			pieces[color][superman] = NULL;
		}
	}

	/* -- Complete optimization -- */

	if (!optimized)
		optimize(color);
}

/* -------------------------------------------------------------------------- */

void Board::optimizeLevelTwo(const Position& whitePosition, const Position& blackPosition)
{
	vector<tuple<Man, Color, vector<Square> > > paths;

	/* -- List men that were not captured and have a definite target -- */

	for (Color color = FirstColor; color <= LastColor; color++)
	{
		const Position& position = (color == White) ? whitePosition : blackPosition;

		for (Partitions::const_iterator partition = position.begin(); partition != position.end(); partition++)
		{
			Targets::const_iterator target = partition->begin();

			if (partition->size() > 1)
				continue;

			if (!target->alive())
				continue;

			if (target->man() == UndefinedMan)
				continue;

			if (target->superman() == UndefinedSuperman)
				continue;
			
			if (target->square() == UndefinedSquare)
				continue;

			/* -- Is there a definite path to reach this target ? -- */

			vector<Square> path;
			if (getUniquePath(target->man(), target->superman(), color, target->square(), path))
				if (path.size() > 1)
					paths.push_back(make_tuple(target->man(), color, path));
		}
	}

	/* -- Use the list of well determined paths to find eventual obstructions -- */

	if (!paths.empty())
		for (Color color = FirstColor; color <= LastColor; color++)
			for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
				if (pieces[color][superman])
					pieces[color][superman]->optimize(paths);
}

/* -------------------------------------------------------------------------- */

void Board::optimize(Color color)
{
	optimized = true;

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		if (pieces[color][superman])
			pieces[color][superman]->optimize();
}

/* -------------------------------------------------------------------------- */

}
