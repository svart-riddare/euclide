#include "board.h"
#include "position.h"
#include "pieces.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Board::Board(const Position& whitePosition, const Position& blackPosition, const Problem& problem)
{
	/* -- Initialize member variables -- */

	_positions[White] = &whitePosition;
	_positions[Black] = &blackPosition;

	_problem = &problem;

	/* -- Initialize movement tables -- */

	for (Color color = FirstColor; color <= LastColor; color++)
		for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
			_pieces[color][superman] = new Piece(superman, color, _problem->moves(color));
}

/* -------------------------------------------------------------------------- */

Board::~Board()
{
	/* -- Release movement tables -- */

	for (Color color = FirstColor; color <= LastColor; color++)
		for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
			delete _pieces[color][superman];
}

/* -------------------------------------------------------------------------- */

int Board::moves() const
{
	int whiteMoves = moves(White);
	int blackMoves = moves(Black);

	return whiteMoves + blackMoves;
}

/* -------------------------------------------------------------------------- */

int Board::moves(Color color) const
{
	assert(color.isValid());

	int moves = 0;

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		if (_pieces[color][superman])
			moves += _pieces[color][superman]->moves();

	return moves;
}

/* -------------------------------------------------------------------------- */

int Board::moves(Superman superman, Color color) const
{
	assert(superman.isValid());
	assert(color.isValid());

	return _pieces[color][superman] ? _pieces[color][superman]->moves() : 0;
}

/* -------------------------------------------------------------------------- */

int Board::moves(Man man, Superman superman, Color color) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());

	assert(_pieces[color][superman] != NULL);

	int moves = _pieces[color][man]->moves();

	if (superman != man)
		moves += _pieces[color][superman]->moves();

	return moves;
}

/* -------------------------------------------------------------------------- */

int Board::distance(Man man, Superman superman, Color color, Square from, Square to) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());
	
	assert(_pieces[color][superman] != NULL);

	/* -- No promotion case -- */

	if (man == superman)
		return _pieces[color][man]->distance(from, to);

	/* -- Handle promotion -- */

	Square square = superman.square(color);
	return _pieces[color][man]->distance(from, square) + _pieces[color][superman]->distance(square, to);
}

/* -------------------------------------------------------------------------- */

int Board::distance(Man man, Superman superman, Color color, Square to) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());

	assert(_pieces[color][superman] != NULL);

	/* -- No promotion case -- */

	if (man == superman)
		return _pieces[color][man]->distance(to);

	/* -- Handle promotion -- */

	return _pieces[color][man]->distance(superman.square(color)) + _pieces[color][superman]->distance(to);
}

/* -------------------------------------------------------------------------- */

int Board::captures(Man man, Superman superman, Color color, Square from, Square to) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());

	assert(_pieces[color][superman] != NULL);

	/* -- No promotion case -- */

	if (man == superman)
		return _pieces[color][man]->captures(from, to);

	/* -- Handle promotion -- */

	Square square = superman.square(color);
	return _pieces[color][man]->captures(from, square) + _pieces[color][superman]->captures(square, to);
}

/* -------------------------------------------------------------------------- */

int Board::captures(Man man, Superman superman, Color color, Square to) const
{
	assert(man.isValid());
	assert(color.isValid());
	assert(superman.isValid());

	assert(_pieces[color][superman] != NULL);

	/* -- No promotion case -- */

	if (man == superman)
		return _pieces[color][man]->captures(to);

	/* -- Handle promotion -- */

	return _pieces[color][man]->captures(superman.square(color)) + _pieces[color][superman]->captures(to);
}

/* -------------------------------------------------------------------------- */

int Board::getCaptures(Man man, Superman superman, Color color, Square from, Square to, vector<Squares>& captures) const
{
	assert(man.isValid());
	assert(superman.isValid());
	assert(color.isValid());

	assert(_pieces[color][superman] != NULL);

	/* -- No promotion case -- */

	if (man == superman)
		return _pieces[color][man]->getCaptures(from, to, captures);

	/* -- Handle promotion -- */

	Square square = superman.square(color);
	vector<Squares> _captures;
	
	_pieces[color][man]->getCaptures(from, square, captures);
	_pieces[color][superman]->getCaptures(square, to, _captures);

	captures.insert(captures.end(), _captures.begin(), _captures.end());
	return (int)captures.size();
}

/* -------------------------------------------------------------------------- */

void Board::block(Superman man, Color color, Square square, bool captured)
{
	assert(man.isValid());
	assert(color.isValid());
	assert(square.isValid());

	Glyph glyph = man.glyph(color);

	/* -- Block pieces for our side -- */

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		if ((superman != man) && _pieces[color][superman])
			_pieces[color][superman]->block(square, glyph, false);

	/* -- Block pieces for opponent pieces -- */

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		if (_pieces[!color][superman])
			_pieces[!color][superman]->block(square, glyph, captured);
}

/* -------------------------------------------------------------------------- */

