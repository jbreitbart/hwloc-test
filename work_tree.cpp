#include "work_tree.hpp"

#include <iostream>

size_t work_tree_node::id_counter = 0;

int main(int argc, char const *argv[]) {


	work_tree asd({1, 2, 4, 2});

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
