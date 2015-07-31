#pragma once

#include <vector>

#include "capacity.hpp"
#include "work_tree_node.hpp"

class work_tree {
  private:
	work_tree_node root;

	size_t translate(size_t id) const { return id; }

  public:
	work_tree() = delete;
	work_tree(std::vector<size_t> vector) : root(vector) {}

	size_t scatter(int key, int hyperthreads) { return translate(root.scatter(key, hyperthreads)); }

	size_t compact(int key, int hyperthreads) { return translate(root.compact(key, hyperthreads)); }

	void remove_all(int key) { root.remove_all(key); }
};
