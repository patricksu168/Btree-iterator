#ifndef BTREE_ITERATOR_H
#define BTREE_ITERATOR_H

#include <iterator>

template <typename T> class btree;
template <typename T> class const_btree_iterator;

template <typename T>
class btree_iterator {
public:
	typedef std::ptrdiff_t            difference_type;
	typedef std::bidirectional_iterator_tag iterator_category;
	typedef T                         value_type;
	typedef T*                        pointer;
	typedef T&                        reference;
	friend class const_btree_iterator<T>;

	reference operator*() const;
	pointer operator->() const{ return &(operator*()); }
	btree_iterator& operator++();
	btree_iterator operator++(int);
	btree_iterator& operator--();
	btree_iterator operator--(int);
	btree_iterator& operator=(const btree_iterator& rhs);
	bool operator==(const btree_iterator& other) const;
	bool operator!=(const btree_iterator& other) const{ return !operator==(other); }

	//constructor
	btree_iterator(std::shared_ptr<typename btree<T>::Node> node = std::make_shared<btree<T>::Node>(), 
		size_t idx = 0, const btree<T>* btr = nullptr): 
		pointee{node},
		index{idx},
		bt{btr} {};


private: 
	std::weak_ptr<typename btree<T>::Node> pointee;
	size_t index;
	const btree<T> *bt;
};

template <typename T>
class const_btree_iterator {
public:
	typedef std::ptrdiff_t            difference_type;
	typedef std::bidirectional_iterator_tag iterator_category;
	typedef T                         value_type;
	typedef const T*                        pointer;
	typedef const T&                        reference;
	friend class btree_iterator<T>;

	reference operator*() const;
	pointer operator->() const{ return &(operator*()); }
	const_btree_iterator& operator++();
	const_btree_iterator operator++(int);
	const_btree_iterator& operator--();
	const_btree_iterator operator--(int);
	bool operator==(const const_btree_iterator& other) const;
	bool operator!=(const const_btree_iterator& other) const{ return !operator==(other); }

	//constructor
	const_btree_iterator(std::shared_ptr<typename btree<T>::Node> node = std::make_shared<btree<T>::Node> (), 
		size_t idx = 0, const btree<T>* btr = nullptr): 
		pointee{node},
		index{idx},
		bt{btr} {};

	const_btree_iterator(const btree_iterator<T>& rhs):
		pointee{rhs.pointee.lock()},
		index{rhs.index},
		bt{rhs.bt} {};

private: 
	std::weak_ptr<typename btree<T>::Node> pointee;
	std::size_t index;
	const btree<T> *bt;
};

/**
 * You MUST implement the btree iterators as (an) external class(es) in this file.
 * Failure to do so will result in a total mark of 0 for this deliverable.
 **/

// iterator related interface stuff here; would be nice if you called your
// iterator class btree_iterator (and possibly const_btree_iterator)
template <typename T>
typename btree_iterator<T>::reference btree_iterator<T>::operator*() const {
	return pointee.lock()->element[index];
}

template <typename T>
btree_iterator<T>& btree_iterator<T>::operator++() {
	bt->search_count++;
	if (pointee.lock()->children[index + 1] != nullptr) {
		pointee.lock()->traverse_index++;
		pointee = pointee.lock()->children[index + 1];
		index = 0;
	}else{
		if (index == pointee.lock()->element.size() - 1) {
			if (bt->search_count == bt->btree_size) {
				while (!pointee.expired()) {
					pointee.lock()->traverse_index = 0;
					pointee = pointee.lock()->parent;
					index = 0;
				}
			}else{
				pointee.lock()->traverse_index = 0;
				pointee = pointee.lock()->parent;
				index = pointee.lock()->traverse_index;
			}
		}else{
			pointee.lock()->traverse_index++;
			index++;
		}
	}
	return *this;
}

template <typename T>
btree_iterator<T> btree_iterator<T>::operator++(int) {
	btree_iterator tmp = *this;
	operator ++();
	return tmp;
}

