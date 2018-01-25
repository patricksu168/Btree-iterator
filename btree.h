/**
 * The btree is a linked structure which operates much like
 * a binary search tree, save the fact that multiple client
 * elements are stored in a single node.  Whereas a single element
 * would partition the tree into two ordered subtrees, a node 
 * that stores m client elements partition the tree 
 * into m + 1 sorted subtrees.
 */

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <cstddef>
#include <utility>
#include <vector>
#include <memory>
#include <algorithm>
#include <queue>

// we better include the iterator
#include "btree_iterator.h"

// we do this to avoid compiler errors about non-template friends
// what do we do, remember? :)
template<typename T> class btree;

template <typename T> 
class btree {
 public:
  /** Hmm, need some iterator typedefs here... friends? **/
 	friend class btree_iterator<T>;
    friend class const_btree_iterator<T>;
 	typedef btree_iterator<T> iterator;
    typedef const_btree_iterator<T> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  /**
   * Constructs an empty btree.  Note that
   * the elements stored in your btree must
   * have a well-defined copy constructor and destructor.
   * The elements must also know how to order themselves
   * relative to each other by implementing operator<
   * and operator==. (These are already implemented on
   * behalf of all built-ins: ints, doubles, strings, etc.)
   * 
   * @param maxNodeElems the maximum number of elements
   *        that can be stored in each B-Tree node
   */
  btree(std::size_t maxNodeElems = 40);

  /**
   * The copy constructor and  assignment operator.
   * They allow us to pass around B-Trees by value.
   * Although these operations are likely to be expensive
   * they make for an interesting programming exercise.
   * Implement these operations using value semantics and 
   * make sure they do not leak memory.
   */

  /** 
   * Copy constructor
   * Creates a new B-Tree as a copy of original.
   *
   * @param original a const lvalue reference to a B-Tree object
   */
  btree(const btree<T>& original);

  /** 
   * Move constructor
   * Creates a new B-Tree by "stealing" from original.
   *
   * @param original an rvalue reference to a B-Tree object
   */
  btree(btree<T>&& original);
  
  
  /** 
   * Copy assignment
   * Replaces the contents of this object with a copy of rhs.
   *
   * @param rhs a const lvalue reference to a B-Tree object
   */
  btree<T>& operator=(const btree<T>& rhs);

  /** 
   * Move assignment
   * Replaces the contents of this object with the "stolen"
   * contents of original.
   *
   * @param rhs a const reference to a B-Tree object
   */
  btree<T>& operator=(btree<T>&& rhs);

  /**
   * Puts a breadth-first traversal of the B-Tree onto the output
   * stream os. Elements must, in turn, support the output operator.
   * Elements are separated by space. Should not output any newlines.
   *
   * @param os a reference to a C++ output stream
   * @param tree a const reference to a B-Tree object
   * @return a reference to os
   */

  /*
  friend std::ostream& operator<< <T> (std::ostream& os, const btree<T>& tree);
  */


  /**
   * The following can go here
   * -- begin() 
   * -- end() 
   * -- rbegin() 
   * -- rend() 
   * -- cbegin() 
   * -- cend() 
   * -- crbegin() 
   * -- crend() 
   */

    iterator begin() const;
    iterator end() const{ return iterator{std::make_shared<Node>(), 0, this}; }
    const_iterator cbegin() const;
    const_iterator cend() const{ return const_iterator{std::make_shared<Node>(), 0, this}; };
    reverse_iterator rbegin() const {
        search_count = 0;
        return reverse_iterator{iterator{tail.lock(), tail.lock()->element.size(), this}}; 
    };
    reverse_iterator rend() const { return reverse_iterator{begin()}; };
    const_reverse_iterator crbegin() const {
        search_count = 0; 
        return const_reverse_iterator{const_iterator{tail.lock(), tail.lock()->element.size(), this}}; 
    };
    const_reverse_iterator crend() const { return const_reverse_iterator{cbegin()}; };
  
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
  iterator find(const T& elem);
    
