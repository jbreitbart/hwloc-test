#pragma once

struct capacity {
	bool enough(int v) { return _value >= v; }

	capacity &operator=(const int c) {
		_value = c;
		return *this;
	}

	capacity &operator-=(const int c) {
		_value -= c;
		return *this;
	}

	capacity &operator+=(const int c) {
		_value += c;
		return *this;
	}

	capacity &operator*=(const int c) {
		_value *= c;
		return *this;
	}

  private:
	int _value;

	friend std::ostream &operator<<(std::ostream &o, const capacity &p);
};

inline std::ostream &operator<<(std::ostream &o, const capacity &p) { return o << p._value; }