template <typename T>
btree_iterator<T>& btree_iterator<T>::operator--() {
	bt->search_count++;
	if (pointee.lock()->children[index] != nullptr) {
		pointee.lock()->traverse_index--;
		pointee = pointee.lock()->children[index];
		index = 0;
	}else{
		if (index == 0) {
			if (bt->search_count == bt->btree_size) {
				while (!pointee.expired()) {
					pointee.lock()->traverse_index = pointee.lock()->element.size();
					pointee = pointee.lock()->parent;
				}
				index = 0;
			}else{
				pointee.lock()->traverse_index = pointee.lock()->element.size();
				pointee = pointee.lock()->parent;
				index = pointee.lock()->traverse_index;
			}
		}else{
			pointee.lock()->traverse_index -= 1;
			index -= 1;
		}
	}
	return *this;
}

template <typename T>
btree_iterator<T> btree_iterator<T>::operator--(int) {
	btree_iterator tmp = *this;
	operator --();
	return tmp;
}

template <typename T>
btree_iterator<T>& btree_iterator<T>::operator=(const btree_iterator& rhs) {
	if (this != &rhs){
		pointee = rhs.pointee;
		index = rhs.index;
		bt = rhs.index;
	}
	return *this;
}

template <typename T>
bool btree_iterator<T>::operator==(const btree_iterator<T>& rhs) const {
	return (bt == rhs.bt && pointee.lock() == rhs.pointee.lock() && index == rhs.index);
}

template <typename T>
typename const_btree_iterator<T>::reference const_btree_iterator<T>::operator*() const {
	return pointee.lock()->element[index];
}

template <typename T>
const_btree_iterator<T>& const_btree_iterator<T>::operator++() {
	bt->search_count++;
	if (pointee.lock()->children[index + 1] != nullptr) {
		pointee.lock()->traverse_index++;
		pointee = pointee.lock()->children[index + 1];
		index = 0;
	}else{
		if (index == pointee.lock()->element.size() - 1) {
			if (bt->search_count == bt->btree_size) {
				while (!pointee.expired()) {
					pointee.lock()->traverse_index = 0;
					pointee = pointee.lock()->parent;
					index = 0;
				}
			}else{
				pointee.lock()->traverse_index = 0;
				pointee = pointee.lock()->parent;
				index = pointee.lock()->traverse_index;
			}
		}else{
			pointee.lock()->traverse_index++;
			index++;
		}
	}
	return *this;
}

template <typename T>
const_btree_iterator<T> const_btree_iterator<T>::operator++(int) {
	const_btree_iterator tmp = *this;
	operator ++();
	return tmp;
}

template <typename T>
const_btree_iterator<T>& const_btree_iterator<T>::operator--() {
	bt->search_count++;
	if (pointee.lock()->children[index] != nullptr) {
		pointee.lock()->traverse_index--;
		pointee = pointee.lock()->children[index];
		index = 0;
	}else{
		if (index == 0) {
			if (bt->search_count == bt->btree_size) {
				while (!pointee.expired()) {
					pointee.lock()->traverse_index = pointee.lock()->element.size();
					pointee = pointee.lock()->parent;
					index = 0;
				}
			}else{
				pointee.lock()->traverse_index = pointee.lock()->element.size();
				pointee = pointee.lock()->parent;
				index = pointee.lock()->traverse_index;
			}
		}else{
			pointee.lock()->traverse_index--;
			index--;
		}
	}
	return *this;
}

template <typename T>
const_btree_iterator<T> const_btree_iterator<T>::operator--(int) {
	const_btree_iterator tmp = *this;
	operator --();
	return tmp;
}


template <typename T>
bool const_btree_iterator<T>::operator==(const const_btree_iterator<T>& rhs) const {
	return (bt == rhs.bt && pointee.lock() == rhs.pointee.lock() && index == rhs.index);
}

#endif
