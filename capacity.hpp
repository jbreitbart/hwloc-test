#pragma once

#include <iostream>

struct capacity {
	bool enough(size_t v) const { return _value >= v; }

	capacity &operator=(const size_t c) {
		_value = c;
		return *this;
	}

	capacity &operator-=(const size_t c) {
		_value -= c;
		return *this;
	}

	capacity &operator+=(const size_t c) {
		_value += c;
		return *this;
	}

	capacity &operator*=(const size_t c) {
		_value *= c;
		return *this;
	}

	template <typename T> T operator/(const T c) const { return static_cast<T>(_value / c); }

  private:
	size_t _value;

	friend std::ostream &operator<<(std::ostream &o, const capacity &p);
};

inline std::ostream &operator<<(std::ostream &o, const capacity &p) { return o << p._value; }
