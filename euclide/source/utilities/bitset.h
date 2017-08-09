#ifndef __EUCLIDE_BITSET_H
#define __EUCLIDE_BITSET_H

#include "../includes.h"
#include "iterator.h"
#include <intrin.h>

namespace Euclide
{

/* -------------------------------------------------------------------------- */

template <typename Type, int Bits> 
class BitSet
{
	private :
		typename typedef std::conditional<Bits <= 32, uint32_t, uint64_t>::type bits_t;
		bits_t _bits;

	public :
		BitSet(bits_t bits = 0) : _bits(bits) {}

		inline BitSet& set()
			{ _bits = ~bits_t(0); return *this; }
		inline BitSet& reset()
			{ _bits = bits_t(0); return *this; }
		inline BitSet& flip()
			{ _bits = ~_bits; return *this; }

		inline BitSet& set(Type position, bool value = true)
			{ assert((position >= 0) && (position < Bits)); if (value) _bits |= (bits_t(1) << position); else reset(position); return *this; }
		inline BitSet& reset(Type position)
			{ assert((position >= 0) && (position < Bits)); _bits &= ~(bits_t(1) << position); return *this; }
		inline BitSet& flip(Type position)
			{ assert((position >= 0) && (position < Bits)); _bits ^= ~(bits_t(1) << position); return *this; }

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
			{ assert((position >= 0) && (position < Bits)); return ((_bits >> position) & 1); }

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
			{ return __popcnt64(_bits); }
		inline int size() const
			{ return Bits; }

	public :
		inline Type first() const
			{ for (int position = 0; position < Bits; position++) if (test(static_cast<Type>(position))) return static_cast<Type>(position); return static_cast<Type>(Bits); }
		inline Type next(int position) const
			{ while (++position < Bits) if (test(static_cast<Type>(position))) return static_cast<Type>(position); return static_cast<Type>(Bits); }

	public :
		inline BitSet& operator&=(const BitSet& bitset)
			{ _bits &= bitset._bits; return *this; }
		inline BitSet& operator|=(const BitSet& bitset)
			{ _bits |= bitset._bits; return *this; }
		inline BitSet& operator^=(const BitSet& bitset)
			{ _bits ^= bitset._bits; return *this; }

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
		template <typename BitSet>
		class BaseIterator
		{
			public :
				BaseIterator(BitSet& bitset, Type position) : _bitset(bitset), _position(position) {}

			public :
				inline bool operator==(const BaseIterator& iterator) const
					{ return _position == iterator._position; }
				inline bool operator!=(const BaseIterator& iterator) const
					{ return _position != iterator._position; }
				inline bool operator<=(const BaseIterator& iterator) const
					{ return _position <= iterator._position; }
				inline bool operator>=(const BaseIterator& iterator) const
					{ return _position >= iterator._position; }
				inline bool operator<(const BaseIterator& iterator) const
					{ return _position < iterator._position; }
				inline bool operator>(const BaseIterator& iterator) const
					{ return _position > iterator._position; }

			private :
				BitSet& _bitset;
				Type _position;
		};

		class ConstIterator : public BaseIterator<const BitSet>
		{
			public :
				ConstIterator(const BitSet& bitset, Type position) : BaseIterator(bitset, position) {}
		};

		class Iterator : public BaseIterator<BitSet>
		{
			public :
				Iterator(BitSet& bitset, Type position) : BaseIterator(bitset, position) {}
		};

		inline ConstIterator begin() const
			{ return ConstIterator(*this, static_cast<Type>(0)); }
		inline ConstIterator cbegin() const
			{ return ConstIterator(*this, static_cast<Type>(0)); }
		inline ConstIterator end() const
			{ return ConstIterator(*this, static_cast<Type>(Bits)); }
		inline ConstIterator cend() const
			{ return ConstIterator(*this, static_cast<Type>(Bits)); }

		inline Iterator begin()
			{ return Iterator(*this, static_cast<Type>(0)); }
		inline Iterator end()
			{ return Iterator(*this, static_cast<Type>(Bits)); }

	public :
		class BitSetRange
		{
			public :
				class BitSetIterator : public std::iterator<std::forward_iterator_tag, Type>
				{
					public :
						BitSetIterator(const BitSet& bitset) : _bitset(&bitset), _position(bitset.first()) {}
						BitSetIterator(const BitSet& bitset, Type position) : _bitset(&bitset), _position(position) {}

					public :
						inline bool operator==(const BitSetIterator& iterator) const
							{ return _position == iterator._position; }
						inline bool operator!=(const BitSetIterator& iterator) const
							{ return _position != iterator._position; }

						inline BitSetIterator& operator++()
							{ _position = _bitset->next(_position); return *this; }

						inline Type operator*() const
							{ return _position; }

					private :
						const BitSet *_bitset;
						Type _position;
				};

				typedef BitSetIterator iterator;
				typedef BitSetIterator const_iterator;

			public :
				BitSetRange(const BitSet& bitset) : _bitset(bitset) {}

				inline BitSetIterator begin() const
					{ return BitSetIterator(_bitset); }
				inline BitSetIterator end() const
					{ return BitSetIterator(_bitset, static_cast<Type>(Bits)); }

			private :
				const BitSet& _bitset;
		};

		inline BitSetRange range() const
			{ return BitSetRange(*this); }
};

/* -------------------------------------------------------------------------- */

}

#endif
