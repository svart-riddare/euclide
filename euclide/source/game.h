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
		Game(const EUCLIDE_Configuration& configuration, const EUCLIDE_Callbacks& callbacks, const Problem& problem, const array<Pieces, NumColors>& pieces);
		~Game();

		void play();
		
	protected :
		class State;
		void play(const State& state);

		State move(const State& state, Square from, Square to, CastlingSide castling);
		void undo(const State& state);

		bool checks(Glyph glyph, Square from, Square king) const;
		bool checked(Square king, Color color) const;
		bool checked(Square king, Square free, Color color) const;

		bool solved() const;
		void cmoves(EUCLIDE_Move *moves, int nmoves) const;

	protected :
		class State
		{
			public :
				State(const Problem& problem);
				State(const State& state, Square from, Square to, const Piece *captured, CastlingSide castling, const array<bool, NumCastlingSides>& castlings);

				inline void check(bool check)
					{ _check = check; }

			public :
				inline bool castling(CastlingSide side) const
					{ return _castlings[_color][side]; }
				inline bool enpassant(Square square) const
					{ return _enpassant == square; }
				inline Color color() const
					{ return _color; }
				inline bool check() const
					{ return _check; }

				inline CastlingSide castling() const
					{ return _castling; } 
				inline const Piece *captured() const
					{ return _captured; }
				inline Square from() const
					{ return _from; }
				inline Square to() const
					{ return _to; }

			private :
				matrix<bool, NumColors, NumCastlingSides> _castlings;    /**< Castling rights. */
				Square _enpassant;                                       /**< Possible en passant capture. */
				Color _color;                                            /**< Whose turn it is. */
				bool _check;                                             /**< Set if side to move is in check. */

				CastlingSide _castling;                                  /**< Set if last move was a castling move. */
				const Piece *_captured;                                  /**< Piece captured last move. */
				Square _from;                                            /**< Last move departure square. */
				Square _to;                                              /**< Last move arrival square. */			
		};

	private :
		const EUCLIDE_Configuration _configuration;        /**< Euclide configuration. */
		const EUCLIDE_Callbacks _callbacks;                /**< Euclide callbacks. */

		const Problem& _problem;                           /**< Problem to solve. */
		const array<Pieces, NumColors>& _pieces;           /**< Problem pieces. */

		array<ArrayOfSquares, NumGlyphs> _captures;        /**< Legal captures, for checks. */
		array<MatrixOfSquares, NumGlyphs> _constraints;    /**< Move constraints, for checks. */
		array<MatrixOfSquares, NumColors> _lines;          /**< Line of sights, for checks and discovered checks. */

		array<const Piece *, NumSquares> _board;           /**< Current board position. */
		array<Squares, NumColors> _position;               /**< Current occupied squares. */
		array<Square, NumColors> _kings;                   /**< Current king positions. */
		Squares _diagram;                                  /**< Occupied squares to reach. */

		std::vector<const State *> _states;                /**< Game states, excluding initial state. */

		int64_t _positions;                                /**< Number of positions examined. */
		int _solutions;                                    /**< Number of solutions found. */
};

/* -------------------------------------------------------------------------- */

}

#endif