  /**
    * Identical in functionality to the non-const version of find, 
    * save the fact that what's pointed to by the returned iterator
    * is deemed as const and immutable.
    *
    * @param elem the client element we are trying to match.
    * @return an iterator to the matching element, or whatever the
    *         const end() returns if no such match was ever found.
    */
  const_iterator find(const T& elem) const;
      
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
    * The insert method makes use of T's copy constructor,
    * and if these things aren't available, 
    * then the call to btree<T>::insert will not compile.  The implementation
    * also makes use of the class's operator== and operator< as well.
    *
    * @param elem the element to be inserted.
    * @return a pair whose first field is an iterator positioned at
    *         the matching element in the btree, and whose second field 
    *         stores true if and only if the element needed to be added 
    *         because no matching element was there prior to the insert call.
    */
  std::pair<iterator, bool> insert(const T& elem);

  /**
    * Disposes of all internal resources, which includes
    * the disposal of any client objects previously
    * inserted using the insert operation. 
    * Check that your implementation does not leak memory!
    */
  ~btree() { root.reset(); };

  //size_t get_max_elem() { return maxNodeElems; }
  //Node* get_root_node() { return root; }
  
private:
  // The details of your implementation go here
	struct Node {
		Node(const T& elem, std::size_t cap = 40, std::shared_ptr<Node> parent_arg = nullptr): 
			parent{parent_arg}, 
			capacity{cap} { 
                for (size_t count = 0; count < capacity + 1; ++count) children.push_back(nullptr);
                element.push_back(elem); 
            };

		Node(std::size_t cap = 40, std::shared_ptr<Node> parent_arg = nullptr):
			parent{parent_arg},
			capacity{cap} {
                for (size_t count = 0; count < capacity + 1; ++count) children.push_back(nullptr);
            };

		~Node() {
            element.clear();
            children.clear();
		}

		int get_size() { return element.size(); };
		bool full() { return element.size() == capacity; };

		std::vector<T> element;
	    std::weak_ptr<Node> parent;
		std::vector<std::shared_ptr<Node>> children;
		//size_t size;
		std::size_t capacity;
        std::size_t traverse_index = 0;
	};

	std::shared_ptr<Node> root;
	std::weak_ptr<Node> tail;
	std::size_t max_element;
    std::size_t btree_size = 0;
    mutable std::size_t search_count = 0;

};

template <typename T>
typename btree<T>::iterator btree<T>::begin() const{
    search_count = 0;
    std::weak_ptr<btree<T>::Node> cur = root;
    while (cur.lock()->children[0] != nullptr) cur = cur.lock()->children[0];
    return iterator(cur.lock(), 0, this);
}

template <typename T>
typename btree<T>::const_iterator btree<T>::cbegin() const{
    search_count = 0;
    std::weak_ptr<btree<T>::Node> cur = root;
    while (cur.lock()->children[0] != nullptr) cur = cur.lock()->children[0];
    return iterator(cur.lock(), 0, this);
}

template <typename T>
btree<T>::btree(std::size_t maxNodeElems): max_element{maxNodeElems} {}

template <typename T>
btree<T>::btree(const btree<T>& original): root{nullptr}, tail{root}, 
max_element{original.max_element} {
  std::queue<std::shared_ptr<btree<T>::Node>> copy_queue;
  copy_queue.push(original.root);
    while (copy_queue.size() != 0) {
        std::weak_ptr<btree<T>::Node> cur = copy_queue.front();
        copy_queue.pop();
        for (auto insert_elem: cur.lock()->element) insert(insert_elem);

        if (cur.lock()->full()) {
            for (auto child: cur.lock()->children) { 
                if (child != nullptr) copy_queue.push(child);
            }
        }
    }
}

template <typename T>
btree<T>::btree(btree<T>&& original):
    root(original.root),
    tail(original.tail),
    btree_size(original.btree_size),
    max_element(original.max_element) {
        original.root = nullptr;
        original.tail.reset();
        original.max_element = 0;
        original.btree_size = 0;
    }

template <typename T>
btree<T>& btree<T>::operator=(const btree<T>& rhs) {
    if (this != &rhs) {
        root.reset();
        tail.reset();
        max_element = rhs.max_element;
        btree_size = rhs.btree_size;
        std::queue<std::shared_ptr<btree<T>::Node>> copy_queue;
        copy_queue.push(rhs.root);
        while (copy_queue.size() != 0) {
            std::weak_ptr<btree<T>::Node> cur = copy_queue.front();
            copy_queue.pop();
            for (auto insert_elem: cur.lock()->element) insert(insert_elem);

            if (cur.lock()->full()) {
                for (auto child: cur.lock()->children) { 
                    if (child != nullptr) copy_queue.push(child);
                }
            }
        }
    }
    return *this;
}

