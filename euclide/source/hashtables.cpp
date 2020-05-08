#include "hashtables.h"
#include "problem.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */

HashPosition::HashPosition(const Problem& problem)
{
	for (Square square : AllSquares())
		(*this)[square] = problem.initialPosition()[square];
}

/* -------------------------------------------------------------------------- */

void HashPosition::set(Square square, Glyph glyph)
{
	static_assert(NumGlyphs <= 16);
	assert(glyph <= 15);

	if (square & 1)
		m_glyphs[square >> 1] = (glyph << 4) | (m_glyphs[square >> 1] & 0x0F);
	else
		m_glyphs[square >> 1] = (glyph << 0) | (m_glyphs[square >> 1] & 0xF0);
}

/* -------------------------------------------------------------------------- */

void HashPosition::set(Square square, const array<bool, NumCastlingSides>& castlings)
{
	static_assert(NumGlyphs <= 13);
	static_assert(NumCastlingSides == 2);

	const int value = (castlings[KingSideCastling] ? -1 : 0) + (castlings[QueenSideCastling] ? -2 : 0);

	if (square == Castlings[White][KingSideCastling].from)
		set(square, value ? Glyph(value & 0x0F) : WhiteKing);
	else
	if (square == Castlings[Black][KingSideCastling].from)
		set(square, value ? Glyph(value & 0x0F) : BlackKing);
	else
		assert(xstd::all(castlings, false));
}

/* -------------------------------------------------------------------------- */

uint32_t HashPosition::hash() const
{
	static const bool wide = (sizeof(void *) >= sizeof(uint64_t));

	if (wide)
	{
		uint64_t hash = UINT64_C(0xCBF29CE484222325);
		for (unsigned k = 0; k < sizeof(m_glyphs) / sizeof(uint64_t); k++)
		{
			hash ^= reinterpret_cast<const uint64_t *>(m_glyphs.data())[k];
			hash *= UINT64_C(0x00000100000001B3);
		}

		return (hash >> 32) & hash & UINT32_MAX;
	}
	else
	{
		uint32_t hash = UINT32_C(0x811C9DC5);
		for (unsigned k = 0; k < sizeof(m_glyphs) / sizeof(uint32_t); k++)
		{
			hash ^= reinterpret_cast<const uint32_t *>(m_glyphs.data())[k];
			hash *= UINT32_C(0x01000193);
		}

		return hash;
	}
}

/* -------------------------------------------------------------------------- */

HashTable::HashTable(int size)
{
	assert(intel::popcnt(uint32_t(size)) == 1);
	m_mask = size - 1;

	m_chaining = 16;

	/* -- Create empty hash table -- */

	m_entries.resize(size + m_chaining - 1);
	for (HashEntry& entry : m_entries)
		entry.hash = std::numeric_limits<uint32_t>::max();
}

/* -------------------------------------------------------------------------- */

void HashTable::insert(const HashPosition& position, int moves)
{
	uint32_t hash = position.hash() ^ moves;
	uint32_t index = hash & m_mask;

	for (int k = 0; k < m_chaining; k++, index++)
		if (m_entries[index].hash == std::numeric_limits<uint32_t>::max())
			break;

	m_entries[index] = { position, moves, hash };
}

/* -------------------------------------------------------------------------- */

bool HashTable::contains(const HashPosition& position, int moves)
{
	uint32_t hash = position.hash() ^ moves;
	uint32_t index = hash & m_mask;

	for (int k = 0; k < m_chaining; k++, index++)
		if (m_entries[index].hash == hash)
			if (m_entries[index].moves == moves)
				if (m_entries[index].position == position)
					return true;

	return false;
}

/* -------------------------------------------------------------------------- */

}
