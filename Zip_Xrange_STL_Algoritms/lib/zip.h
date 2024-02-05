#pragma once

#include <utility>

template <typename T1, typename T2>
class zip {

	const T1& first_pointer_;
	const T2& second_pointer_;
	class zipper_iterator;

public:
	zip(const T1& t1, const T2& t2) : first_pointer_(t1),
	                                  second_pointer_(t2) {}

	zipper_iterator begin() {
		return zipper_iterator(first_pointer_.begin(),
		                       second_pointer_.begin());
	}

	zipper_iterator end() {
		return zipper_iterator(first_pointer_.end(), second_pointer_.end());
	}

	class zipper_iterator {
		typename T1::const_iterator first_iterator_;
		typename T2::const_iterator second_iterator_;
		std::pair<typename T1::value_type, typename T2::value_type>
		current_pair_;

	public:
		zipper_iterator(typename T1::const_iterator t1,
		                 typename T2::const_iterator t2) :
			first_iterator_(t1), second_iterator_(t2) {}

		zipper_iterator operator++(int) {
			zipper_iterator tmp = *this;
			++(*this);
			return tmp;
		}

		zipper_iterator& operator++() {
			std::advance(first_iterator_, 1);
			std::advance(second_iterator_, 1);
			return *this;
		}

		std::pair<typename T1::value_type, typename T2::value_type>&
		operator*() {
			current_pair_ = std::make_pair(*first_iterator_, *second_iterator_);
			return current_pair_;
		}


		bool operator!=(const zipper_iterator& other) {
			return first_iterator_ != other.first_iterator_ and
				second_iterator_ != other.second_iterator_;
		}

		bool operator==(const zipper_iterator& other) {
			return first_iterator_ == other.first_iterator_ and
				second_iterator_ == other.second_iterator_;
		}

		zipper_iterator operator+(int amount) const noexcept {
			auto tmp = *this;
			return tmp += amount;
		}

		zipper_iterator& operator+=(int amount) {
			std::advance(first_iterator_, amount);
			std::advance(second_iterator_, amount);
			return *this;
		}
	};
};
