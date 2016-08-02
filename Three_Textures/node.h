//node.h
#pragma once
#include "consts.h"
#include <list>

struct node {
	T x, y;
	node(const T& x = 0, const T& y = 0) : x(x), y(y) {}
	node(const std::pair< T, T >& p) : x(p.first), y(p.second) {}
	node(const node& n) : x(n.x), y(n.y) {}
	inline operator std::pair< T, T >() const {
		return std::pair< T, T >(x, y);
	}
	inline bool operator<(const node& n) const {
		if ((x + y < n.x + n.y) || ((x + y == n.x + n.y) && (y > n.y))) return true;
		return false;
	}
	inline bool operator==(const node& n) const {
		return y == n.y && x == n.x;
	}
	inline bool operator!=(const node& n) const {
		return !(*this == n);
	}
	inline bool operator>(const node& n) const {
		return !(*this == n || *this < n);
	}
	inline T distance(const node& n) const {
		//return std::max((x > n.x ? x - n.x : n.x - x), (y > n.y ? y - n.y : n.y - y));
		T l = (x > n.x ? x - n.x : n.x - x), r = (y > n.y ? y - n.y : n.y - y);
		return l > r ? l : r;
	}
};

#define GEN_OPERATORS(_ENUM_CLASS_)\
inline _ENUM_CLASS_ operator | (const _ENUM_CLASS_& l, const _ENUM_CLASS_& r) { return _ENUM_CLASS_(unsigned char(l) | unsigned char(r)); }\
inline _ENUM_CLASS_ operator & (const _ENUM_CLASS_& l, const _ENUM_CLASS_& r) { return _ENUM_CLASS_(unsigned char(l) & unsigned char(r)); }\
inline bool operator || (const _ENUM_CLASS_& l, const _ENUM_CLASS_& r) { return bool(l | r); }\
inline bool operator && (const _ENUM_CLASS_& l, const _ENUM_CLASS_& r) { return bool(l & r); }\
inline _ENUM_CLASS_& operator |= (_ENUM_CLASS_& l, const _ENUM_CLASS_& r) { return l = _ENUM_CLASS_(unsigned char(l) | unsigned char(r)); }\
inline _ENUM_CLASS_& operator &= (_ENUM_CLASS_& l, const _ENUM_CLASS_& r) { return l = _ENUM_CLASS_(unsigned char(l) & unsigned char(r)); }\
inline bool operator !(const _ENUM_CLASS_& val) { return !bool(val); }


enum class limits : unsigned char { NOTHING = 0, UP = 1, DOWN = 2, LEFT = 4, RIGHT = 8 }; GEN_OPERATORS(limits);


std::list< node > make_neighbors(const T& x, const T& y, const T& w, const T& h);
inline std::list< node > make_neighbors(const node& n, const T& w, const T& h) {
	return make_neighbors(n.x, n.y, w, h);
}


/*
1 2 3
8 x 4 //номера соседей
7 6 5
*/
enum class neighbor_number : unsigned char { N1 = 1, N2 = 2, N3 = 4, N4 = 8, N5 = 16, N6 = 32, N7 = 64, N8 = 128 }; GEN_OPERATORS(neighbor_number);
//приходит список соседей в порядке по часовой стрелке начиная с первого
std::list< node > make_neighbors(const T& x, const T& y, const T& w, const T& h, const neighbor_number& neighbors_number);
inline std::list< node > make_neighbors(const node& n, const T& w, const T& h, const neighbor_number& neighbors_number) {
	return make_neighbors(n.x, n.y, w, h, neighbors_number);
}