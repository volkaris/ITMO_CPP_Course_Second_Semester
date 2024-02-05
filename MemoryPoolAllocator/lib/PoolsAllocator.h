#pragma once

#include "Pool.h"

#include <stack>
#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <utility>


template <typename T>
class ManyPoolsAllocator : std::allocator<T> {
public:
	using value_type = T;
	using pointer = T*;
	using size_type = std::size_t;
	using is_always_equal = std::false_type;
	using propagate_on_container_copy_assignment = std::false_type;
	using propagate_on_container_move_assignment = std::false_type;
	using propagate_on_container_swap = std::false_type;



	template <typename ... Args>
	ManyPoolsAllocator(size_t amount_of_args, Args... args) {
		std::vector<int> vec{ args... };
		for (size_type i = 0; i < vec.size(); i += 2)
		{
			pools.push_back({ std::make_shared<pool>(vec[i], vec[i + 1]) });	  //first=pool size,second-chunk size
		}

	}
	ManyPoolsAllocator() = default;

	~ManyPoolsAllocator() = default;

	template <typename U>
	ManyPoolsAllocator(const ManyPoolsAllocator<U>& other) noexcept {
		pools = other.pools;
	}

	ManyPoolsAllocator(ManyPoolsAllocator&& other) noexcept {
		pools = std::move(other.pools);
	}

	ManyPoolsAllocator& operator=(ManyPoolsAllocator&& other) noexcept {
		pools = std::move(other.pools);

		return *this;
	}

	ManyPoolsAllocator(const ManyPoolsAllocator& other) noexcept {
		pools = other.pools;
	}

	ManyPoolsAllocator& operator=(const ManyPoolsAllocator& other) noexcept {
		pools = other.pools;

		return *this;
	}

	pointer allocate(size_type n) {

		if (first_time) {
			for (const auto& i : pools) {
				i->AllocateIfNeccesary();
			}
			first_time = false;
		}

		for (const auto& i : pools) {
			if (i->chunk_size_ >= sizeof(T) * n  and !i->m_addrs.empty()) {
				from_where_we_take.push(i);
				return static_cast<pointer>(i->ReturnPointer());
			}
		}
		throw std::bad_alloc();
	}

	void deallocate(pointer ptr, size_type n) {

		for (const auto& i : pools) {
			if (!from_where_we_take.empty() && i == from_where_we_take.top()) {
				i->Deallocate(ptr);
				from_where_we_take.pop();
				return;
			}
		}
	}

	bool first_time = true;
	std::vector<std::shared_ptr<pool>> pools;
	std::stack<std::shared_ptr<pool>> from_where_we_take;

	template <typename T1, typename T2>
	friend bool operator==(const ManyPoolsAllocator<T1>& lhs, const ManyPoolsAllocator<T2>& rhs);

	template <typename T1, typename T2>
	friend bool operator!=(const ManyPoolsAllocator<T1>& lhs, const ManyPoolsAllocator<T2>& rhs);

	template <typename U>
	friend class myallocator;
};

template <typename T1, typename T2>
bool operator==(const ManyPoolsAllocator<T1>& lhs, const ManyPoolsAllocator<T2>& rhs) {
	return true;
}

template <typename T1, typename T2>
bool operator!=(const ManyPoolsAllocator<T1>& lhs, const ManyPoolsAllocator<T2>& rhs) {
	return false;
}

