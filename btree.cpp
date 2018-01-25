// your btree implementation goes here
//#include "btree.h"
/**
 * Default constructor for BTree.
 *
 * @param maxNodeElems the maximum number of elements
 *        that can be stored in each B-Tree node
 */
template<typename T>
btree<T>::btree(size_t maxNodeElems) :
		root_(nullptr), head_(nullptr), tail_(nullptr), __maxNodeElems(maxNodeElems),
			__tree_Size_(0) {}

/**
 * Copy constructor
 * Creates a new B-Tree as a copy of original.
 *
 * @param original a const lvalue reference to a B-Tree object
 */
template<typename T>
btree<T>::btree(const btree<T>& original) :
		root_(nullptr), head_(nullptr), tail_(nullptr), __maxNodeElems(
				original.__maxNodeElems), __tree_Size_(0) {
	queue<typename btree<T>::Node*> queue_nodes;
	queue_nodes.push(original.root_);
	while (queue_nodes.size() != 0) {
		typename btree<T>::Node* tmp = queue_nodes.front();
		queue_nodes.pop();
		for (size_t i = 0; i < tmp->__occupied_size_; ++i) {
			insert(tmp->__elem_[i]);
		}
		if (tmp->is_full()) {
			for (size_t i = 0; i <= tmp->__occupied_size_; ++i) {
				if (tmp->__descendants_[i] != nullptr)
					queue_nodes.push(tmp->__descendants_[i]);
			}
		}
	}
}

/**
 * Move constructor
 * Creates a new B-Tree by "stealing" from original.
 *
 * @param original an rvalue reference to a B-Tree object
 */
template<typename T>
btree<T>::btree(btree<T> && original) :
		root_(original.root_), head_(original.head_), tail_(original.tail_), __maxNodeElems(
				original.__maxNodeElems), __tree_Size_(original.__tree_Size_) {
}

template<typename T>
btree<T>::~btree() {
	delete root_;
}

/**
 * Copy assignment
 * Replaces the contents of this object with a copy of rhs.
 *
 * @param rhs a const lvalue reference to a B-Tree object
 */
template<typename T> btree<T>&
btree<T>::operator=(const btree<T>& rhs) {
	if (this != &rhs) {
		delete root_;
		root_ = nullptr;
		head_ = nullptr;
		tail_ = nullptr;
		
		queue<typename btree<T>::Node*> queue_nodes;
		queue_nodes.push(rhs.root_);
		while (queue_nodes.size() != 0) {
			typename btree<T>::Node* tmp = queue_nodes.front();
			queue_nodes.pop();
			for (size_t i = 0; i < tmp->__occupied_size_; ++i) {
				insert(tmp->__elem_[i]);
			}
			if (tmp->is_full()) {
				for (size_t i = 0; i <= tmp->__occupied_size_; ++i) {
					if (tmp->__descendants_[i] != nullptr)
						queue_nodes.push(tmp->__descendants_[i]);
				}
			}
		}
		__maxNodeElems = rhs.__maxNodeElems;
		__tree_Size_ = rhs.__tree_Size_;
	}
	return *this;
}

/**
 * Move assignment
 * Replaces the contents of this object with the "stolen"
 * contents of original.
 *
 * @param rhs a const reference to a B-Tree object
 */
template<typename T> btree<T>&
btree<T>::operator=(btree<T> && rhs) {
	if (this != &rhs) {
		delete root_;
		root_ = nullptr;
		head_ = nullptr;
		tail_ = nullptr;
		__maxNodeElems = rhs.__maxNodeElems;
		__tree_Size_ = rhs.__tree_Size_;
		root_ = rhs.root_;

		rhs.root_ = new Node(__maxNodeElems);
		rhs.head_ = nullptr;
		rhs.tail_ = nullptr;
	}
	return *this;
}

template<typename T> typename btree<T>::Node&
btree<T>::Node::operator =(const btree<T>::Node& rhs) {
	if (this != &rhs) {
		delete this;
		__max_size_ = rhs.__max_size_;
		__occupied_size_ = rhs.__occupied_size_;
		__elem_ = new T[__max_size_];
		__parent_ = nullptr;
		for (size_t i = 0; i < __max_size_; ++i) {
			if (rhs.__descendants_[i] != nullptr) {
				Node tmp(*rhs.__descendants_[i]);
				__descendants_[i] = &tmp;
			}
		}
	}
	return *this;
}

