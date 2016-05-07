#pragma once

#include <cstddef>
#include <utility> //for move()

using namespace std;

struct Nothing{};

template <typename T>
class Maybe{
	const T *val;

public:
	Maybe(T value);
	Maybe(Nothing);

	~Maybe();

	T fromJust() const;

	T fromMaybe(T &def) const;

	bool isJust() const;
	bool isNothing() const;
};

template <typename T>
Maybe<T>::Maybe(T value)
	:val(new T(move(value))){}

template <typename T>
Maybe<T>::Maybe(Nothing)
	:val(nullptr){}

template <typename T>
Maybe<T>::~Maybe() {
	if(val)delete val;
}


template <typename T>
T Maybe<T>::fromJust() const {
	return *val;
}

template <typename T>
T Maybe<T>::fromMaybe(T &def) const {
	return val?*val:def;
}

template <typename T>
bool Maybe<T>::isJust() const {
	return (bool)val;
}

template <typename T>
bool Maybe<T>::isNothing() const {
	return !val;
}
