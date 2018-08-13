#ifndef __EUCLIDE_GAME_H
#define __EUCLIDE_GAME_H

#include "includes.h"

namespace Euclide
{

class Problem;
class Pieces;
class Piece;

/* -------------------------------------------------------------------------- */
/* -- GameState                                                            -- */
/* -------------------------------------------------------------------------- */

class Game
{
	public :
		Game(const Problem& problem, const array<Pieces, NumColors>& pieces);
		~Game();

		void play(const EUCLIDE_Callbacks& callbacks);

	protected :
		bool solved() const;
		
	protected :
		class State
		{
			public :
				State(const Problem& problem, const array<Pieces, NumColors>& pieces);

				const Piece *move(Square from, Square to);
				void undo(Square from, Square to, const Piece *capture);

				struct Move { Square from, to; Move() {} Move(Square from, Square to) : from(from), to(to) {}};
				void moves(std::vector<Move>& moves);

				void moves(const Problem& problem, int nmoves, EUCLIDE_Move *moves) const;

			public :
				inline const Piece& piece(Square square) const
					{ assert(_pieces[square]); return *_pieces[square]; }
				inline Squares squares() const
					{ return _squares; }
				inline int move() const
					{ return _move; }

			private :
				array<const Piece *, NumSquares> _pieces;    /**< Current board position. */
				Squares _squares;                            /**< Current occupied squares. */
				Color _color;                                /**< Whose turn it is. */
				int _move;                                   /**< Move index, from 0. */

				array<Move, MaxMoves> _moves;                /**< Current game moves. */
		};

	private :
		const Problem& _problem;                    /**< Problem to solve. */
		const array<Pieces, NumColors>& _pieces;    /**< Problem pieces. */

		Squares _diagram;                           /**< Final diagram to reach, used to speed up solved() function. */
		State _state;                               /**< Current game state. */

		int64_t _positions;                         /**< Number of positions examined. */
		int _solutions;                             /**< Number of solutions found. */
};

/* -------------------------------------------------------------------------- */

}

#endif
