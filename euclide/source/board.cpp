#include "board.h"
#include "assignations.h"
#include "implications.h"
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

	_assignedMoves[White] = new Assignations();
	_assignedMoves[Black] = new Assignations();

	_assignedCaptures[White] = new Assignations();
	_assignedCaptures[Black] = new Assignations();

	/* -- Initialize movement tables -- */

	for (Color color = FirstColor; color <= LastColor; color++)
		for (Superman superman = FirstSuperman; superman <= LastSuperman; superman++)
			_pieces[color][superman] = new Piece(superman, color, _problem->moves(color));
}

/* -------------------------------------------------------------------------- */

Board::~Board()
{
	/* -- Destroy member variables -- */

	delete _assignedMoves[White];
	delete _assignedMoves[Black];

	delete _assignedCaptures[White];
	delete _assignedCaptures[Black];

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
	/* -- Loop over both colors -- */

	for (Color color = FirstColor; color <= LastColor; color++)
	{
		/* -- Retrieve position implications -- */

		Implications implications(*_positions[color], *_assignedMoves[color], *_assignedCaptures[color]);

		/* -- Remove useless assignations -- */

		_assignedMoves[color]->cleanup(implications, &Implications::assignedMoves);
		_assignedCaptures[color]->cleanup(implications, &Implications::assignedCaptures);

		/* -- Find possible capture squares -- */

		Squares captures;
		for (Man man = FirstMan; man <= LastMan; man++)
			if (implications.captured(man))
				captures |= implications.squares(man);

		/* -- Synchronize castling -- */

		_pieces[color][King]->synchronizeCastling(*_pieces[color][KingRook], *_pieces[color][QueenRook]);

		/* -- Apply move restrictions -- */

		for (Man man = FirstMan; man <= LastMan; man++)
			setPossibleSquares(man, implications.supermen(man), color, implications.squares(man), implications.captured(man) ? (implications.alive(man) ? tribool(indeterminate) : tribool(true)) : tribool(false), implications.availableMoves(man), implications.availableCaptures(man));

		/* -- Apply capture restrictions -- */

		for (Man man = FirstMan; man <= LastMan; man++)
			setPossibleCaptures(man, man, !color, captures);

		/* -- Block squares -- */

		for (Man man = FirstMan; man <= LastMan; man++)
			if (!implications.captured(man))
				block(man, implications.supermen(man), color);

		/* -- Delete useless supermen -- */

		for (Superman superman = FirstPromotedMan; superman <= LastPromotedMan; superman++)
		{
			if (!implications.supermen(superman.man())[superman])
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
	Moves moves;

	/* -- List mandatory moves for each piece -- */

	for (Color color = FirstColor; color <= LastColor; color++)
	{
		for (Man man = FirstMan; man <= LastMan; man++)
		{
			if (_pieces[color][man])
			{
				_pieces[color][man]->getMandatoryMoves(moves, true);

				/* -- If there are mandatory moves, add constraints -- */

				if (moves.size() > 0)
					for (Color xcolor = FirstColor; xcolor <= LastColor; xcolor++)
						for (Man xman = FirstMan; xman <= LastMan; xman++)
							if (_pieces[xcolor][xman])
								_pieces[xcolor][xman]->setMandatoryMoveConstraints(*_pieces[color][man], moves);
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
}

/* -------------------------------------------------------------------------- */

void Board::optimizeLevelThree()
{
	/* -- Get implications of the current position -- */

	Implications implications[NumColors] = { Implications(*_positions[White], *_assignedMoves[White], *_assignedCaptures[White]), Implications(*_positions[Black], *_assignedMoves[Black], *_assignedCaptures[Black]) };

	/* -- List all mutual obstructions -- */

	std::vector<OptimizeLevelThreeItem> pairs;
	pairs.reserve(NumColors * NumColors * NumMen * NumMen / 2);

	for (Color colorA = FirstColor; colorA <= LastColor; colorA++)
	{
		for (Color colorB = colorA; colorB <= LastColor; colorB++)
		{
			for (Man manA = FirstMan; manA <= LastMan; manA++)
			{
				if (!_pieces[colorA][manA] || !_pieces[colorA][manA]->moves())
					continue;

				for (Man manB = (colorA == colorB) ? (man_t)(manA + 1) : FirstMan; manB <= LastMan; manB++)
				{
					if (!_pieces[colorB][manB] || !_pieces[colorB][manB]->moves())
						continue;

					OptimizeLevelThreeItem pair(manA, manB, colorA, colorB, 
						_pieces[colorA][manA]->moves(), _pieces[colorB][manB]->moves(),
						(colorA == colorB) ? implications[colorA].requiredMoves(manA, manB) : (implications[colorA].requiredMoves(manA) + implications[colorB].requiredMoves(manB)),
						(colorA == colorB) ? implications[colorA].availableMoves(manA, manB) : (implications[colorA].availableMoves(manA) + implications[colorB].availableMoves(manB)));

					pairs.push_back(pair);
				}
			}
		}
	}

	/* -- Sort pairs -- */

	std::sort(pairs.begin(), pairs.end());

	/* -- For each pair, take mutual obstructions between the two pieces into account -- */

	for (std::vector<OptimizeLevelThreeItem>::const_iterator pair = pairs.begin(); pair != pairs.end(); pair++)
	{
		bool assigned = false;
		bool modified = false;

		/* -- Let's not loose ourselves in infinite computations -- */

		bool fast = pair->complexity >= 1000000;
		if (pair->complexity >= 10000000)
			break;

		/* -- Mutual obstructions -- */

		int requiredMoves = -1;
		if (_pieces[pair->colorA][pair->manA]->setMutualObstructions(*_pieces[pair->colorB][pair->manB], pair->availableMoves, pair->assignedMoves, &requiredMoves, fast))
			modified = true;

		if ((requiredMoves > pair->assignedMoves) && (pair->colorA == pair->colorB))
			assigned = true;

		/* -- If we have modified something, complete optimization now -- */

		if (modified)
		{
			_pieces[pair->colorA][pair->manA]->optimize();
			_pieces[pair->colorB][pair->manB]->optimize();
		}

		/* -- If we require more moves than expected, keep information and break -- */

		if (assigned)
			_assignedMoves[pair->colorA]->push_back(Assignation((1 << pair->manA) | (1 << pair->manB), pair->colorA, requiredMoves));
			
		if (assigned && modified)
			break;
	}
}

/* -------------------------------------------------------------------------- */

bool Board::optimize(int level, bool recursive)
{
	/* -- If level is out of bound, just exit -- */

	if ((level <= 0) || (level > 3))
		return false;

	/* -- Save number of possibles moves -- */

	int moves = this->moves();

	/* -- Perform optimization -- */

	if (level == 1)
		optimizeLevelOne();
	if (level == 2)
		optimizeLevelTwo();
	if (level == 3)
		optimizeLevelThree();

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