void Board::unblock(Superman man, Color color, Square square, bool captured)
{
	assert(man.isValid());
	assert(color.isValid());
	assert(square.isValid());

	Glyph glyph = man.glyph(color);

	/* -- Unblock pieces for our side -- */

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		if ((superman != man) && _pieces[color][superman])
			_pieces[color][superman]->unblock(square, glyph, false);

	/* -- Unblock pieces for opponent pieces -- */

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		if (_pieces[!color][superman])
			_pieces[!color][superman]->unblock(square, glyph, captured);
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

	assert(_pieces[color][superman] != NULL);

	/* -- Find squares on which the piece must lie -- */

	Squares squares = _pieces[color][man]->squares();

	if (man != superman)
		squares |= _pieces[color][superman]->squares();

	/* -- Block given these squares -- */

	block(man, superman, color, squares);
}

/* -------------------------------------------------------------------------- */

void Board::block(Man man, const Supermen& supermen, Color color)
{
	assert(man.isValid());
	assert(supermen.any());
	assert(color.isValid());

	/* -- Find squares on which the piece must lie -- */

	Squares squares = _pieces[color][man]->squares();

	for (Superman superman = FirstPromotedMan; superman <= LastSuperman; superman++)
		if (supermen[superman])
			squares |= _pieces[color][superman]->squares();

	/* -- Block given these squares -- */

	block(man, supermen, color, squares);
}

/* -------------------------------------------------------------------------- */

void Board::block(Man man, Superman superman, Color color, const Squares& squares)
{
	assert(man.isValid());
	assert(superman.isValid());
	assert(color.isValid());
	assert(squares.any());

	/* -- Just call more generic function -- */

	Supermen supermen;

	supermen[man] = true;
	supermen[superman] = true;

	block(man, supermen, color, squares);
}

/* -------------------------------------------------------------------------- */

void Board::block(Man man, const Supermen& supermen, Color color, const Squares& squares)
{
	assert(man.isValid());
	assert(supermen.any());
	assert(color.isValid());
	assert(squares.any());

	/* -- Find out glyph of blocking piece -- */

	Glyph glyph = man.glyph(color);
	if ((supermen.count() > 1) || !supermen[man])
		glyph = NoGlyph;

	/* -- Block our pieces -- */

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		if ((superman != man) && !supermen[superman] && _pieces[color][superman])
			_pieces[color][superman]->block(squares, glyph);

	/* -- Block opponent pieces -- */

	for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
		if (_pieces[!color][superman])
			_pieces[!color][superman]->block(squares, glyph);
}

/* -------------------------------------------------------------------------- */

void Board::setPossibleSquares(Man man, const Supermen& supermen, Color color, const Squares& squares, tribool captured, int availableMoves, int availableCaptures)
{
	assert(man.isValid());
	assert(color.isValid());
	assert(supermen.count() >= 1);

	/* -- No promotion case -- */

	if ((supermen.count() == 1) && supermen[man])
		_pieces[color][man]->setPossibleSquares(squares, captured, availableMoves, availableCaptures);

	/* -- Promotion case -- */

	else
	{
		/* -- Handle piece before its promotion -- */

		Squares promotions;
		if (supermen[man])
			promotions = squares;

		int promotionMoves = supermen[man] ? 0 : infinity;
		int promotionCaptures = supermen[man] ? 0 : infinity;

		for (Superman superman = FirstPromotedMan; superman <= LastPromotedMan; superman++)
		{
			if (supermen[superman])
			{
				assert(_pieces[color][superman] != NULL);

				promotions[superman.square(color)] = true;
				minimize(promotionMoves, _pieces[color][superman]->distance(squares));
				minimize(promotionCaptures, _pieces[color][superman]->captures(squares));
			}
		}

		_pieces[color][man]->setPossibleSquares(promotions, captured, availableMoves - promotionMoves, availableCaptures - promotionCaptures); 

		/* -- Handle each possible promotion -- */

		for (Superman superman = FirstPromotedMan; superman <= LastPromotedMan; superman++)
		{
			if (supermen[superman])
			{
				Square promotion = superman.square(color);

				int requiredMoves = _pieces[color][man]->distance(promotion);
				int requiredCaptures = _pieces[color][man]->captures(promotion);

				assert(_pieces[color][superman] != NULL);
				_pieces[color][superman]->setPossibleSquares(squares, captured, availableMoves - requiredMoves, availableCaptures - requiredCaptures);
			}
		}
	}
}

/* -------------------------------------------------------------------------- */

void Board::setPossibleCaptures(Man man, Superman superman, Color color, const Squares& squares)
{
	assert(man.isValid());
	assert(superman.isValid());
	assert(color.isValid());
	
	assert(_pieces[color][superman] != NULL);

	/* -- Set possible capture squares -- */

	_pieces[color][man]->setPossibleCaptures(squares);

	if (man != superman)
		_pieces[color][superman]->setPossibleCaptures(squares);
}

/* -------------------------------------------------------------------------- */

