#include "node.h"
using namespace std;

list< node > make_neighbors(const T& x, const T& y, const T& w, const T& h) {
	limits limitless = (limits)(
		(y > 0) |
		((y + 1 < h) << 1) |
		((x > 0) << 2) |
		((x + 1 < w) << 3));

	list< node > neighbors;
	if (limitless && limits::UP) {
		if (limitless && limits::LEFT) neighbors.push_back(node(x - 1, y - 1));
		neighbors.push_back(node(x, y - 1));
		if (limitless && limits::RIGHT) neighbors.push_back(node(x + 1, y - 1));
	}
	if (limitless && limits::RIGHT) neighbors.push_back(node(x + 1, y));
	if (limitless && limits::DOWN) {
		if (limitless && limits::RIGHT) neighbors.push_back(node(x + 1, y + 1));
		neighbors.push_back(node(x, y + 1));
		if (limitless && limits::LEFT) neighbors.push_back(node(x - 1, y + 1));
	}
	if (limitless && limits::LEFT) neighbors.push_back(node(x - 1, y));

	return neighbors;
}


/*
1 2 3
8 x 4 //номера соседей
7 6 5
*/

//приходит список соседей в порядке по часовой стрелке начиная с первого
list< node > make_neighbors(const T& x, const T& y, const T& w, const T& h, const neighbor_number& neighbors_number) {
	limits limitless = (limits)(
		(y > 0) |
		((y + 1 < h) << 1) |
		((x > 0) << 2) |
		((x + 1 < w) << 3));

	list< node > neighbors;
	if (limitless && limits::UP) {
		if ((neighbors_number && neighbor_number::N1) && (limitless && limits::LEFT)) neighbors.push_back(node(x - 1, y - 1));
		if (neighbors_number && neighbor_number::N2) neighbors.push_back(node(x, y - 1));
		if ((neighbors_number && neighbor_number::N3) && (limitless && limits::RIGHT)) neighbors.push_back(node(x + 1, y - 1));
	}
	if ((neighbors_number && neighbor_number::N4) && (limitless && limits::RIGHT)) neighbors.push_back(node(x + 1, y));
	if (limitless && limits::DOWN) {
		if ((neighbors_number && neighbor_number::N5) && (limitless && limits::RIGHT)) neighbors.push_back(node(x + 1, y + 1));
		if (neighbors_number && neighbor_number::N6) neighbors.push_back(node(x, y + 1));
		if ((neighbors_number && neighbor_number::N7) && (limitless && limits::LEFT)) neighbors.push_back(node(x - 1, y + 1));
	}
	if ((neighbors_number && neighbor_number::N8) && (limitless && limits::LEFT)) neighbors.push_back(node(x - 1, y));

	return neighbors;
}