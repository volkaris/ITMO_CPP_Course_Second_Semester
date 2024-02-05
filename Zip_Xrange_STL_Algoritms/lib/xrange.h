#pragma once

#include <stdexcept>

template <typename StartEndType>
class xrange {
	signed long long step_ = 1;
	StartEndType current_value_{0};
	StartEndType end_;
	class xrange_iterator;

public:
	explicit xrange(const StartEndType& start, const StartEndType& end) : current_value_(start), end_(end) {}

	explicit xrange(const StartEndType& end) : end_(end) {}

	explicit xrange(const StartEndType& start, const StartEndType& end, signed long long step) : step_(step),
		current_value_(start),
		end_(end) {
		if (step_ == 0) {
			throw std::invalid_argument("Step can't be zero");
		}
	}

	xrange_iterator begin() {
		return xrange_iterator(current_value_, *this);
	}

	xrange_iterator end() {
		return xrange_iterator(end_, *this);
	}

private:
	class xrange_iterator {
	public:
		explicit xrange_iterator(StartEndType current_value, const xrange& xr) : object_(xr),
			current_value_(current_value) {}

		xrange_iterator operator++(int) {
			xrange_iterator tmp = *this;
			++(*this);
			return tmp;
		}

		xrange_iterator& operator++() {
			current_value_ += object_.step_;
			return *this;
		}

		StartEndType& operator*() {
			return current_value_;
		}

		xrange_iterator operator+(long long int value) {
			auto tmp = *this;
			return tmp += value;
		}

		xrange_iterator& operator+=(long long int amount) {
			current_value_ += object_.step_ * amount;

			if ((object_.step_ > 0 and current_value_ >= object_.end_) or (object_.step_ < 0 and current_value_ <=
				object_.end_)) {
				throw std::out_of_range("Iterator out of bounds");
			}

			return *this;
		}

		bool operator!=(const xrange_iterator& other) {
			return !operator==(other);
		}

		bool operator==(const xrange_iterator& other) {
			if (object_.step_ < 0) {
				return current_value_ == other.current_value_ or current_value_ < other.current_value_;
			}
			if (object_.step_ > 0) {
				return current_value_ == other.current_value_ or current_value_ > other.current_value_;
			}
		}

	private:
		xrange object_;
		StartEndType current_value_;
	};
};
