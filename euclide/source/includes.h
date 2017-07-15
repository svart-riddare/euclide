#ifndef __EUCLIDE_INCLUDES_H
#define __EUCLIDE_INCLUDES_H

/* -------------------------------------------------------------------------- */

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

/* -------------------------------------------------------------------------- */

#include <array>
#include <algorithm>
#include <new>

using std::array;

/* -------------------------------------------------------------------------- */

#define countof(array) std::extent<decltype(array)>::value

#define static_assert(expression) \
	static_assert((expression), #expression)

namespace xstd
{
	template<typename Iterator, typename T> inline
	bool all(Iterator first, Iterator last, const T& value)
		{ return std::all_of(first, last, [&](const T& item) { return value == item; }); }

	template<typename Iterator, typename T> inline
	bool any(Iterator first, Iterator last, const T& value)
		{ return std::any_of(first, last, [&](const T& item) { return value == item; }); }

	template<typename Iterator, typename T> inline
	bool none(Iterator first, Iterator last, const T& value)
		{ return std::none_of(first, last, [&](const T& item) { return value == item; }); }

	template <typename Collection, typename T> inline
	typename Collection::size_type count(const Collection& collection, const T& value)
		{ return std::count(collection.begin(), collection.end(), value); }

	template <typename Collection, typename Predicate> inline
	typename Collection::size_type count_if(const Collection& collection, const Predicate& predicate)
		{ return std::count_if(collection.begin(), collection.end(), predicate); }

	template <typename Collection, typename T> inline
	bool all(const Collection& collection, const T& value)
		{ return std::all_of(collection.begin(), collection.end(), [&](const T& item) { return value == item; }); }

	template <typename Collection, typename Predicate> inline
	bool all_of(const Collection& collection, const Predicate& predicate)
		{ return std::all_of(collection.begin(), collection.end(), predicate); }

	template <typename Collection, typename Predicate> inline
	bool any_of(const Collection& collection, const Predicate& predicate)
		{ return std::any_of(collection.begin(), collection.end(), predicate); }
}

/* -------------------------------------------------------------------------- */

#include "../interface/euclide.h"
#include "definitions.h"

/* -------------------------------------------------------------------------- */

namespace Euclide
{
	extern const wchar_t *Copyright;
}

/* -------------------------------------------------------------------------- */

#endif
