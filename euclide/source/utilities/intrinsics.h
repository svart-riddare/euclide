#ifndef __EUCLIDE_INTRINSICS_H
#define __EUCLIDE_INTRINSICS_H

#include "../includes.h"

namespace Euclide
{
namespace intel
{

/* -------------------------------------------------------------------------- */

static inline bool bt(uint32_t bits, int bit)
{
#ifdef EUCLIDE_WIN_IMPLEMENTATION
	return _bittest(reinterpret_cast<const long *>(&bits), bit) ? true : false;
#else
	return ((bits >> bit) & 1) ? true : false;
#endif
}

static inline bool bt(uint64_t bits, int bit)
{
#ifdef EUCLIDE_WIN_IMPLEMENTATION
	return _bittest64(reinterpret_cast<const int64_t *>(&bits), bit) ? true : false;
#else
	return ((bits >> bit) & 1) ? true : false; 
#endif
}

/* -------------------------------------------------------------------------- */

static inline uint32_t bts(uint32_t bits, int bit)
{
#ifdef EUCLIDE_WIN_IMPLEMENTATION
	_bittestandset(reinterpret_cast<long *>(&bits), bit);
	return bits;
#else
	return bits | (uint32_t(1) << bit);
#endif
}

static inline uint64_t bts(uint64_t bits, int bit)
{
#ifdef EUCLIDE_WIN_IMPLEMENTATION
	_bittestandset64(reinterpret_cast<int64_t *>(&bits), bit);
	return bits;
#else
	return bits | (uint64_t(1) << bit);
#endif
}

/* -------------------------------------------------------------------------- */

static inline uint32_t btr(uint32_t bits, int bit)
{
#ifdef EUCLIDE_WIN_IMPLEMENTATION
	_bittestandreset(reinterpret_cast<long *>(&bits), bit);
	return bits;
#else
	return bits & ~(uint32_t(1) << bit);
#endif
}

static inline uint64_t btr(uint64_t bits, int bit)
{
#ifdef EUCLIDE_WIN_IMPLEMENTATION
	_bittestandreset64(reinterpret_cast<int64_t *>(&bits), bit);
	return bits;
#else
	return bits & ~(uint64_t(1) << bit);
#endif
}

/* -------------------------------------------------------------------------- */

static inline uint32_t btc(uint32_t bits, int bit)
{
#ifdef EUCLIDE_WIN_IMPLEMENTATION
	_bittestandcomplement(reinterpret_cast<long *>(&bits), bit);
	return bits;
#else
	return bits ^ (uint32_t(1) << bit);
#endif
}

static inline uint64_t btc(uint64_t bits, int bit)
{
#ifdef EUCLIDE_WIN_IMPLEMENTATION
	_bittestandcomplement64(reinterpret_cast<int64_t *>(&bits), bit);
	return bits;
#else
	return bits ^ (uint64_t(1) << bit);
#endif
}

/* -------------------------------------------------------------------------- */

static inline bool bsf(uint32_t bits, int *bit)
{
#ifdef EUCLIDE_WIN_IMPLEMENTATION
	return _BitScanForward(reinterpret_cast<unsigned long *>(bit), bits) ? true : false;
#else
	for (int k = 0; bits; k++, bits >>= 1)
		if (bits & 1)
			return (*bit = k), true;

	return false;
#endif
}

static inline bool bsf(uint64_t bits, int *bit)
{
#ifdef EUCLIDE_WIN_IMPLEMENTATION
	return _BitScanForward64(reinterpret_cast<unsigned long *>(bit), bits) ? true : false;
#else
	for (int k = 0; bits; k++, bits >>= 1)
		if (bits & 1)
			return (*bit = k), true;

	return false;
#endif
}

/* -------------------------------------------------------------------------- */

static inline int popcnt(uint32_t bits)
{
#ifdef EUCLIDE_WIN_IMPLEMENTATION
	return __popcnt(bits);
#else
	static const int8_t popcnt[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 }; 
	
	int count = 0;
	for ( ; bits; bits >>= 4) 
		count += popcnt[bits & 15]; 
	
	return count;
#endif
}

static inline int popcnt(uint64_t bits)
{
#ifdef EUCLIDE_WIN_IMPLEMENTATION
	return __popcnt64(bits);
#else
	static const int8_t popcnt[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 }; 
	
	int count = 0;
	for ( ; bits; bits >>= 4) 
		count += popcnt[bits & 15]; 
	
	return count;
#endif
}

/* -------------------------------------------------------------------------- */

}}

#endif
