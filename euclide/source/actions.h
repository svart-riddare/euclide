#ifndef __EUCLIDE_ACTIONS_H
#define __EUCLIDE_ACTIONS_H

#include "includes.h"
#include "hashtables.h"

namespace Euclide
{

class Piece;
class Action;
class Problem;

/* -------------------------------------------------------------------------- */
/* -- Consequence                                                          -- */
/* -------------------------------------------------------------------------- */

class Consequence
{
	public:
		Consequence(const Action& action, const Piece& piece);

		void updateRequiredMoves(Square square, int requiredMoves);
		void updateRequiredMoves(const array<int, NumSquares>& requiredMoves);

	public:
		inline const Action& action() const
			{ return m_action; }
		inline const Piece& piece() const
			{ return m_piece; }

		inline int requiredMoves(Square square) const
			{ return m_requiredMoves[square]; }

	private:
		const Action& m_action;                      /**< Action having consequences. */
		const Piece& m_piece;	                     /**< Piece facing the consequence. */

		array<short, NumSquares> m_requiredMoves;    /**< Required moves for above piece given the piece position if action is performed. */
};

/* -------------------------------------------------------------------------- */
/* -- Consequences                                                         -- */
/* -------------------------------------------------------------------------- */

class Consequences
{
	public:
		Consequences(const Action& action);

		void updateRequiredMoves(const Piece& piece, Square square, int requiredMoves);
		void updateRequiredMoves(const Piece& piece, const array<int, NumSquares>& requiredMoves);

	public:
		inline const std::list<Consequence>& consequences() const
			{ return m_consequences; }

	protected:
		Consequence *consequence(const Piece& piece);

	private:
		const Action& m_action;                                  /**< Action having consequences. */

		std::list<Consequence> m_consequences;                   /** List of consequences. */
		matrix<Consequence *, MaxPieces, NumColors> m_matrix;    /**< Index for fast access in above list. */
};

/* -------------------------------------------------------------------------- */
/* -- Action                                                               -- */
/* -------------------------------------------------------------------------- */

class Action
{
	public:
		Action(const Piece& piece, Square from, Square to);

		void mandatory(bool mandatory);
		void unique(bool unique);

	public :
		inline Square from() const
			{ return m_from; }
		inline Square to() const
			{ return m_to; }

		inline bool mandatory() const
			{ return m_mandatory; }
		inline bool unique() const
			{ return m_unique; }

		inline const Consequences& consequences() const
			{ return m_consequences; }
		inline Consequences& consequences()
			{ return m_consequences; }

	private:
		const Piece& m_piece;			/**< Piece performing the action. */

		Square m_from;					/**< Action source square. */
		Square m_to;					/**< Action destination square. */

		bool m_mandatory;				/**< Set if action must be part of the solution. */
		bool m_unique;					/**< Set if this action is performed only once. */

		Consequences m_consequences;    /**< Consequences of this action. */
};

/* -------------------------------------------------------------------------- */
/* -- Actions                                                              -- */
/* -------------------------------------------------------------------------- */

class Actions : public std::list<Action>
{
	public:
		Actions(const Piece& piece);

		void clean();

	public:
		const Action& get(Square from, Square to) const
			{ return *m_actions[from][to]; }
		Action& get(Square from, Square to)
			{ return *m_actions[from][to]; }

	private:
		const Piece& m_piece;                                  /**< Piece performing the action. */

		matrix<Action *, NumSquares, NumSquares> m_actions;    /**< Index for fast access in list. */
};

/* -------------------------------------------------------------------------- */

}

#endif
