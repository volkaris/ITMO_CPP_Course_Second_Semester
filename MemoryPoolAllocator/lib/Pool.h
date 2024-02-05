#pragma once
#include <cstdint>
#include <memory>
#include <stack>

class pool {
public:
	using size_type = std::size_t;

	template <typename W> friend class 	ManyPoolsAllocator;
	template <typename Q> friend class 	SinglePoolAllocator;
	pool(size_type size, size_type chunk_size);

	void* AllocateAndReturn();

	void AllocateIfNeccesary();

	void* ReturnPointer();


	void Deallocate(void* ptr);

	void Rebind(size_type size);

private:

	void AddMemoryAddreses();

	std::stack<void*> m_addrs{};
	std::stack<std::unique_ptr<uint8_t[]>> m_blocks{};
	size_type chunk_size_;
	size_type pool_size;
	size_type m_size = 0;
};