template <typename T>
btree<T>& btree<T>::operator=(btree<T>&& rhs) {
    if (this != &rhs) {
        root.reset();
        tail.reset();
        max_element = rhs.max_element;
        root = rhs.root;
        tail = rhs.tail;
        max_element = rhs.max_element;
        btree_size = rhs.btree_size;

        rhs.root = nullptr;
        rhs.tail.reset();
        rhs.max_element = 0;
        rhs.btree_size = 0;
    }
}

template <typename T>
typename btree<T>::iterator btree<T>::find(const T& elem) {
    bool loop = true;
    std::weak_ptr<Node> cur = root;
    std::size_t index;
    while (loop) {
        for (index = 0; index < cur->element.size(); index++) {
            if (elem == cur.lock()->element[index]) {
                return iterator(cur.lock(), index, this);
            }else if (elem < cur.lock()->element[index]){
                if (cur.lock()->children[index] == nullptr) return end();
                cur = cur.lock()->children[index];
                loop = false;
            }
        }
        if (index == cur.lock()->element.size()) {
            if (cur.lock()->children[index] == nullptr) return end();
            cur = cur.lock()->children[index];
        }
    }
    return end();
}

template <typename T>
typename btree<T>::const_iterator btree<T>::find(const T& elem) const{
    bool loop = true;
    std::weak_ptr<Node> cur = root;
    std::size_t index;
    while (loop) {
        for (index = 0; index < cur.lock()->element.size(); index++) {
            if (elem == cur.lock()->element[index]) {
                return iterator(cur.lock(), index, this);
            }else if (elem < cur.lock()->element[index]){
                if (cur.lock()->children[index] == nullptr) return cend();
                cur = cur.lock()->children[index];
                loop = false;
            }
        }
        if (index == cur.lock()->element.size()) {
            if (cur.lock()->children[index] == nullptr) return cend();
            cur = cur.lock()->children[index];
        }
    }
    return cend();
}

template <typename T>
std::pair<typename btree<T>::iterator, bool> btree<T>::insert(const T& elem) {
    std::weak_ptr<Node> cur = root;
    bool done = true; 
    std::size_t index = 0;
    if (cur.expired()) {
        root = std::make_shared<btree<T>::Node> (Node(elem, max_element));
        tail = root;
        btree_size++;
        return std::make_pair(iterator(cur.lock(), 0, this), true);
    }
    while (index < cur.lock()->element.size()){
        if (elem == cur.lock()->element[index]) {
            return std::make_pair(iterator(cur.lock(), 0, this), false);
        }else if (elem < cur.lock()->element[index]) {
            if (cur.lock()->full()) {
                if (cur.lock()->children[index] != nullptr) {
                    cur = cur.lock()->children[index];
                    index = 0;
                    continue;
                }
                cur.lock()->children[index] = std::make_shared<btree<T>::Node> (Node(elem, max_element, cur.lock()));
                btree_size++;
                if (elem > tail.lock()->element.back()) tail = cur.lock()->children[index];
                return std::make_pair(iterator(cur.lock(), 0, this), true);
            }
            auto insert_iter = cur.lock()->element.begin();
            cur.lock()->element.insert(insert_iter + index, elem);
            btree_size++;
            return std::make_pair(iterator(cur.lock(), index, this), true);
        }
        index++;
    }
    if (index == cur.lock()->capacity){
        if (cur.lock()->children[index] == nullptr) {
            cur.lock()->children[index] = std::make_shared<Node> (Node(elem, max_element, cur.lock()));
            btree_size++;
            if (elem > tail.lock()->element.back()) tail = cur.lock()->children[index];
            return std::make_pair(iterator(cur.lock(), index, this), true);
        }
        cur = cur.lock()->children[index];
    }else{
        if (done){
            auto insert_iter = cur.lock()->element.begin();
            cur.lock()->element.insert(insert_iter + index, elem);
            btree_size++;
            return std::make_pair(iterator(cur.lock(), index, this), true);
        }
    }
    return std::make_pair(iterator(std::make_shared<Node>(), 0, this), false);
}



#endif