void Board::optimizeLevelOne()
{
	/* -- Temporary structure describing constraints for each man -- */

	struct 
	{
		int availableMoves;
		int availableCaptures;

		Squares squares;

		Supermen supermen;
		Superman superman;

		bool captured;
		bool alive;
			
	} men[NumMen];

	/* -- Loop over both colors -- */

	for (Color color = FirstColor; color <= LastColor; color++)
	{
		const Position *position = _positions[color];

		int availableMoves = _problem->moves(color);
		int availableCaptures = _problem->captures(color);

		/* -- Initialize local information -- */

		for (Man man = FirstMan; man <= LastMan; man++)
		{
			men[man].squares.reset();
			men[man].supermen.reset();

			men[man].availableMoves = 0;
			men[man].availableCaptures = 0;

			men[man].captured = false;
			men[man].alive = false;
		}

		/* -- Loop through partitions, targets and destinations to collect the information -- */

		for	(Partitions::const_iterator partition = position->begin(); partition != position->end(); partition++)
		{
			int unassignedMoves = availableMoves - position->requiredMoves() + partition->requiredMoves() - partition->assignedMoves();
			int unassignedCaptures = availableCaptures - position->requiredCaptures() + partition->requiredCaptures() - partition->assignedCaptures();

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
					men[man].supermen[superman] = true;

					if (destination->captured())
						men[man].captured = true;
					else
						men[man].alive = true;
				}
			}
		}

		/* -- Find possible capture squares -- */

		Squares captures;
		for (Man man = FirstMan; man <= LastMan; man++)
			if (men[man].captured)
				captures |= men[man].squares;

		/* -- Synchronize castling -- */

		_pieces[color][King]->synchronizeCastling(*_pieces[color][KingRook], *_pieces[color][QueenRook]);

		/* -- Apply move restrictions -- */

		for (Man man = FirstMan; man <= LastMan; man++)
			setPossibleSquares(man, men[man].supermen, color, men[man].squares, men[man].captured ? (men[man].alive ? tribool(indeterminate) : true) : false, men[man].availableMoves, men[man].availableCaptures);

		/* -- Apply capture restrictions -- */

		for (Man man = FirstMan; man <= LastMan; man++)
			setPossibleCaptures(man, man, !color, captures);

		/* -- Block squares -- */

		for (Man man = FirstMan; man <= LastMan; man++)
			if (!men[man].captured)
				block(man, men[man].supermen, color);

		/* -- Delete useless supermen -- */

		for (Superman superman = FirstPromotedMan; superman <= LastPromotedMan; superman++)
		{
			if (!men[superman.man()].supermen[superman])
			{
				delete _pieces[color][superman];
				_pieces[color][superman] = NULL;
			}
		}
	}

	/* -- Complete optimization -- */
		
	for (Color color = FirstColor; color <= LastColor; color++)
		for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
			if (_pieces[color][superman])
				_pieces[color][superman]->optimize();
}

/* -------------------------------------------------------------------------- */

void Board::optimizeLevelTwo()
{
#if 0
	Moves moves;

	/* -- List mandatory moves for each piece -- */

	for (Color color = FirstColor; color <= LastColor; color++)
	{
		for (Man man = FirstMan; man <= LastMan; man++)
		{
			if (_pieces[color][man])
			{
				_pieces[color][man]->getMandatoryMoves(moves);

				/* -- If there are mandatory moves, add constraints -- */

				if (moves.size() > 0)
					for (Color xcolor = FirstColor; xcolor <= LastColor; xcolor++)
						for (Man xman = FirstMan; xman <= LastMan; xman++)
							if (_pieces[xcolor][xman])
								_pieces[xcolor][xman]->setMandatoryMoves(*_pieces[color][man], moves);
			}
		}
	}

	/* -- Apply these constraints -- */

	bool constrain = true;

	while (constrain)
	{
		constrain = false;
	
		for (Color color = FirstColor; color <= LastColor; color++)
			for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
				if (_pieces[color][superman])
					if (_pieces[color][superman]->constrain())
						constrain = true;
	}
#endif
}

/* -------------------------------------------------------------------------- */

bool Board::optimize(int level, bool recursive)
{
	/* -- If level is out of bound, just exit -- */

	if ((level <= 0) || (level > 2))
		return false;

	/* -- Save number of possibles moves -- */

	int moves = this->moves();

	/* -- Perform optimization -- */

	if (level == 1)
		optimizeLevelOne();
	if (level == 2)
		optimizeLevelTwo();

	/* -- Have we optimized something ? -- */

	bool optimized = (this->moves() < moves) ? true : false;

	/* -- Let's perform more computations if we have not optimized anything -- */

	if (!optimized && recursive)
		return optimize(level + 1, recursive);

	/* -- Let's drop back to level one if instead we have found something -- */

	if (optimized && recursive && (level > 1))
		optimize(1, false);

	/* -- Return result -- */

	return optimized;
}

/* -------------------------------------------------------------------------- */

}
