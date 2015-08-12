#pragma once

#include <iostream>

#include <vector>
#include <map>
#include <iterator>

#include <memory>
#include <functional>

#include <cassert>
#include <limits>

#include "capacity.hpp"

class work_tree {
  private:
	struct node {
		// Array of children
		std::vector<node> _children;

		// Stores a application key and how many ressources are used
		std::map<int, int> _processes;

		// The available ressources
		capacity _cap;

		// Pointer to the parent
		node *const _parent = nullptr;
		size_t _id = 0;
		size_t _level = 0;

		node() = delete;
		node(node &&) = default;

		// called by the tree
		node(work_tree const *const tree, const std::vector<size_t> &list) : node(tree, list, nullptr, nullptr, 0) {}

		// recursivly generate the whole tree
		node(work_tree const *const tree, const std::vector<size_t> &list, size_t *counters, node *const parent,
			 const size_t level)
			: _parent(parent), _level(level) {

			assert(list[0] == 1);

			if (parent == nullptr) {
				counters = new size_t[list.size()];
				for (size_t i = 0; i < list.size(); ++i) counters[i] = 0;
			}

			// compute capacity
			_cap = 1;
			size_t temp = list.size();
			for (auto it = std::crbegin(list); it != std::crend(list); ++it) {
				if (level + 1 >= temp) {
					break;
				}
				--temp;
				_cap *= *it;
			}

			_id = counters[level];

			// we expect the last level to be hyperthreads
			if (level != list.size() - 1) {
				++counters[level];
			} else {
				// last level. We expect hyperthreading here
				assert(list.size() > 1);
				size_t nb_of_real_cores = tree->_root._cap / list[list.size() - 1];
				if (counters[level] >= nb_of_real_cores) {
					counters[level] -= nb_of_real_cores;
					++counters[level];
				} else {
					counters[level] += nb_of_real_cores;
				}
			}

			std::cout << "Created node with ID " << _id << " @ level " << level << " with cap " << _cap << std::endl;
			// end recursion if we are at the end of the list
			if (level == list.size() - 1) {
				return;
			}

			// create the node in place
			for (size_t i = 0; i < list[level + 1]; ++i) {
				_children.emplace_back(tree, list, counters, this, level + 1);
			}

			if (parent == nullptr) delete[] counters;
		}

		~node() {}

		void remove_all(int key) {
			if (_processes.count(key) == 0) {
				// std::cout << "nothing at " << _id << "@" << _level << " - " << _cap << std::endl;
				return;
			}

			for (size_t i = 0; i < _children.size(); ++i) _children[i].remove_all(key);

			auto it = _processes.find(key);
			_cap += it->second;
			_processes.erase(it);
			// std::cout << "increased cap on " << _id << "@" << _level << " by " << it->second << " to " << _cap <<
			// std::endl;
		}

		void reserve(int key, int hyperthreads) {
			auto temp = _processes.find(key);
			if (temp == _processes.end()) {
				_processes.insert(std::pair<int, int>(key, hyperthreads));
			} else {
				temp->second += hyperthreads;
			}
			_cap -= hyperthreads;
			// std::cout << "decreased cap on " << _id << "@" << _level << " by " << hyperthreads << " to " << _cap
			//		  << std::endl;

			if (_parent != nullptr) _parent->reserve(key, hyperthreads);
		}
	};

  private:
	node _root;

  public:
	work_tree() = delete;
	work_tree(const work_tree &) = delete;
	work_tree(std::vector<size_t> vector) : _root(this, vector) {}
	~work_tree() {}

  private:
	size_t execute_algorithm(int key, int hyperthreads, std::function<size_t(const std::vector<node> &, size_t)> f) {
		node *n = &_root;
		while (true) {
			if (!n->_cap.enough(hyperthreads)) throw "kein platz mehr :)";

			size_t min_i = std::numeric_limits<size_t>::max();

			min_i = f(n->_children, std::numeric_limits<size_t>::max());

			// true if there are no children of if the capacity of the children is not big enough
			if (min_i == std::numeric_limits<size_t>::max()) {
				std::cout << "allocating key " << key << " on " << n->_id << "@" << n->_level << std::endl;
				n->reserve(key, hyperthreads);
				// we need to return a real cpu id. So lets walk down until we get the final result!
				while (n->_children.size() != 0) {
					n = &(n->_children[0]);
				}
				return n->_id;
			}

			n = &(n->_children[min_i]);
		}
	}

  public:
	size_t scatter(int key, int hyperthreads) {
		return execute_algorithm(key, hyperthreads, [=](const std::vector<node> &children, size_t ri) -> size_t {
			size_t min_v = std::numeric_limits<size_t>::max();
			for (size_t i = 0; i < children.size(); ++i) {
				const node &c = children[i];
				auto count = c._processes.count(key);
				if (c._cap.enough(hyperthreads)) {
					if (count < min_v) {
						min_v = count;
						ri = i;
					}
				}
			}
			return ri;
		});
	}

	size_t compact(int key, int hyperthreads) {
		return execute_algorithm(key, hyperthreads, [=](const std::vector<node> &children, size_t ri) -> size_t {
			size_t max_v = std::numeric_limits<size_t>::max();
			for (size_t i = 0; i < children.size(); ++i) {
				const node &c = children[i];
				auto count = c._processes.count(key);
				if (c._cap.enough(hyperthreads)) {
					if (count >= max_v) {
						max_v = count;
						ri = i;
					}
				}
			}
			return ri;
		});
	}

	void remove_all(int key) { _root.remove_all(key); }
};
