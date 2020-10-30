#ifndef __EUCLIDE_BITSET_H
#define __EUCLIDE_BITSET_H

#include "../includes.h"
#include "intrinsics.h"
#include "iterator.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */

template <typename Type, int Bits>
class BitSet
{
	private:
		typedef typename std::conditional<Bits <= 32, uint32_t, uint64_t>::type bits_t;
		bits_t m_bits;

		static const constexpr bits_t None = 0;
		static const constexpr bits_t All = ~None >> (std::numeric_limits<bits_t>::digits - Bits);

	public:
		constexpr BitSet(bits_t bits = None) : m_bits(bits) {}
		constexpr BitSet(Type position) : m_bits(bits_t(1) << position) {}

		template <typename Predicate> inline
		BitSet(const Predicate& predicate, bool value = true) : m_bits(0)
			{ for (Type position : EnumRange<Type, Bits>()) if (predicate(position)) set(position, value); }

		template <typename Predicate, typename Range> inline
		BitSet(const Predicate& predicate, const Range& range, bool value = true) : m_bits(0)
			{ for (Type position : range) if (predicate(position)) set(position, value); }

		inline BitSet& set()
			{ m_bits = All; return *this; }
		inline BitSet& reset()
			{ m_bits = None; return *this; }
		inline BitSet& flip()
			{ m_bits = ~m_bits & All; return *this; }

		inline BitSet& set(Type position, bool value = true)
			{ assert((position >= 0) && (position < Bits)); m_bits = value ? intel::bts(m_bits, position) : intel::btr(m_bits, position); return *this; }
		inline BitSet& reset(Type position)
			{ assert((position >= 0) && (position < Bits)); m_bits = intel::btr(m_bits, position); return *this; }
		inline BitSet& flip(Type position)
			{ assert((position >= 0) && (position < Bits)); m_bits = intel::btc(m_bits, position); return *this; }

		template <typename Predicate> inline
		BitSet& set(const Predicate& predicate, bool value = true)
			{ for (Type position : EnumRange<Type, Bits>()) if (predicate(position)) set(position, value); return *this; }
		template <typename Predicate> inline
		BitSet& reset(const Predicate& predicate)
			{ for (Type position : EnumRange<Type, Bits>()) if (predicate(position)) reset(position); return *this; }
		template <typename Predicate> inline
		BitSet& flip(const Predicate& predicate)
			{ for (Type position : EnumRange<Type, Bits>()) if (predicate(position)) flip(position); return *this; }

		template <typename Predicate, typename Range> inline
		BitSet& set(const Predicate& predicate, const Range& range, bool value = true)
			{ for (Type position : range) if (predicate(position)) set(position, value); return *this; }
		template <typename Predicate, typename Range> inline
		BitSet& reset(const Predicate& predicate, const Range& range)
			{ for (Type position : range) if (predicate(position)) reset(position); return *this; }
		template <typename Predicate, typename Range> inline
		BitSet& flip(const Predicate& predicate, const Range& range)
			{ for (Type position : range) if (predicate(position)) flip(position); return *this; }

	public:
		inline constexpr bool test(Type position) const
			{ return assert((position >= 0) && (position < Bits)), intel::bt(m_bits, position); }

		inline constexpr bool all() const
			{ return m_bits == All; }
		inline constexpr bool any() const
			{ return m_bits != None; }
		inline constexpr bool none() const
			{ return m_bits == None; }

		inline constexpr operator bits_t() const
			{ return m_bits; }
		inline constexpr bool operator!() const
			{ return none(); }

		inline constexpr int count() const
			{ return intel::popcnt(m_bits); }
		inline constexpr int size() const
			{ return Bits; }

	public:
		inline Type first() const
			{ int bit; return static_cast<Type>(intel::bsf(m_bits, &bit) ? bit : Bits); }
		inline Type next(int position) const
			{ int bit; return static_cast<Type>(intel::bsf(m_bits >> position >> 1, &bit) ? bit + position + 1 : Bits); }

		inline Type pop()
			{ const Type position = first(); assert(m_bits); reset(position); return position; }

	public:
		inline BitSet& operator&=(const BitSet& bitset)
			{ m_bits &= bitset.m_bits; return *this; }
		inline BitSet& operator|=(const BitSet& bitset)
			{ m_bits |= bitset.m_bits; return *this; }
		inline BitSet& operator^=(const BitSet& bitset)
			{ m_bits ^= bitset.m_bits; return *this; }
		inline BitSet& operator+=(const BitSet& bitset)
			{ m_bits |= bitset.m_bits; return *this; }
		inline BitSet& operator-=(const BitSet& bitset)
			{ m_bits &= ~bitset.m_bits; return *this; }

		inline BitSet operator&(const BitSet& bitset) const
			{ return BitSet(*this) &= bitset; }
		inline BitSet operator|(const BitSet& bitset) const
			{ return BitSet(*this) |= bitset; }
		inline BitSet operator^(const BitSet& bitset) const
			{ return BitSet(*this) ^= bitset; }
		inline BitSet operator+(const BitSet& bitset) const
			{ return BitSet(*this) += bitset; }
		inline BitSet operator-(const BitSet& bitset) const
			{ return BitSet(*this) -= bitset; }