template<typename T> typename btree<T>::iterator
btree<T>::begin() const {
//	Node* tmp = head_;
//	if(tmp != nullptr) {
//		while(tmp->__descendants_[0] != nullptr) 
//			tmp = tmp->__descendants_[0];
//		return iterator(tmp, 0, this);
//	} else {
//		return end();
//	}
	return iterator(head_, 0, this);
}

template<typename T> typename btree<T>::iterator
btree<T>::end() const {
//	Node* tmp = head_;
//	if (tmp != nullptr) {
//		while (tmp->__descendants_[tmp->__occupied_size_] != nullptr)
//			tmp = tmp->__descendants_[tmp->__occupied_size_];
//	}
//	return iterator(tmp, tmp->__occupied_size_, this);
	return iterator(NULL, 0, this);
}

template<typename T> typename btree<T>::reverse_iterator
btree<T>::rbegin() const {
	return reverse_iterator(end());
}

template<typename T> typename btree<T>::reverse_iterator
btree<T>::rend() const {
	return reverse_iterator(begin());
}

template<typename T> typename btree<T>::const_iterator
btree<T>::cbegin() const {
	return const_iterator(begin());
}

template<typename T> typename btree<T>::const_iterator
btree<T>::cend() const {
	return const_iterator(end());
}

template<typename T> typename btree<T>::const_reverse_iterator
btree<T>::crbegin() const {
	return const_reverse_iterator(end());
}

template<typename T> typename btree<T>::const_reverse_iterator
btree<T>::crend() const {
	return const_reverse_iterator(begin());
}

/**
 * Puts a breadth-first traversal of the B-Tree onto the output
 * stream os. Elements must, in turn, support the output operator.
 * Elements are separated by space. Should not output any newlines.
 *
 * @param os a reference to a C++ output stream
 * @param tree a const reference to a B-Tree object
 * @return a reference to os
 */
template<typename T> std::ostream&
operator<<(std::ostream& os, const btree<T>& tree) {
	vector<typename btree<T>::Node *> _currLevel;
	vector<typename btree<T>::Node *> _nextLevel;

	_currLevel.push_back(tree.root_);
	while (!_currLevel.empty()) {
		typename vector<typename btree<T>::Node *>::const_iterator iter;
		for (iter = _currLevel.begin(); iter != _currLevel.end(); ++iter) {
			typename btree<T>::Node * _node = *iter;
			size_t i;
			for (i = 0; i < _node->__occupied_size_; ++i) {
				os << _node->__elem_[i] << " ";
				if (_node->__descendants_[i] != nullptr)
					_nextLevel.push_back(_node->__descendants_[i]);
			}
			// descendant always have one more
			if (_node->__descendants_[i] != nullptr) 
				_nextLevel.push_back(_node->__descendants_[i]);
		}
		_currLevel = _nextLevel;
		_nextLevel.clear();
	}
	return os;
}

/**
 * Returns an iterator to the matching element, or whatever
 * the non-const end() returns if the element could
 * not be found.
 *
 * @param elem the client element we are trying to match.  The elem,
 *        if an instance of a true class, relies on the operator< and
 *        and operator== methods to compare elem to elements already
 *        in the btree.  You must ensure that your class implements
 *        these things, else code making use of btree<T>::find will
 *        not compile.
 * @return an iterator to the matching element, or whatever the
 *         non-const end() returns if no such match was ever found.
 */
template<typename T> typename btree<T>::iterator
btree<T>::find(const T& elem) {
	Node *tmp_ = root_;
	while(true) {
		size_t i;
		// go through all elements from root to tail
		for (i = 0; i < tmp_->__occupied_size_; ++i) {
			if (tmp_->__elem_[i] == elem) {
				// find the elem, return an iterator
				return iterator(tmp_, i, this);
			} else if (tmp_->__elem_[i] > elem) {
				// elem is not in current Node, go to descendants
				// for the elem.
				if (tmp_->__descendants_ == nullptr) {
					return end();
				} else {
					tmp_ = tmp_->__descendants_[i];
					break;
				}
			}
		}
		// handling boundaries cases
		if (i == tmp_->__occupied_size_) {
			if (tmp_->__descendants_[i] == nullptr) {
				return end();
			} else {
				tmp_ = tmp_->__descendants_[i];
			}
		}
	}
}

/**
 * Identical in functionality to the non-const version of find,
 * save the fact that what's pointed to by the returned iterator
 * is deemed as const and immutable.
 *
 * @param elem the client element we are trying to match.
 * @return an iterator to the matching element, or whatever the
 *         const end() returns if no such match was ever found.
 */
