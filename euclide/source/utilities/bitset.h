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
	private :
		typedef typename std::conditional<Bits <= 32, uint32_t, uint64_t>::type bits_t;
		bits_t _bits;

	public :
		BitSet(bits_t bits = 0) : _bits(bits) {}

		template <typename Predicate> inline
		BitSet(const Predicate& predicate, bool value = true) : _bits(0)
			{ for (Type position : EnumRange<Type, Bits>()) if (predicate(position)) set(position, value); }

		inline BitSet& set()
			{ _bits = ~bits_t(0); return *this; }
		inline BitSet& reset()
			{ _bits = bits_t(0); return *this; }
		inline BitSet& flip()
			{ _bits = ~_bits; return *this; }

		inline BitSet& set(Type position, bool value = true)
			{ assert((position >= 0) && (position < Bits)); _bits = value ? intel::bts(_bits, position) : intel::btr(_bits, position); return *this; }
		inline BitSet& reset(Type position)
			{ assert((position >= 0) && (position < Bits)); _bits = intel::btr(_bits, position); return *this; }
		inline BitSet& flip(Type position)
			{ assert((position >= 0) && (position < Bits)); _bits = intel::btc(_bits, position); return *this; }

		template <typename Predicate> inline
		BitSet& set(const Predicate& predicate, bool value = true)
			{ for (Type position : EnumRange<Type, Bits>()) if (predicate(position)) set(position, value); return *this; }
		template <typename Predicate> inline
		BitSet& reset(const Predicate& predicate)
			{ for (Type position : EnumRange<Type, Bits>()) if (predicate(position)) reset(position); return *this; }
		template <typename Predicate> inline
		BitSet& flip(const Predicate& predicate)
			{ for (Type position : EnumRange<Type, Bits>()) if (predicate(position)) flip(position); return *this; }

	public :
		inline bool test(Type position) const
			{ assert((position >= 0) && (position < Bits)); return intel::bt(_bits, position); }

		inline bool all() const
			{ return ~_bits == 0; }
		inline bool any() const
			{ return _bits != 0; }
		inline bool none() const
			{ return _bits == 0; }

		inline operator bits_t() const
			{ return _bits; }
		inline bool operator!() const
			{ return none(); }

		inline int count() const
			{ return intel::popcnt(_bits); }
		inline int size() const
			{ return Bits; }

	public :
		inline Type first() const
			{ int bit; return static_cast<Type>(intel::bsf(_bits, &bit) ? bit : Bits); }
		inline Type next(int position) const
			{ int bit; return static_cast<Type>(intel::bsf(_bits >> position >> 1, &bit) ? bit + position + 1 : Bits); }

	public :
		inline BitSet& operator&=(const BitSet& bitset)
			{ _bits &= bitset._bits; return *this; }
		inline BitSet& operator|=(const BitSet& bitset)
			{ _bits |= bitset._bits; return *this; }
		inline BitSet& operator^=(const BitSet& bitset)
			{ _bits ^= bitset._bits; return *this; }
		inline BitSet& operator+=(const BitSet& bitset)
			{ _bits |= bitset._bits; return *this; }
		inline BitSet& operator-=(const BitSet& bitset)
			{ _bits &= ~bitset._bits; return *this; }

		inline BitSet operator&(const BitSet& bitset) const
			{ return BitSet(*this) &= bitset; }
		inline BitSet operator|(const BitSet& bitset) const
			{ return BitSet(*this) |= bitset; }
		inline BitSet operator^=(const BitSet& bitset) const
			{ return BitSet(*this) ^= bitset; }
		inline BitSet operator+(const BitSet& bitset) const
			{ return BitSet(*this) += bitset; }
		inline BitSet operator-(const BitSet& bitset) const
			{ return BitSet(*this) -= bitset; }

		inline bool operator==(const BitSet& bitset) const
			{ return _bits == bitset._bits; }
		inline bool operator!=(const BitSet& bitset) const
			{ return _bits != bitset._bits; }
		inline bool operator<=(const BitSet& bitset) const
			{ return (_bits & ~bitset._bits) == 0; }
		inline bool operator>=(const BitSet& bitset) const
			{ return (~_bits & bitset._bits) == 0; }
		inline bool operator<(const BitSet& bitset) const
			{ return (*this <= bitset) && (*this != bitset); }
		inline bool operator>(const BitSet& bitset) const
			{ return (*this >= bitset) && (*this != bitset); }

		inline BitSet operator~() const
			{ return BitSet(*this).flip(); }

	public :
		static BitSet mask(int n)
			{ return BitSet((n < Bits) ? (bits_t(1) << n) - 1 : std::numeric_limits<bits_t>::max()); }

	public :
		class BitReference
		{
			public :
				BitReference(BitSet& bitset, Type position) : _bitset(bitset), _position(position) {}

				inline BitReference& operator=(bool value)
					{ _bitset.set(_position, value); return *this; }

				inline operator bool() const
					{ return _bitset.test(_position); }

			private :
				BitSet& _bitset;
				Type _position;
		};

		inline bool operator[](Type position) const
			{ assert((position >= 0) && (position < Bits)); return test(position); }
		inline BitReference operator[](Type position)
			{ assert((position >= 0) && (position < Bits)); return BitReference(*this, position); }

	public :
		class BitSetRange
		{
			public :
				typedef Type value_type;

			public :
				class BitSetIterator : public std::iterator<std::forward_iterator_tag, Type>
				{
					public :
						BitSetIterator() : _bits(0), _position(Bits) {}
						BitSetIterator(const BitSet& bitset) : _bits(bitset) { operator++(); }

					public :
						inline bool operator==(const BitSetIterator& iterator) const
							{ return _position == iterator._position; }
						inline bool operator!=(const BitSetIterator& iterator) const
							{ return _position != iterator._position; }

						inline BitSetIterator& operator++()
							{ intel::bsf(_bits, &_position) ? (_bits = intel::btr(_bits, _position)) : (_position = Bits); return *this; }

						inline Type operator*() const
							{ assert(_position < Bits); return static_cast<Type>(_position); }

					private :
						bits_t _bits;
						int _position;
				};

				typedef BitSetIterator iterator;

			public :
				BitSetRange(const BitSet& bitset) : _bitset(bitset) {}

				inline BitSetIterator begin() const
					{ return BitSetIterator(_bitset); }
				inline BitSetIterator end() const
					{ return BitSetIterator(); }

			private :
				const BitSet& _bitset;
		};

		inline BitSetRange range() const
			{ return BitSetRange(*this); }

	public :
		template <typename Collection>
		class BitSetSelection
		{
			public :
				typedef typename Collection::value_type value_type;

			public :
				class BitSetIterator : public std::iterator<std::forward_iterator_tag, Type>
				{
					public :
						BitSetIterator(const Collection& collection) : _collection(&collection), _bits(0), _position(Bits) {}
						BitSetIterator(const Collection& collection, const BitSet& bitset) : _collection(&collection), _bits(bitset) { operator++(); }

					public :
						inline bool operator==(const BitSetIterator& iterator) const
							{ return _position == iterator._position; }
						inline bool operator!=(const BitSetIterator& iterator) const
							{ return _position != iterator._position; }

						inline BitSetIterator& operator++()
							{ intel::bsf(_bits, &_position) ? (_bits = intel::btr(_bits, _position)) : (_position = Bits); return *this; }

						inline typename Collection::const_reference operator*() const
							{ return (*_collection)[_position]; }

					private :
						const Collection *_collection;
						bits_t _bits;
						int _position;
				};

				typedef BitSetIterator iterator;

			public :
				BitSetSelection(const Collection& collection, const BitSet& bitset) : _collection(collection), _bitset(bitset) {}

				inline BitSetIterator begin() const
					{ return BitSetIterator(_collection, _bitset); }
				inline BitSetIterator end() const
					{ return BitSetIterator(_collection); }

			private :
				const Collection& _collection;
				const BitSet& _bitset;
		};

		template <typename Collection>
		inline BitSetSelection<Collection> in(const Collection& collection) const
			{ return BitSetSelection<Collection>(collection, *this); }
};

/* -------------------------------------------------------------------------- */

}

#endif
