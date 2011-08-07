#ifndef __EUCLIDE_IMPLICATIONS_H
#define __EUCLIDE_IMPLICATIONS_H

#include "includes.h"
#include "position.h"

namespace euclide 
{

class Assignations;

/* -------------------------------------------------------------------------- */

/**
 * \class Implication
 * Various informations implied by the current position, for a given man and color.
 */

/**
 * \class Implications
 * Various informations implied by the current position, for a given color.
 */

/* -------------------------------------------------------------------------- */

class Implication
{
	public :
		Implication();
		~Implication();

		void clear();
		void add(int requiredMoves, int unassignedMoves, int requiredCaptures, int unassignedCaptures, Square square, Superman superman, bool captured);
		void set(int requiredMoves, int availableMoves, int requiredCaptures, int availableCaptures);
		void sub(int moves, int captures);

	public :
		inline int assignedMoves() const
			{ return _assignedMoves; }
		inline int assignedCaptures() const
			{ return _assignedCaptures; }
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
		int _assignedMoves;         /**< Number of moves assigned to this piece, i.e. that must have been played by this piece. */
		int _availableMoves;        /**< Number of available moves for current piece. */
		int _unassignedMoves;       /**< Number of unassigned moves available for current piece. */
		int _assignedCaptures;      /**< Number of captures assigned to this piece, i.e. that lust have been performed by this piece. */
		int _availableCaptures;     /**< Number of available captures for current piece. */
		int _unassignedCaptures;    /**< Number of unassigned captures available for current piece. */

		Squares _squares;           /**< Possible destination squares of current piece. */
		Supermen _supermen;         /**< Possible supermen for current piece. */

		bool _captured;             /**< True if piece may have been captured. */
		bool _alive;                /**< True if piece may still be on the board. */
};

/* -------------------------------------------------------------------------- */

class Implications
{
	public :
		Implications();
		Implications(const Position& position);
		Implications(const Position& position, const Assignations& assignedMoves, const Assignations& assignedCaptures);
		~Implications();

	protected :
		void update(const Position& position, bool clear = true);
		void update(const Assignations& assignedMoves, const Assignations& assignedCaptures);

	public :
		inline int assignedMoves(Man man) const
			{ return _implications[man].assignedMoves(); }
		inline int assignedCaptures(Man man) const
			{ return _implications[man].assignedCaptures(); }
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

	public :
		int assignedMoves(Man man, Man xman) const;
		int assignedCaptures(Man man, Man xman) const;
		int availableMoves(Man man, Man xman) const;
		int availableCaptures(Man man, Man xman) const;
		int unassignedMoves(Man man, Man xman) const;
		int unassignedCaptures(Man man, Man xman) const;

	public :
		int assignedMoves(Men men) const;
		int assignedCaptures(Men men) const;
		int availableMoves(Men men) const;
		int availableCaptures(Men men) const;
		int unassignedMoves(Men men) const;
		int unassignedCaptures(Men men) const;

		Squares squares(Men men) const;

	private :
		array<Implication, NumMen> _implications;    /**< Implications, for each piece. */

		const Assignations *_assignedMoves;          /**< Assigned moves. */
		const Assignations *_assignedCaptures;       /**< Assigned captures. */
};

/* -------------------------------------------------------------------------- */

}

#endif
