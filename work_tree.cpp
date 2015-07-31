#include "work_tree.hpp"

#include <iostream>
#include <cassert>
#include <hwloc.h>

size_t work_tree_node::id_counter = 0;

int main(int argc, char const *argv[]) {
	hwloc_topology_t top;
	assert(0 == hwloc_topology_init(&top));
	assert(0 == hwloc_topology_load(top));
	std::vector<size_t> temp;
	temp.push_back(hwloc_get_nbobjs_by_depth(top, 0));
	for (unsigned int i = 1; i < hwloc_topology_get_depth(top); ++i) {
		auto v = hwloc_get_nbobjs_by_depth(top, i);
		auto prev = *(temp.end() - 1);
		if (prev != v) {
			v = v / prev;
			temp.push_back(v);
		}
	}

	for (auto v : temp) std::cout << v;
	std::cout << std::endl;

	work_tree asd(temp);

	std::cout << "=== COMPACT ===" << std::endl;
	asd.compact(0, 1);
	asd.compact(0, 2);

	std::cout << "=== CLEARING ===" << std::endl;
	asd.remove_all(0);

	std::cout << "=== SCATTER ===" << std::endl;
	asd.scatter(2, 1);
	asd.scatter(2, 2);

	asd.scatter(1, 1);

	return 0;
}
