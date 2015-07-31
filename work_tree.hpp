#pragma once

#include "capacity.hpp"
#include "work_tree_node.hpp"

class work_tree {
  private:
	work_tree_node root;

	int translate(int id) const { return id; }

  public:
	work_tree() = delete;
	template <typename T> work_tree(std::initializer_list<T> list) : root(list) {}

	int scatter(int key, int hyperthreads) { return translate(root.scatter(key, hyperthreads)); }

	int compact(int key, int hyperthreads) { return translate(root.compact(key, hyperthreads)); }

	void remove_all(int key) { root.remove_all(key); }
};
