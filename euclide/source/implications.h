#ifndef __EUCLIDE_IMPLICATIONS_H
#define __EUCLIDE_IMPLICATIONS_H

#include "includes.h"
#include "position.h"

namespace euclide 
{

/* -------------------------------------------------------------------------- */

/**
 * \class Implication
 * Various informations implied by the current position, for a given man and color.
 */

/* -------------------------------------------------------------------------- */

class Implication
{
	public :
		Implication();
		~Implication();

		void add(int requiredMoves, int unassignedMoves, int requiredCaptures, int unassignedCaptures, Square square, Superman superman, bool captured);

	public :
		inline int availableMoves() const
			{ return _availableMoves; }
		inline int availableCaptures() const
			{ return _availableCaptures; }
		inline int unassignedMoves() const
			{ return _unassignedMoves; }
		inline int unassignedCaptures() const
			{ return _unassignedCaptures; }

		inline const Squares& squares() const
			{ return _squares; }
		inline const Supermen& supermen() const
			{ return _supermen; }

		inline bool captured() const
			{ return _captured; }
		inline bool alive() const
			{ return _alive; }

	private :
		int _availableMoves;
		int _unassignedMoves;
		int _availableCaptures;
		int _unassignedCaptures;

		Squares _squares;
		Supermen _supermen;

		bool _captured;
		bool _alive;
};

/* -------------------------------------------------------------------------- */

class Implications
{
	public :
		Implications(const Position& position);
		~Implications();

	public :
		inline int availableMoves(Man man) const
			{ return _implications[man].availableMoves(); }
		inline int availableCaptures(Man man) const
			{ return _implications[man].availableCaptures(); }
		inline int unassignedMoves(Man man) const
			{ return _implications[man].unassignedMoves(); }
		inline int unassignedCaptures(Man man) const
			{ return _implications[man].unassignedCaptures(); }

		inline const Squares& squares(Man man) const
			{ return _implications[man].squares(); }
		inline const Supermen& supermen(Man man) const
			{ return _implications[man].supermen(); }

		inline bool captured(Man man) const
			{ return _implications[man].captured(); }
		inline bool alive(Man man) const
			{ return _implications[man].alive(); }

	private :
		array<Implication, NumMen> _implications;
};

/* -------------------------------------------------------------------------- */

}

#endif