		inline BitSet operator&(Type position) const
			{ return BitSet(*this) &= BitSet(position); }
		inline BitSet operator|(Type position) const
			{ return BitSet(*this) |= BitSet(position); }
		inline BitSet operator^(Type position) const
			{ return BitSet(*this) ^= BitSet(position); }
		inline BitSet operator+(Type position) const
			{ return BitSet(*this) += BitSet(position); }
		inline BitSet operator-(Type position) const
			{ return BitSet(*this) -= BitSet(position); }

		inline bool operator==(const BitSet& bitset) const
			{ return m_bits == bitset.m_bits; }
		inline bool operator!=(const BitSet& bitset) const
			{ return m_bits != bitset.m_bits; }
		inline bool operator<=(const BitSet& bitset) const
			{ return (m_bits & ~bitset.m_bits) == 0; }
		inline bool operator>=(const BitSet& bitset) const
			{ return (~m_bits & bitset.m_bits) == 0; }
		inline bool operator<(const BitSet& bitset) const
			{ return (*this <= bitset) && (*this != bitset); }
		inline bool operator>(const BitSet& bitset) const
			{ return (*this >= bitset) && (*this != bitset); }

		inline BitSet operator~() const
			{ return BitSet(*this).flip(); }

	public:
		static constexpr BitSet mask(int n)
			{ return BitSet((n < Bits) ? (bits_t(1) << n) - 1 : All); }

	public:
		class BitReference
		{
			public:
				BitReference(BitSet& bitset, Type position) : m_bitset(bitset), m_position(position) {}

				inline BitReference& operator=(bool value)
					{ m_bitset.set(m_position, value); return *this; }

				inline operator bool() const
					{ return m_bitset.test(m_position); }

			private:
				BitSet& m_bitset;
				Type m_position;
		};

		inline constexpr bool operator[](Type position) const
			{ return assert((position >= 0) && (position < Bits)), test(position); }
		inline BitReference operator[](Type position)
			{ assert((position >= 0) && (position < Bits)); return BitReference(*this, position); }

	public:
		class BitSetRange
		{
			public:
				typedef Type value_type;

			public:
				class BitSetIterator : public std::iterator<std::forward_iterator_tag, Type>
				{
					public:
						BitSetIterator() : m_bits(0), m_position(Bits) {}
						BitSetIterator(const BitSet& bitset) : m_bits(bitset) { operator++(); }

					public:
						inline bool operator==(const BitSetIterator& iterator) const
							{ return m_position == iterator.m_position; }
						inline bool operator!=(const BitSetIterator& iterator) const
							{ return m_position != iterator.m_position; }

						inline BitSetIterator& operator++()
							{ intel::bsf(m_bits, &m_position) ? (m_bits = intel::btr(m_bits, m_position)) : (m_position = Bits); return *this; }

						inline Type operator*() const
							{ assert(m_position < Bits); return static_cast<Type>(m_position); }

					private:
						bits_t m_bits;
						int m_position;
				};

				typedef BitSetIterator iterator;

			public:
				BitSetRange(const BitSet& bitset, bool complement = false) : m_bitset(complement ? ~bitset : bitset) {}

				inline BitSetIterator begin() const
					{ return BitSetIterator(m_bitset); }
				inline BitSetIterator end() const
					{ return BitSetIterator(); }

			private:
				const BitSet m_bitset;
		};

		inline BitSetRange range(bool complement = false) const
			{ return BitSetRange(*this, complement); }

	public:
		template <typename Collection>
		class BitSetSelection
		{
			public:
				typedef typename Collection::value_type value_type;

			public:
				class BitSetIterator : public std::iterator<std::forward_iterator_tag, Type>
				{
					public:
						BitSetIterator(const Collection& collection) : m_collection(&collection), m_bits(0), m_position(Bits) {}
						BitSetIterator(const Collection& collection, const BitSet& bitset) : m_collection(&collection), m_bits(bitset) { operator++(); }

					public:
						inline bool operator==(const BitSetIterator& iterator) const
							{ return m_position == iterator.m_position; }
						inline bool operator!=(const BitSetIterator& iterator) const
							{ return m_position != iterator.m_position; }

						inline BitSetIterator& operator++()
							{ intel::bsf(m_bits, &m_position) ? (m_bits = intel::btr(m_bits, m_position)) : (m_position = Bits); return *this; }

						inline typename Collection::const_reference operator*() const
							{ return (*m_collection)[m_position]; }

					private:
						const Collection *m_collection;
						bits_t m_bits;
						int m_position;
				};

				typedef BitSetIterator iterator;

			public:
				BitSetSelection(const Collection& collection, const BitSet& bitset) : m_collection(collection), m_bitset(bitset) {}

				inline BitSetIterator begin() const
					{ return BitSetIterator(m_collection, m_bitset); }
				inline BitSetIterator end() const
					{ return BitSetIterator(m_collection); }

			private:
				const Collection& m_collection;
				const BitSet m_bitset;
		};

		template <typename Collection>
		inline BitSetSelection<Collection> in(const Collection& collection) const
			{ return BitSetSelection<Collection>(collection, *this); }
};

/* -------------------------------------------------------------------------- */

}

#endif
