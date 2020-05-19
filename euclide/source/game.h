#ifndef __EUCLIDE_GAME_H
#define __EUCLIDE_GAME_H

#include "includes.h"
#include "hashtables.h"

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
	public:
		Game(const EUCLIDE_Configuration& configuration, const EUCLIDE_Callbacks& callbacks, const Problem& problem, const array<Pieces, NumColors>& pieces, const array<int, NumColors>& freeMoves);
		~Game();

		void play();

	protected:
		class State;
		bool play(const State& state);

		State move(const State& state, Square from, Square to, CastlingSide castling);
		void undo(const State& state);

		bool checks(Glyph glyph, Square from, Square king) const;
		bool checked(Square king, Color color) const;
		bool checked(Square king, Square free, Color color) const;

		bool solved() const;
		void cmoves(EUCLIDE_Move *moves, int nmoves) const;

	protected:
		class State
		{
			public:
				State(const Problem& problem);
				State(const State& state, Square from, Square to, const Piece *captured, CastlingSide castling, const array<bool, NumCastlingSides>& castlings);

				inline void check(bool check)
					{ m_check = check; }

			public:
				inline const array<bool, NumCastlingSides>& castlings(Color color) const
					{ return m_castlings[color]; }
				inline bool castling(CastlingSide side) const
					{ return m_castlings[m_color][side]; }
				inline bool enpassant(Square square) const
					{ return m_enpassant == square; }
				inline Color color() const
					{ return m_color; }
				inline bool check() const
					{ return m_check; }

				inline CastlingSide castling() const
					{ return m_castling; }
				inline const Piece *captured() const
					{ return m_captured; }
				inline Square from() const
					{ return m_from; }
				inline Square to() const
					{ return m_to; }

			private:
				matrix<bool, NumColors, NumCastlingSides> m_castlings;    /**< Castling rights. */
				Square m_enpassant;                                       /**< Possible en passant capture. */
				Color m_color;                                            /**< Whose turn it is. */
				bool m_check;                                             /**< Set if side to move is in check. */

				CastlingSide m_castling;                                  /**< Set if last move was a castling move. */
				const Piece *m_captured;                                  /**< Piece captured last move. */
				Square m_from;                                            /**< Last move departure square. */
				Square m_to;                                              /**< Last move arrival square. */
		};

	private:
		const EUCLIDE_Configuration m_configuration;        /**< Euclide configuration. */
		const EUCLIDE_Callbacks m_callbacks;                /**< Euclide callbacks. */

		const Problem& m_problem;                           /**< Problem to solve. */
		const array<Pieces, NumColors>& m_pieces;           /**< Problem pieces. */

		array<ArrayOfSquares, NumGlyphs> m_captures;        /**< Legal captures, for checks. */
		array<MatrixOfSquares, NumGlyphs> m_constraints;    /**< Move constraints, for checks. */
		array<MatrixOfSquares, NumColors> m_lines;          /**< Line of sights, for checks and discovered checks. */

		array<const Piece *, NumSquares> m_board;           /**< Current board position. */
		array<Squares, NumColors> m_position;               /**< Current occupied squares. */
		array<Square, NumColors> m_kings;                   /**< Current king positions. */
		array<int, NumColors> m_moves;                      /**< Current number of free moves available. */
		HashPosition m_hash;                                /**< Position encoded for hash tables. */
		Squares m_diagram;                                  /**< Occupied squares to reach. */

		std::vector<const State *> m_states;                /**< Game states, excluding initial state. */

		HashTable m_cache;                                  /**< Cache of already explored positions. */

		int64_t m_positions;                                /**< Number of positions examined. */
		int m_solutions;                                    /**< Number of solutions found. */
};

/* -------------------------------------------------------------------------- */

}

#endif