template<typename T> typename btree<T>::const_iterator
btree<T>::find(const T& elem) const {
	Node *tmp_ = root_;
	while(true) {
		size_t i;
		// go through all elements from root to tail
		for (i = 0; i < tmp_->__occupied_size_; ++i) {
			if (tmp_->__elem_[i] == elem) {
				// find the elem, return an iterator
				return const_iterator(tmp_, i, this);
			} else if (tmp_->__elem_[i] > elem) {
				// elem is not in current Node, go to descendants
				// for the elem.
				if (tmp_->__descendants_ == nullptr) {
					return cend();
				} else {
					tmp_ = tmp_->__descendants_[i];
					break;
				}
			}
		}
		// handling boundaries cases
		if (i == tmp_->__occupied_size_) {
			if (tmp_->__descendants_[i] == nullptr) {
				return cend();
			} else {
				tmp_ = tmp_->__descendants_[i];
			}
		}
	}
}

/**
 * Operation which inserts the specified element
 * into the btree if a matching element isn't already
 * present.  In the event where the element truly needs
 * to be inserted, the size of the btree is effectively
 * increases by one, and the pair that gets returned contains
 * an iterator to the inserted element and true in its first and
 * second fields.
 *
 * If a matching element already exists in the btree, nothing
 * is added at all, and the size of the btree stays the same.  The
 * returned pair still returns an iterator to the matching element, but
 * the second field of the returned pair will store false.  This
 * second value can be checked to after an insertion to decide whether
 * or not the btree got bigger.
 *
 * The insert method makes use of T's zero-arg constructor and
 * operator= method, and if these things aren't available,
 * then the call to btree<T>::insert will not compile.  The implementation
 * also makes use of the class's operator== and operator< as well.
 *
 * @param elem the element to be inserted.
 * @return a pair whose first field is an iterator positioned at
 *         the matching element in the btree, and whose second field
 *         stores true if and only if the element needed to be added
 *         because no matching element was there prior to the insert call.
 */
template<typename T> std::pair<typename btree<T>::iterator, bool>
btree<T>::insert(const T& elem) {
	Node **ptr_ = &root_;
	Node *parent = NULL;
	bool new_head = false;
	bool new_tail = false;
	while (true) {
		Node *tmp_ = *ptr_;
		if (tmp_ == nullptr) {
			// No element in the btree currently, create a new Node elem
			*ptr_ = new Node(elem, __maxNodeElems, parent);
			if (*ptr_ == root_) {
				head_ = root_;
				tail_ = root_;
			} else if (new_head == true) {
				head_ = *ptr_;
				new_head = false;
			} else if (new_tail == true) {
				tail_ = *ptr_;
				new_tail = false;
			}
			++__tree_Size_;
			return make_pair(iterator(tmp_, 0, this), true);
		} else {
			// search current node to see if elem exists
			size_t i;
			for (i = 0; i < tmp_->__occupied_size_; ++i) {
				if (tmp_->__elem_[i] == elem) {
					// elem already exists in current node, should return false.
					return make_pair(iterator(tmp_, i, this), false);
				} else if (tmp_->__elem_[i] > elem) {
					if (tmp_->is_full()) {
						// current node is full, need to check descendants nodes
//						for (auto t = 0; t < tmp_->__occupied_size_; ++t)
//							cout << tmp_->__elem_[t] << " ";
//						cout << endl;
						ptr_ = &(tmp_->__descendants_[i]);
						break;
					} else {
						// move existing element 
						for (size_t j = tmp_->__occupied_size_ - 1; j >= i && j > 0; --j) {
							tmp_->__elem_[j + 1] = tmp_->__elem_[j];
						}
						tmp_->__elem_[i] = elem;
						break;
					}
				}
			}

			// handling boundaries cases
			if (tmp_->is_full()) {
//				std::cout << i << std::endl;
				if (i == 0) {
					if (tmp_ == head_)
						new_head = true;
				} else if (i == __maxNodeElems) {
					ptr_ = &(tmp_->__descendants_[i]);
					if (tmp_ == tail_)
						new_tail = true;
				}
				parent = tmp_;
			} else {
				// elem is larger than all exist elements, add to the end
				if (tmp_->__occupied_size_ == i)
					tmp_->__elem_[i] = elem;
				++tmp_->__occupied_size_;
				++__tree_Size_;
				return make_pair(iterator(tmp_, i, this), true);
			}

		}

	}

}