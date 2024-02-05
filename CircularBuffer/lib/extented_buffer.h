#pragma once

#include <iostream>
#include <limits>
#include  "static_buffer.h"
template <typename BufferType, typename Alloc = typename std::allocator_traits<std::allocator<BufferType>>::template rebind_alloc<BufferType>>
class ExtendableBuffer : public Buffer<BufferType> {
private:
	using _MyBase = Buffer<BufferType>;
	using _MyBase::Allocate;
	using _MyBase::tail_;
	using _MyBase::head_;
	using _MyBase::size_;
	using _MyBase::capacity_;
	using _MyBase::was_destroyed_;
	using _MyBase::array_;
	using typename _MyBase::ConstIterator;
	using typename _MyBase::Iterator;
	using _MyBase::Buffer;
	using _MyBase::FillWithIterators;
	using _MyBase::FillUntilEnd;
	using _MyBase::AllocateRawArray;
	using _MyBase::Deallocate;
	using _MyBase::FillWithList;
	using _MyBase::CreateTmpForExtendableBuffer;
	using  _MyBase::RawPushBack;
	using _MyBase::m_alloc_;
	using _MyBase::Raw_erase;

	void Resize(const Alloc& alloc = Alloc()) noexcept {
		size_t tmp_capacity = capacity_;
		if (capacity_ == 0) { ++capacity_; }
		capacity_ *= 2;
		BufferType* tmp = std::allocator_traits<Alloc>::allocate(m_alloc_, capacity_);
		for (std::size_t i = 0; i < tmp_capacity; ++i) {
			std::allocator_traits<Alloc>::construct(m_alloc_, &tmp[i], array_[i]);
		}

		for (std::size_t i = tmp_capacity; i < capacity_; ++i) {
			std::allocator_traits<Alloc>::construct(m_alloc_, &tmp[i]);
		}

		for (std::size_t i = 0; i < tmp_capacity; ++i) {
			std::allocator_traits<Alloc>::destroy(m_alloc_, &array_[i]);
		}
		std::allocator_traits<Alloc>::deallocate(m_alloc_, array_, tmp_capacity);
		array_ = tmp;
	}
public:
	using _MyBase::begin;
	using _MyBase::end;

	~ExtendableBuffer() noexcept override = default;

	/**
	 * \brief Changes buffer to match the content of other buffer. If other buffer is bigger than source it will be resized  
	 */
	ExtendableBuffer& operator=(const ExtendableBuffer<BufferType>& other) noexcept {
		while (capacity_ < other.capacity_) {
			Resize();
		}
		_MyBase::operator=(other);
		return *this;
	}

	/**
	 * \brief Push one value into start of buffer. If we got overflowing,then buffer will be resized
	 * \param item what will be pushed at front
	 */
	void PushFront(const BufferType& item) noexcept override {
		if (size_ + 1 > capacity_) {
			Resize();
		}
		_MyBase::PushFront(item);
	}

	/**
	 * \brief Push one value into back of buffer. If we got overflowing,then buffer will be resized
	 * \param item what will be pushed at back
	 */
	void PushBack(const BufferType& item) noexcept override {
		if (size_ + 1 > capacity_) {
			Resize();
		}
		_MyBase::PushBack(item);
	}

	/**
	 * \brief Return max size of buffer,since it's resizeable it will match max of size_t
	 */
	size_t MaxSize() const noexcept override {
		return std::numeric_limits<size_t>::max();
	}

	/**
	 * \brief Inserts a copy of what before where for how_many times
	 * \param where it will be inserted
	 * \param how_many times it will be inserted
	 * \param what will be inserted
	 * \return iterator points to first element that  was inserted
	 */
	ConstIterator Insert(const ConstIterator& where, size_t how_many, const BufferType& what) noexcept {
		size_t idx = 0;
		size_t where_idx = 0;
		BufferType* tmp = CreateTmpForExtendableBuffer(where, how_many, idx, where_idx);

		for (size_t i = 0; i < how_many; ++i) {
			std::allocator_traits<Alloc>::construct(m_alloc_, &tmp[idx], what);
			++idx;
			++tail_;
		}

		FillUntilEnd(where, idx, tmp, how_many);
		return Iterator(&array_[where_idx]);
	}

	/**
	 * \brief Inserts a copy of what before where
	 * \param where it will be inserted
	 * \param what will be inserted
	 * \return iterator points to where it was inserted
	 */
	ConstIterator Insert(const ConstIterator& where, BufferType what) noexcept {
		return Insert(where, 1, what);
	}


	/**
 * \brief Do same as previous,but inserts elements in range of two iterators:from_where
 * to until_where
 * \param where it will be inserted
 * \param from_where starts of sequence
 * \param until_where end of sequence
 * \return iterator points to first element that  was inserted
 */
	ConstIterator Insert(const ConstIterator& where, const ConstIterator& from_where, const ConstIterator& until_where) noexcept {
		size_t idx = 0;
		size_t where_idx = 0;
		BufferType* tmp = CreateTmpForExtendableBuffer(where, std::distance(from_where, until_where), idx, where_idx);
		FillWithIterators(from_where, until_where, tmp, idx);
		FillUntilEnd(where, idx, tmp, std::distance(from_where, until_where));
		return Iterator(&array_[where_idx]);
	}

	/**
	 * \brief Do same as previous but inserts values at std::initializer_list
	 * \param where it will be inserted
	 * \param list of values
	 * \return iterator points to first element that  was inserted
	 */
	ConstIterator Insert(const ConstIterator& where, const std::initializer_list<BufferType>& list) noexcept {
		size_t idx = 0;
		size_t where_idx = 0;
		BufferType* tmp = CreateTmpForExtendableBuffer(where, list.size(), idx, where_idx);
		FillWithList(list, tmp, idx);
		FillUntilEnd(where, idx, tmp, list.size());
		return Iterator(&array_[where_idx]);
	}

	


	/**
	 * \brief Erases  one element that being pointed by where
	 * \param where element that will erased
	 * \param amount_of_skipped_elements how many elements will be skipped
	 * \return Returned iterator points at the element that was immediately following where prior to erasure 
	 */
	ConstIterator Erase(const ConstIterator& where, size_t amount_of_skipped_elements = 1) noexcept override {
		size_t idx = 0;
		size_t where_idx = where - begin();
		size_t tmp_size = 0;
		BufferType* tmp = CreateTmpForExtendableBuffer(where, size_-amount_of_skipped_elements, idx, where_idx);
		Raw_erase(idx, where_idx, tmp_size, where, amount_of_skipped_elements, tmp);
		
		return Iterator(&array_[where_idx]);
	}

	/**
	 * \brief Erases elements in range of from_where to until_where
	 * \param from_where iterator,points at place,where the erasing starts
	 * \param until_where iterator,points at place,where the erasing ends
	 *@warning Last element of this group doesn't count. It means that element in until_where
	 *wont'be deleted
	 * \return iterator at the element that was pointed by until_where
	*/
	ConstIterator Erase(const ConstIterator& from_where, const ConstIterator& until_where) noexcept  override {
		return Erase(from_where, std::distance(from_where, until_where));
	}
};
