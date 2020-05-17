#ifndef __EUCLIDE_CONDITIONS_H
#define __EUCLIDE_CONDITIONS_H

#include "includes.h"
#include "hashtables.h"

namespace Euclide
{

class Piece;
class Problem;

/* -------------------------------------------------------------------------- */
/* -- Condition                                                            -- */
/* -------------------------------------------------------------------------- */

class Condition
{
	public :
		Condition() {}
		virtual ~Condition() {}

		virtual bool satisfied() const = 0;
};

/* -------------------------------------------------------------------------- */

class PositionalCondition : public Condition
{
	public :
		PositionalCondition(const Piece& piece, const Squares& squares);

		virtual bool satisfied() const override;

	private:
		const Piece& m_piece;    /**< Piece concerned by condition. */
		Squares m_squares;       /**< Squares on which the piece may lie. */
};

/* -------------------------------------------------------------------------- */
/* -- Conditions                                                           -- */
/* -------------------------------------------------------------------------- */

class Conditions : public Condition
{
	public :
		Conditions(const Piece& piece, Square from, Square to);
		virtual ~Conditions();

		virtual bool satisfied() const override;

		void add(Condition *condition);

	public :
		inline const Piece& piece() const
			{ return m_piece; }

		inline Square from() const
			{ return m_from; }
		inline Square to() const
			{ return m_to; }

	private :
		const Piece& m_piece;                   /**< Piece to which these conditions apply. */
		Square m_from;                          /**< Move departure square. */
		Square m_to;                            /**< Move arrival square. */

		std::list<Condition *> m_conditions;    /**< Conditions that must be fullfilled before playing a move. */
};

/* -------------------------------------------------------------------------- */
/* -- AllConditions                                                        -- */
/* -------------------------------------------------------------------------- */

class PieceConditions
{
	public :
		PieceConditions(const Piece& piece);

		inline std::vector<Conditions>::iterator begin()
			{ return m_conditions.begin(); }
		inline std::vector<Conditions>::iterator end()
			{ return m_conditions.end(); }
		inline std::vector<Conditions>::const_iterator begin() const
			{ return m_conditions.begin(); }
		inline std::vector<Conditions>::const_iterator end() const
			{ return m_conditions.end(); }

	public :
		inline const Conditions& get(Square from, Square to) const
			{ return *m_pointers[from][to]; }
		inline Conditions& get(Square from, Square to)
			{ return *m_pointers[from][to]; }

	private :
		const Piece& m_piece;                                       /**< Piece to which these conditions apply. */

		std::vector<Conditions> m_conditions;                       /**< Conditions, one for each piece move. */
		matrix<Conditions *, NumSquares, NumSquares> m_pointers;    /**< Conditions, sorted by move start and destination squares. */
};

/* -------------------------------------------------------------------------- */

}

#endif
