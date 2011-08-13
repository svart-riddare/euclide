#ifndef __EUCLIDE_IMPLICATIONS_H
#define __EUCLIDE_IMPLICATIONS_H

#include "includes.h"
#include "assignations.h"

namespace euclide 
{

class Position;

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

	public :
		void add(int requiredMoves, int requiredCaptures);
		void add(int requiredMoves, int requiredCaptures, Square square, Superman superman, bool captured);
		void set(int extraMoves, int extraCaptures);

	public :
		inline int assignedMoves() const
			{ return _assignedMoves; }
		inline int assignedCaptures() const
			{ return _assignedCaptures; }
		inline int requiredMoves() const
			{ return _requiredMoves; }
		inline int requiredCaptures() const
			{ return _requiredCaptures; }
		inline int availableMoves() const
			{ return _availableMoves; }
		inline int availableCaptures() const
			{ return _availableCaptures; }

		inline const Squares& squares() const
			{ return _squares; }
		inline const Supermen& supermen() const
			{ return _supermen; }

		inline bool captured() const
			{ return _captured; }
		inline bool alive() const
			{ return _alive; }

	private :
		int _assignedMoves;         /**< Minimum number of moves assigned to current piece. */
		int _requiredMoves;         /**< Minimum number of moves required for current piece. */
		int _availableMoves;        /**< Number of available moves for current piece. */
		int _assignedCaptures;      /**< Minimum number of captures assigned to current piece. */
		int _requiredCaptures;      /**< Minimum numner of captures required for current piece. */
		int _availableCaptures;     /**< Number of available captures for current piece. */

		Squares _squares;           /**< Possible destination squares of current piece. */
		Supermen _supermen;         /**< Possible supermen for current piece. */

		bool _captured;             /**< True if piece may have been captured. */
		bool _alive;                /**< True if piece may still be on the board. */
};

/* -------------------------------------------------------------------------- */

class Implications
{
	public :
		Implications(const Position& position);
		Implications(const Position& position, const Assignations& assignedMoves, const Assignations& assignedCaptures);
		~Implications();

	protected :
		void constructor(const Position& position, const Assignations *assignedMoves = NULL, const Assignations *assignedCaptures = NULL);

	public :
		inline int assignedMoves(Man man) const
			{ return _implications[man].assignedMoves(); }
		inline int assignedCaptures(Man man) const
			{ return _implications[man].assignedCaptures(); }
		inline int requiredMoves(Man man) const
			{ return _implications[man].requiredMoves(); }
		inline int requiredCaptures(Man man) const
			{ return _implications[man].requiredCaptures(); }
		inline int availableMoves(Man man) const
			{ return _implications[man].availableMoves(); }
		inline int availableCaptures(Man man) const
			{ return _implications[man].availableCaptures(); }

		inline const Squares& squares(Man man) const
			{ return _implications[man].squares(); }
		inline const Supermen& supermen(Man man) const
			{ return _implications[man].supermen(); }

		inline bool captured(Man man) const
			{ return _implications[man].captured(); }
		inline bool alive(Man man) const
			{ return _implications[man].alive(); }

	public :
		int requiredMoves(Man manA, Man manB) const;
		int requiredCaptures(Man manA, Man manB) const;
		int availableMoves(Man manA, Man manB) const;
		int availableCaptures(Man manA, Man manB) const;

	public :
		int assignedMoves(const Men& men) const;
		int assignedCaptures(const Men& men) const;

	public :
		Squares squares(const Men& men) const;

	private :
		array<Implication, NumMen> _implications;    /**< Implications, for each piece. */
		
		vector<Assignation> _movePartitions;         /**< Men partitions, represented as move assignations. */
		vector<Assignation> _capturePartitions;      /**< Men partitions, represented as capture assignations. */

		Assignations _assignedMoves;                 /**< Assigned moves. */
		Assignations _assignedCaptures;              /**< Assigned captures. */

		int _availableMoves;                         /**< Position total available moves. */
		int _availableCaptures;                      /**< Position total available captures. */

		int _requiredMoves;                          /**< Position minimum number or required moves. */
		int _requiredCaptures;                       /**< Position minimum number of required captures. */
};

/* -------------------------------------------------------------------------- */

}

#endif
