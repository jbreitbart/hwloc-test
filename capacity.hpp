#pragma once

#include <iostream>

struct capacity {
	bool enough(size_t v) { return _value >= v; }

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

  private:
	size_t _value;

	friend std::ostream &operator<<(std::ostream &o, const capacity &p);
};

inline std::ostream &operator<<(std::ostream &o, const capacity &p) { return o << p._value; }
