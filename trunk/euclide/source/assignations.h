#ifndef __EUCLIDE_ASSIGNATIONS_H
#define __EUCLIDE_ASSIGNATIONS_H

#include "includes.h"

namespace euclide 
{

class Implications;

/* -------------------------------------------------------------------------- */

/**
 * \class Assignation
 * Moves or captures assigned to a group of pieces.
 */

/**
 * \class Assignations
 * List of assignations.
 */

/* -------------------------------------------------------------------------- */

class Assignation
{
	public :
		Assignation();
		Assignation(Man man, Color color, int assigned);
		Assignation(Men men, Color color, int assigned);

		void merge(const Assignation& assignation);
		void minimum(int minimum);

	public :
		inline Man man() const
			{ for (Man man = FirstMan; man <= LastMan; man++) if (_men[man]) return man;  return UndefinedMan; }
		inline const Men& men() const
			{ return _men; }
		inline Color color() const
			{ return _color; }
		inline int assigned() const
			{ return _assigned; }
		inline int minimum() const
			{ return _minimum; }

	public :
		inline bool isMen(const Men& men) const
			{ return _men == men; }

	private :
		Men _men;         /**< Group of pieces to which moves or captures are assigned. */
		Color _color;     /**< Color of the piece group. */
		int _assigned;    /**< Number of assigned moves or captures. */
		int _minimum;     /**< Minimum number of moves for assignation. */
};

/* -------------------------------------------------------------------------- */

class Assignations : public std::list<Assignation>
{
	public :
		void cleanup(const Implications& implications, int (Implications::*assigned)(Man) const);
};

/* -------------------------------------------------------------------------- */

}

#endif
