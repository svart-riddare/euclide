#ifndef __EUCLIDE_ALGORITHM_H
#define __EUCLIDE_ALGORITHM_H

#include "../includes.h"

namespace xstd
{

/* -------------------------------------------------------------------------- */

template<typename Iterator, typename T> inline
bool all(Iterator first, Iterator last, const T& value)
	{ return std::all_of(first, last, [&](const T& item) { return value == item; }); }

template<typename Iterator, typename T> inline
bool any(Iterator first, Iterator last, const T& value)
	{ return std::any_of(first, last, [&](const T& item) { return value == item; }); }

template<typename Iterator, typename T> inline
bool none(Iterator first, Iterator last, const T& value)
	{ return std::none_of(first, last, [&](const T& item) { return value == item; }); }

template<typename Iterator, typename T, typename X, typename Function> inline
T sum(Iterator first, Iterator last, T zero, const Function& function)
	{ return std::accumulate(first, last, zero, [&](T sum, const typename Iterator::value_type& x) { return sum + function(x); }); }

template<typename Iterator, typename T, typename X, typename Function> inline
T min(Iterator first, Iterator last, T initial, const Function& function)
	{ return std::accumulate(first, last, initial, [&](T min, const typename Collection::value_type& x) { return std::min<T>(min, function(x)); }); }

template<typename Iterator, typename T, typename X, typename Function> inline
T max(Iterator first, Iterator last, T initial, const Function& function)
	{ return std::accumulate(first, last, initial, [&](T max, const typename Collection::value_type& x) { return std::max<T>(max, function(x)); }); }

/* -------------------------------------------------------------------------- */

template <typename Collection, typename T> inline
bool all(const Collection& collection, const T& value)
	{ return std::all_of(std::begin(collection), std::end(collection), [&](const T& item) { return value == item; }); }

template <typename Collection, typename T> inline
bool any(const Collection& collection, const T& value)
	{ return std::any_of(std::begin(collection), std::end(collection), [&](const T& item) { return value == item; }); }

template <typename Collection, typename T> inline
bool none(const Collection& collection, const T& value)
	{ return std::none_of(std::begin(collection), std::end(collection), [&](const T& item) { return value == item; }); }

template<typename Collection, typename T, typename Function> inline
T sum(const Collection& collection, T zero, const Function& function)
	{ return std::accumulate(std::begin(collection), std::end(collection), zero, [&](T sum, const typename Collection::value_type& x) { return sum + function(x); }); }

template<typename Collection, typename Function> inline
int sum(const Collection& collection, const Function& function)
	{ return xstd::sum(collection, 0, function); }

template<typename Collection, typename T, typename Function> inline
T min(const Collection& collection, T initial, const Function& function)
	{ return std::accumulate(std::begin(collection), std::end(collection), initial, [&](T min, const typename Collection::value_type& x) { return std::min<T>(min, function(x)); }); }

template<typename Collection, typename Function> inline
int min(const Collection& collection, const Function& function)
	{ return xstd::min(collection, std::numeric_limits<int>::max(), function); }

template<typename Collection, typename T, typename Function> inline
T max(const Collection& collection, T initial, const Function& function)
	{ return std::accumulate(std::begin(collection), std::end(collection), initial, [&](T max, const typename Collection::value_type& x) { return std::max<T>(max, function(x)); }); }

template<typename Collection, typename Function> inline
int max(const Collection& collection, const Function& function)
	{ return xstd::max(collection, std::numeric_limits<int>::min(), function); }

/* -------------------------------------------------------------------------- */

template <typename Collection, typename T> inline
T accumulate(const Collection& collection, T zero)
	{ return std::accumulate(std::begin(collection), std::end(collection), zero); }

template <typename Collection, typename T, typename Function> inline
T accumulate(const Collection& collection, T zero, const Function& function)
	{ return std::accumulate(std::begin(collection), std::end(collection), zero, function); }

template <typename Collection, typename Predicate> inline
bool all_of(const Collection& collection, const Predicate& predicate)
	{ return std::all_of(std::begin(collection), std::end(collection), predicate); }

template <typename Collection, typename Predicate> inline
bool any_of(const Collection& collection, const Predicate& predicate)
	{ return std::any_of(std::begin(collection), std::end(collection), predicate); }

template <typename Collection, typename Iterator> inline
Iterator copy(const Collection& collection, Iterator iterator)
	{ return std::copy(std::begin(collection), std::end(collection), iterator); }

template <typename Collection, typename T> inline
typename Collection::size_type count(const Collection& collection, const T& value)
	{ return std::count(std::begin(collection), std::end(collection), value); }

template <typename Collection, typename Predicate> inline
typename Collection::size_type count_if(const Collection& collection, const Predicate& predicate)
	{ return std::count_if(std::begin(collection), std::end(collection), predicate); }

template <typename Collection> inline
typename Collection::const_iterator max_element(const Collection& collection)
	{ return std::move(std::max_element(std::begin(collection), std::end(collection))); }

template <typename Collection, typename Function> inline
typename Collection::const_iterator max_element(const Collection& collection, const Function& function)
	{ return std::move(std::max_element(std::begin(collection), std::end(collection), function)); }

template <typename Collection> inline
typename Collection::const_iterator min_element(const Collection& collection)
	{ return std::move(std::min_element(std::begin(collection), std::end(collection))); }

template <typename Collection, typename Function> inline
typename Collection::const_iterator min_element(const Collection& collection, const Function& function)
	{ return std::move(std::min_element(std::begin(collection), std::end(collection), function)); }

template <typename Collection, typename Function> inline
Function for_each(const Collection& collection, const Function& function)
	{ return std::move(std::for_each(std::begin(collection), std::end(collection), function)); }

template <typename Collection, typename Predicate> inline
bool none_of(const Collection& collection, const Predicate& predicate)
	{ return std::none_of(std::begin(collection), std::end(collection), predicate); }

/* -------------------------------------------------------------------------- */

template <class T>
T& maximize(T& variable, const T& value)
	{ return variable = std::max(variable, value); }

template <class T>
T& minimize(T& variable, const T& value)
	{ return variable = std::min(variable, value); }

template <class T>
void maximize(T *variables, const T *values, int size)
	{ for (int k = 0; k < size; k++) variables[k] = std::max(variables[k], values[k]); }

template <class T>
void minimize(T *variables, const T *values, int size)
	{ for (int k = 0; k < size; k++) variables[k] = std::min(variables[k], values[k]); }

/* -------------------------------------------------------------------------- */

}

#endif
