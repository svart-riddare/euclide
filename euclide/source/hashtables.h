#ifndef __EUCLIDE_HASH_TABLES_H
#define __EUCLIDE_HASH_TABLES_H

#include "includes.h"

namespace Euclide
{

class Problem;

/* -------------------------------------------------------------------------- */

class HashPosition
{
	public :
		HashPosition() {}
		HashPosition(const Problem& problem);

		class Assignment
		{
			public :
				Assignment(HashPosition& position, Square square) : m_position(position), m_square(square) {}
				void operator=(Glyph glyph)
					{ m_position.set(m_square, glyph); }

			private :
				HashPosition& m_position;
				Square m_square;
		};

		inline Assignment operator[](Square square)
			{ return Assignment(*this, square); }

	public :
		inline bool operator==(const HashPosition& position) const
			{ return m_glyphs == position.m_glyphs; }
		inline bool operator!=(const HashPosition& position) const
			{ return m_glyphs != position.m_glyphs; }

		uint32_t hash() const;

	protected :
		void set(Square square, Glyph glyph);

	private :
		array<uint8_t, NumSquares / 2> m_glyphs;    /**< Glyphs, four bits each. Castling rights are encoded with the rooks. */
};

/* -------------------------------------------------------------------------- */

class HashTable
{
	public :
		HashTable(int size);

		void insert(const HashPosition& position, int moves);
		bool contains(const HashPosition& position, int moves);

	private :
		struct HashEntry { HashPosition position; int32_t moves; uint32_t hash; };
		std::vector<HashEntry> m_entries;      /**< Hash table of positions and associated data. */

		uint32_t m_mask;                       /**< Mask for hash function. */
		int m_chaining;                        /**< Maximum number of entries to check for a given hash index. */
};

/* -------------------------------------------------------------------------- */

}

#endif
