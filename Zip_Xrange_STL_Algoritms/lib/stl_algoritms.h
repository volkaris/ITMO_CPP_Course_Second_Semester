#pragma once

#include <iterator>

namespace stl_algorithms {
	template <typename InputIt, typename UnaryPredicate>
	constexpr bool all_of(InputIt first, InputIt last, UnaryPredicate p) {
		for (auto i = first; i != last; ++i) {
			if (!p(*i)) {
				return false;
			}
		}

		return true;
	}

	template <typename InputIt, typename UnaryPredicate>
	constexpr bool any_of(InputIt first, InputIt last, UnaryPredicate p) {
		for (auto i = first; i != last; ++i) {
			if (p(*i)) {
				return true;
			}
		}

		return false;
	}

	template <typename InputIt, typename UnaryPredicate>
	constexpr bool none_of(InputIt first, InputIt last, UnaryPredicate p) {
		return !stl_algorithms::any_of(first, last, p);
	}

	template <typename InputIt, typename UnaryPredicate>
	constexpr bool one_of(InputIt first, InputIt last, UnaryPredicate p) {
		bool only_one = false;
		for (auto i = first; i != last; ++i) {
			if (p(*i) and only_one == false) {
				only_one = true;
			}
			else if (p(*i) and only_one == true) {
				return false;
			}
		}

		return only_one;
	}

	template <typename InputIt, typename ValueType>
	constexpr InputIt find_not(InputIt first, InputIt last, const ValueType& p) {
		for (auto i = first; i != last; ++i) {
			if (*i != p) {
				return i;
			}
		}
		return last;
	}

	template <typename BidirectionalIt, typename ValueType>
	constexpr BidirectionalIt find_backward(BidirectionalIt first, BidirectionalIt last, const ValueType& p) {
		if (last != first) {
			auto tmp = last;
			while (--tmp != first) {
				if (*tmp == p) {
					return tmp;
				}
			}
			
		}
		return last;
	}

	template <typename BidirectionalIt, typename Comp>
	constexpr bool is_sorted(BidirectionalIt first, BidirectionalIt last, Comp p) {
		for (auto i = first; i != last - 1; ++i) {
			auto current = *i;
			auto next = *(i + 1);

			if (current != next and !p(current, next)) {
				return false;
			}
		}
		return true;
	}

	template <typename ForwardIt>
	constexpr bool is_sorted(ForwardIt first, ForwardIt last) {
		return stl_algorithms::is_sorted(first, last, std::less<>{});
	}

	template <typename ForwardIt, typename Comp>
	constexpr bool is_partitioned(ForwardIt first, ForwardIt last, Comp p) {
		for (;; ++first) {
			if (first == last) {
				return true;
			}

			if (!p(*first)) {
				break;
			}
		}

		while (++first != last) {
			if (p(*first)) {
				return false;
			}
		}

		return true;
	}

	template <typename BidirectionalIt, typename ValueType>
	constexpr bool is_palindrome(BidirectionalIt first, BidirectionalIt last, const ValueType& p) {
		auto tmp_start = first;
		auto temp_end = last;

		size_t size = ceil((static_cast<double>(last - first)) / 2);
		size_t counter = 0;

		for (auto& i = first; i != last and counter != size; ++i) {
			if (!((p(*tmp_start) and p(*std::prev(temp_end))) or (!p(*tmp_start) and !p(*std::prev(temp_end))))) {
				return false;
			}

			std::advance(tmp_start, 1);
			std::advance(temp_end, -1);
			++counter;
		}
		return true;
	}
}

