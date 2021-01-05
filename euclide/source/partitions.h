#ifndef __EUCLIDE_PARTITIONS_H
#define __EUCLIDE_PARTITIONS_H

#include "includes.h"

namespace Euclide
{

class Pieces;
class Target;
class Targets;
class Capture;
class Captures;

/* -------------------------------------------------------------------------- */
/* -- Partition                                                            -- */
/* -------------------------------------------------------------------------- */

class Partition
{
	public:
		Partition();

		bool merge(const Target& target);
		bool merge(const Capture& capture);
		void assign(const Pieces& pieces);

		bool split(Pieces& pieces, int freeMoves, int freeCaptures, Targets& targets, Captures& captures) const;
		bool split(Pieces& pieces, Glyph glyph, int availableMoves, int availableCaptures, Targets& targets, Captures& captures) const;

	public:
		inline Squares squares() const
			{ return m_squares; }
		inline Squares captures() const
			{ return m_captures; }
		inline Glyphs glyphs() const
			{ return m_glyphs; }
		inline Men men() const
			{ return m_men; }

		inline int requiredMoves() const
			{ return m_requiredMoves; }
		inline int requiredCaptures() const
			{ return m_requiredCaptures; }

		inline int assignedRequiredMoves() const
			{ return m_assignedRequiredMoves; }
		inline int assignedRequiredCaptures() const
			{ return m_assignedRequiredCaptures; }

		inline int unassignedRequiredMoves() const
			{ return m_unassignedRequiredMoves; }
		inline int unassignedRequiredCaptures() const
			{ return m_unassignedRequiredCaptures; }

	private:
		Squares m_squares;                          /**< Target squares. */
		Squares m_captures;                         /**< Capture squares. */
		Glyphs m_glyphs;                            /**< Possible glyphs. */
		Men m_men;                                  /**< Partition men. */

		struct Destination {
			Squares squares;
			Square square;
			Glyphs glyphs;
			Glyph glyph;
			Men men;

			const Target *target;
			const Capture *capture;
		};

		std::vector<Destination> m_destinations;    /**< Target and capture locations. */

		int m_requiredMoves;                        /**< Required moves. */
		int m_requiredCaptures;                     /**< Required captures. */

		int m_assignedRequiredMoves;                /**< Required moves assigned to a specific piece. */
		int m_assignedRequiredCaptures;             /**< Required captures assigned to a specific piece. */

		int m_unassignedRequiredMoves;              /**< Required moves unassigned to a specific piece. */
		int m_unassignedRequiredCaptures;           /**< Required captures unassigned to a specific piece. */
};

/* -------------------------------------------------------------------------- */
/* -- Partitions                                                           -- */
/* -------------------------------------------------------------------------- */

class Partitions : public std::vector<Partition>
{
	public:
		Partitions(const Pieces& pieces, const Targets& targets, const Captures& captures);

	public:
		inline int requiredMoves() const
			{ return m_requiredMoves; }
		inline int requiredCaptures() const
			{ return m_requiredCaptures; }

		inline int unassignedRequiredMoves() const
			{ return m_unassignedRequiredMoves; }
		inline int unassignedRequiredCaptures() const
			{ return m_unassignedRequiredCaptures; }

		inline int requiredMoves(Man man) const
			{ return m_map[man]->requiredMoves(); }
		inline int requiredCaptures(Man man) const
			{ return m_map[man]->requiredCaptures(); }

		inline int unassignedRequiredMoves(Man man) const
			{ return m_map[man]->unassignedRequiredMoves(); }
		inline int unassignedRequiredCaptures(Man man) const
			{ return m_map[man]->unassignedRequiredCaptures(); }

	private:
		int m_requiredMoves;                          /**< Required moves. */
		int m_requiredCaptures;                       /**< Required capture. */

		int m_unassignedRequiredMoves;                /**< Required moves unassigned to a specific piece. */
		int m_unassignedRequiredCaptures;             /**< Required captures unassigned to a specific piece. */

		array<const Partition *, MaxPieces> m_map;    /**< Quick map between men and partitions. */
		Partition m_null;                             /**< Null partition, used in above map. */
};

/* -------------------------------------------------------------------------- */

}

#endif
