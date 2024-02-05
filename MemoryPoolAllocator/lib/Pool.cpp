#include "Pool.h"


pool::pool(size_type size, size_type chunk_size) : chunk_size_(chunk_size), pool_size{ size } {}

void* pool::AllocateAndReturn() {
	if (m_addrs.empty()) {
		this->AddMemoryAddreses();
	}

	auto ptr = m_addrs.top();
	m_addrs.pop();


	return ptr;
}

void pool::AllocateIfNeccesary() {
	if (m_addrs.empty()) {
		this->AddMemoryAddreses();
	}
}

void* pool::ReturnPointer() {
	auto ptr = m_addrs.top();
	m_addrs.pop();

	return ptr;
}

void pool::Deallocate(void* ptr) {
	m_addrs.push(ptr);
}

void pool::Rebind(size_type size) {
	chunk_size_ = size;
}

void pool::AddMemoryAddreses() {
	auto block = std::make_unique<uint8_t[]>(pool_size);
	auto block_ptr = block.get();

	size_type total_size = pool_size % chunk_size_ == 0 ? pool_size : pool_size - (pool_size % chunk_size_);
	for (size_type i = 0; i < total_size; i += chunk_size_) {
		m_addrs.push(block_ptr + i);
	}

	m_blocks.push(std::move(block));
}