#ifndef __EUCLIDE_CACHE_H
#define __EUCLIDE_CACHE_H

#include "includes.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */

class TwoPieceFastCache
{
	public:
		struct Position
		{
			array<Square, 2> squares;    /**< Squares on which the two pieces lie. */
			array<int, 2> moves;         /**< Number of moves played by each piece. */

			Position() = default;
			Position(Square squareA, int movesA, Square squareB, int movesB)
				{ squares = { squareA, squareB }; moves = { movesA, movesB }; }
		};

	public:
		TwoPieceFastCache();

		void add(Square squareA, int movesA, Square squareB, int movesB);
		bool hit(Square squareA, int movesA, Square squareB, int movesB) const;

		void add(const Position& position);
		bool hit(const Position& position) const;

	private:
		typedef struct { short movesA, movesB; } Entry;
		typedef array<Entry, 3> Line;                    /**< A cache line is made of up to three elements. */
		matrix<Line, NumSquares, NumSquares> m_cache;    /**< Cache, one line for each pair of squares. */
};

/* -------------------------------------------------------------------------- */

template<typename State>
class TwoPieceFullCache
{
	public:
		struct Position
		{
			array<Square, 2> squares;    /**< Squares on which the two pieces lie. */
			array<int, 2> moves;         /**< Number of moves played by each piece. */
			array<State, 2> states;      /**< States of each piece. */

			Position() = default;
			Position(Square squareA, int movesA, State stateA, Square squareB, int movesB, State stateB)
				{ squares = { squareA, squareB }; moves = { movesA, movesB }; states = { stateA, stateB }; }
		};

	public:
		TwoPieceFullCache();

		void add(Square squareA, int movesA, State stateA, Square squareB, int movesB, State stateB, int requiredMoves);
		bool hit(Square squareA, int movesA, State stateA, Square squareB, int movesB, State stateB, int *requiredMoves) const;

		void add(const Position& position, int requiredMoves);
		bool hit(const Position& position, int *requiredMoves) const;

	private:
		typedef struct { bool valid; short movesA, movesB; short requiredMoves; State stateA, stateB; } Entry;
		typedef array<Entry, 8> Line;                    /**< A cache line is made of up to eight elements. */
		matrix<Line, NumSquares, NumSquares> m_cache;    /**< Cache, one line for each pair of squares. */

		unsigned m_id;                                   /**< Id used as pseudo-random number to replace cache entries where there is no space left. */
};

/* -------------------------------------------------------------------------- */

class TwoPieceCache : public TwoPieceFullCache<Squares> {};

/* -------------------------------------------------------------------------- */

}

#endif
