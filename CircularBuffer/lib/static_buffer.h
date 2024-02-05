#pragma once

#include <compare>
#include <ostream>
#include <stdexcept>

constexpr bool kIndexOffset = true;

template <typename BufferType, typename Alloc = typename std::allocator_traits<std::allocator<BufferType>>::template rebind_alloc<BufferType>>
class Buffer {
	/*! \cond PRIVATE */
protected:
	class ConstIterator;
	class Iterator;
	size_t tail_ = 0;
	size_t head_ = 0;
	size_t size_ = 0;
	size_t capacity_;
	bool was_destroyed_ = false;
	BufferType* array_;
	Alloc m_alloc_;

	BufferType* AllocateRawArray(size_t how_many) noexcept {
		BufferType* where_to_allocate = std::allocator_traits<Alloc>::allocate(m_alloc_, how_many);

		for (auto i = 0; i < how_many; ++i) {
			std::allocator_traits<Alloc>::construct(m_alloc_, &where_to_allocate[i]);
		}
		return where_to_allocate;
	}

	void Deallocate(size_t how_many, BufferType* what) noexcept {
		for (std::size_t i = 0; i < how_many; ++i) {
			std::allocator_traits<Alloc>::destroy(m_alloc_, &what[i]);
		}

		std::allocator_traits<Alloc>::deallocate(m_alloc_, what, how_many);
	}

	void Raw_erase(size_t& idx, size_t& where_idx, size_t& tmp_size, const ConstIterator& where, size_t& amount_of_skipped_elements, BufferType*& tmp) {
		for (size_t i = idx; i < capacity_; ++i) {
			std::allocator_traits<Alloc>::construct(m_alloc_, &tmp[i]);
		}

		for (auto i = where + amount_of_skipped_elements; i != end(); ++i) {
			RawPushBack(tmp, head_, capacity_, idx, tmp_size, *i);
		}

		Deallocate(capacity_, array_);
		tail_ = idx;
		size_ = tmp_size;
		array_ = tmp;
	}

	void AssignArray(BufferType* tmp, size_t amount) noexcept {
		Deallocate(capacity_, array_);

		size_ = amount;
		tail_ = amount;
		capacity_ = amount;
		array_ = AllocateRawArray(amount);

		for (auto i = 0; i < amount; ++i) {
			array_[i] = std::move(tmp[i]);
		}

		Deallocate(amount, tmp);
	}

	void FillWithIterators(const ConstIterator& from_where, const ConstIterator& until_where, BufferType*& tmp, size_t& idx) noexcept {
		for (auto i = from_where; i != until_where; ++i) {
			std::allocator_traits<Alloc>::construct(m_alloc_, &tmp[idx], *i);
			++idx;
			++tail_;
		}
	}

	BufferType* CreateTmpForExtendableBuffer(const ConstIterator& where, size_t how_many, size_t& idx, size_t& tmp_size) noexcept {
		BufferType* tmp;

		if (how_many + size_ <= capacity_) {
			tmp = std::allocator_traits<Alloc>::allocate(m_alloc_, capacity_);
		} else {
			tmp = std::allocator_traits<Alloc>::allocate(m_alloc_, how_many + size_);
		}
		Construct(where, idx, tmp, tmp_size);

		return tmp;
	}

	void Construct(const ConstIterator& where, size_t& idx, BufferType*& tmp, size_t& tmp_size) noexcept {
		for (auto i = begin(); i != where; ++i) {
			std::allocator_traits<Alloc>::construct(m_alloc_, &tmp[idx], array_[idx]);
			++idx;
			++tmp_size;
		}
	}

	BufferType* CreateTmpForStaticBuffer(const ConstIterator& where, size_t& idx, size_t& tmp_size) noexcept {
		BufferType* tmp;
		tmp = std::allocator_traits<Alloc>::allocate(m_alloc_, capacity_);
		Construct(where, idx, tmp, tmp_size);
		return tmp;
	}


	void FillUntilEnd(const ConstIterator& where, size_t& idx, BufferType*& tmp, size_t how_many) noexcept {
		for (auto i = where; i != end(); ++i) {
			std::allocator_traits<Alloc>::construct(m_alloc_, &tmp[idx], *i);

			++idx;
		}

		size_ += how_many;

		for (std::size_t i = 0; i < capacity_; ++i) {
			std::allocator_traits<Alloc>::destroy(m_alloc_, &array_[i]);
		}

		std::allocator_traits<Alloc>::deallocate(m_alloc_, array_, capacity_);
		capacity_ = size_;
		array_ = tmp;
	}

	void FillWithList(const std::initializer_list<BufferType>& list, BufferType*& tmp, size_t& idx) noexcept {
		for (auto i : list) {
			std::allocator_traits<Alloc>::construct(m_alloc_, &tmp[idx], i);
			++idx;
			++tail_;
		}
	}

	void Allocate(size_t size, const Alloc& alloc = Alloc()) noexcept {
		capacity_ = size;
		m_alloc_ = alloc;
		array_ = m_alloc_.allocate(size);
		for (std::size_t i = 0; i < size; ++i)
			std::allocator_traits<Alloc>::construct(m_alloc_, &array_[i]);
	}

	void SetFields(size_t tail_, size_t head_, size_t size_) noexcept {
		this->head_ = head_;
		this->tail_ = tail_;
		this->size_ = size_;
	}

	void RawPushBack(BufferType*& array, size_t& head, size_t& capacity, size_t& tail, size_t& size, BufferType item) noexcept {
		if (head == capacity)
			head = 0;
		if (tail < capacity) {
			array[tail] = item;
			++tail;
			if (size < capacity)
				++size;
		} else {
			tail = 0;
			array[tail] = item;
			++tail;
		}
	}

	/*! \endcond */

public:
	/**
	 * \brief Default copy consturctor
	 * \param other const-reference to previously created buffer
	 * \param alloc Default std::allocator. Every constructor has this,but it won' be
	 * mentioned in next definitions
	 */
	Buffer(const Buffer& other, const Alloc& alloc = Alloc()) noexcept {
		Allocate(other.capacity_);
		SetFields(other.tail_, other.head_, other.size_);
		for (size_t i = 0; i < other.Size(); ++i)
			array_[i] = other.array_[i];
	}

	/**
	 * \brief 
	 * \param size With this size buffer will be created,it can'be changed,
	 * size must be in range of size_t
	 */
	Buffer(size_t size, const Alloc& alloc = Alloc()) noexcept {
		Allocate(size);
	}

	/**
	 * \brief Creates empty buffer. In fact,you cant't interact with this somehow
	 * 
	 */
	Buffer(const Alloc& alloc = Alloc()) noexcept { Allocate(0); }

	/**
	 * \brief Creates buffer with size equals count,filled with values
	 * \param count size of buffer
	 * \param val value with which buffer will be filled
	 */
	Buffer(size_t count, BufferType val,
		   const Alloc& alloc = Alloc()) noexcept {
		Allocate(count);
		for (auto i = 0; i < count; ++i)
			Buffer::PushBack(val);
	}

	/**
	 * \brief Creates buffer with size equals size of std::initializer_list
	 * \param initializer_list standard initializer_list
	 */
	Buffer(std::initializer_list<BufferType> initializer_list,
		   const Alloc& alloc = Alloc()) noexcept {
		Allocate(initializer_list.size());
		for (auto value : initializer_list)
			Buffer::PushBack(value);
	}

	/**
	 * \brief Creates buffer with size of std::distance(begin,end) filled with item
	 * that in this range
	 * \param begin const/non-const iterator where sequence starts
	 * \param end  const/non-const iterator where sequence end
	 */
	Buffer(ConstIterator begin, ConstIterator end) noexcept {
		Allocate(std::distance(begin, end));

		for (auto i = begin; i != end; ++i)
			Buffer::PushBack(*i);
	}


	/**
	 * \brief Destructor with defence from user. Due to protection,it wont't throw access violating memory if user tries to clear (reference below)
	 * same buffer for more than one time
	 */
	/// @see clear
	virtual ~Buffer() noexcept {
		if (was_destroyed_)
			return;
		Clear();
		m_alloc_.deallocate(array_, capacity_);
		was_destroyed_ = true;
	}

	/**
	 * \brief Fill the buffer with values in std::initializer_list
	 */
	virtual Buffer& operator=(std::initializer_list<BufferType> list) noexcept {
		for (auto value : list)
			Buffer::PushBack(value);
		return *this;
	}

	/**
	 * \brief Assignment operator,conducting deep copying of object 
	 */
	Buffer& operator=(const Buffer& other) noexcept {
		if (this != std::addressof(other)) {
			SetFields(other.tail_, other.head_, other.size_);
			for (size_t i = 0; i < other.Size(); ++i)
				array_[i] = other.array_[i];
		}
		return *this;
	}

	/**
	 * \brief Push element at the from of the buffer with respect of circular condition
	 * \param item what will be placed at front of buffer
	 */
	virtual void PushFront(const BufferType& item) noexcept {
		if (size_ < capacity_)
			++size_;

		for (auto i = size_ - 1; i > 0; --i)
			array_[i] = array_[i - 1];

		array_[0] = item;
	}


	/**
	 * \brief Push element at the back of the buffer with respect of circular condition
	 * \param item what will be placed at back of buffer
	 */
	virtual void PushBack(const BufferType& item) noexcept {
		RawPushBack(array_, head_, capacity_, tail_, size_, item);
	}

	/**
	 * \brief Deleting last element in buffer
	 * @warning Will throw exception if buffer is empty
	 */
	void PopBack() {
		if (Empty())
			throw std::out_of_range("Pop back from Empty buffer");
		if (tail_ > 0)
			tail_--;
		size_--;
	}

	/**
	 * \brief Deleting first element in buffer
	 * @warning Will throw exception if buffer is empty
	 */
	void PopFront() {
		if (Size() == 0)
			throw std::out_of_range("Pop front from Empty buffer");
		head_++;
	}

	/**
	 * \brief Swaps elements in 2 buffers
	 * \param right Buffer that will be swapped with another buffer
	 */
	void Swap(Buffer<BufferType>& right) noexcept {
		if (this != std::addressof(right)) {
			auto tmp = *this;
			*this = right;
			right = tmp;
		}
	}

	/**
	 * \brief Returns value placed at idx
	 */
	BufferType& operator [](size_t idx) const noexcept {
		return array_[idx];
	}

	/**
	 * \brief Return max capacity of buffer
	 * \return As this buffer can't to change it's capacity,returns capacity
	 * provided in time of creation
	 */
	virtual size_t MaxSize() const noexcept {
		return capacity_;
	}

	/**
	 * \brief Returns value at idx only if this value exists
	 * @warning Can throw exception if value doesn't
	 * exists or idx is less than zero
	 */
	BufferType& At(size_t idx) const {
		if (idx > capacity_ || idx < 0)
			throw std::out_of_range("Index out of range or less than zero");

		return array_[idx];
	}

	/**
	 * \brief Clear the buffer. Doesn't destruct object,so you can continue the same buffer after cleaning.
	 */
	void Clear() noexcept {
		for (std::size_t i = 0; i < capacity_; ++i) {
			std::allocator_traits<Alloc>::destroy(m_alloc_, &array_[i]);
		}
		SetFields(0, 0, 0);
	}

	/**
	 * \brief Erases  one element that being pointed by iterator
	 * \param where the element will be erased
	 * \return Returned iterator points at the element that was immediately following where prior to erasure
	 */
	virtual ConstIterator Erase(const ConstIterator& where, size_t amount_of_skipped_elements = 1) noexcept {
		size_t idx = 0;
		size_t where_idx = where - begin();
		size_t tmp_size = 0;
		BufferType* tmp = CreateTmpForStaticBuffer(where, idx, tmp_size);

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
	virtual ConstIterator Erase(const ConstIterator& from_where, const ConstIterator& until_where) noexcept {
		return Erase(from_where, std::distance(from_where, until_where));
	}

	/**
	 * \brief Changes values in buffer to values in from_where to until_where range
	 */
	virtual void Assign(const ConstIterator& from_where, const ConstIterator& until_where) noexcept {
		size_t where_idx = 0;
		size_t new_size = std::distance(from_where, until_where);

		BufferType* tmp = AllocateRawArray(new_size);

		for (auto i = from_where; i != until_where; ++i) {
			tmp[where_idx] = *i;
			++where_idx;
		}

		AssignArray(tmp, new_size);
	}

	/**
	 * \brief Changes values in buffer to values in std::initializer_list
	 */
	virtual void Assign(const std::initializer_list<BufferType>& list) noexcept {
		size_t where_idx = 0;
		size_t new_size = list.size();

		BufferType* tmp = AllocateRawArray(new_size);

		for (auto i : list) {
			tmp[where_idx] = i;
			++where_idx;
		}

		AssignArray(tmp, new_size);
	}

	/**
	 * \brief Changes values in buffer.
	 * \param amount new capacity of buffer
	 * \param item what buffer will consists of
	 */
	virtual void Assign(size_t amount, const BufferType& item) noexcept {
		BufferType* tmp = AllocateRawArray(amount);

		for (size_t i = 0; i < amount; ++i) {
			tmp[i] = item;
		}

		AssignArray(tmp, amount);
	}

	/**
	/**
	 * \return Return size of buffer
	 */
	size_t Size() const noexcept { return size_ - head_; }

	/**
	 * \return Returns whether buffer is empty or not
	 */
	bool Empty() const noexcept { return (begin() == end()) && (size_ == 0); }

	/**
	 * \return First element in buffer
	 * @warning Can throw exception if buffer is empty
	 */
	BufferType Front() const {
		if (Size() == 0)
			throw std::out_of_range(
				"Can't get first element.The buffer is Empty");
		return array_[head_];
	}

	/**
	* \return Last element in buffer
	* @warning Can throw exception if buffer is empty
	*/
	BufferType Back() const {
		if (Size() == 0)
			throw std::out_of_range(
				"Can't get last element.The buffer is Empty");
		return array_[size_ - kIndexOffset];
	}

	/**
	 * \brief Constant iterator class. You won't be able to modify
	 * values of buffer via this iterator
	 */
	class ConstIterator {
	public:
		/**
		 * \brief Equals to buffer type 
		 */
		using value_type = BufferType;
		using const_reference = const BufferType&;
		using const_pointer = const BufferType*;
		using size_type = unsigned int;
		/**
		 * \brief Declaration of iterator_category and difference_type gives us an opportunity to use std algorithms with iterators
		 */
		using iterator_category = std::random_access_iterator_tag;
		using difference_type = std::ptrdiff_t;

		/**
		 * 
		 * \param ptr iterator will point at the same element as ptr
		 */
		explicit ConstIterator(const_pointer ptr) noexcept : m_ptr_(ptr) {
		}

		virtual ~ConstIterator() = default;

		/**
		 * \brief Dereference the pointer
		 * \return Some value which the pointer pointed to
		 */
		const_reference operator*() const noexcept { return *operator->(); }

		const_pointer operator->() const noexcept { return m_ptr_; }

		/**
		 * \brief Prefix increment
		 */
		virtual ConstIterator& operator++() noexcept {
			++m_ptr_;
			return *this;
		}

		/**
		 * \brief Postfix increment
		 */
		ConstIterator operator++(int) const noexcept {
			ConstIterator tmp = *this;
			++(*this);
			return tmp;
		}

		/**
		 * \brief Prefix decrement
		 */
		ConstIterator& operator--() noexcept {
			--m_ptr_;
			return *this;
		}

		/**
		 * \brief Postfix decrement
		 */
		ConstIterator operator--(int) const noexcept {
			ConstIterator tmp = *this;
			--(*this);
			return tmp;
		}

		/**
		 * \brief Moves pointer to moves the pointer to the number of cells equal to amount
		 * \param amount how much will the pointer move
		 */
		ConstIterator& operator+=(const difference_type amount) noexcept {
			m_ptr_ += amount;
			return *this;
		}

		/**
		 * \brief Moves pointer to moves the pointer to the number of cells equal to amount
		 * \param amount how much will the pointer move
		 */
		ConstIterator operator+(difference_type amount) const noexcept {
			auto tmp = *this;
			return tmp += amount;
		}

		/**
		 * \brief Moves pointer  to the number of cells equal to amount
		 * \param amount how much will the pointer move
		 */
		friend ConstIterator operator+(const difference_type amount,
									   ConstIterator it) noexcept {
			return it += amount;
		}

		/**
		 * \brief Moves pointer in reverse way  to the number of cells equal to amount
		 * \param amount how much will the pointer move backwards
		 */
		ConstIterator& operator-=(const difference_type amount) noexcept {
			m_ptr_ -= amount;
			return *this;
		}

		/**
		 * \brief Moves pointer in reverse way  to the number of cells equal to amount
		 * \param amount how much will the pointer move backwards
		 */
		ConstIterator operator-(const difference_type amount) const noexcept {
			auto tmp = *this;
			return tmp -= amount;
		}

		/**
		 * \brief Subtracts one pointer from another
		 */
		difference_type operator-(const ConstIterator& it) const noexcept {
			return m_ptr_ - it.m_ptr_;
		}

		/**
		 * \brief Used for pointer arithmetic
		 */
		const_reference operator[](const difference_type amount) const noexcept {
			return *(*this + amount);
		}

		/**
		 * \brief Rocket compare operator. Makes all types of compare available
		 */
		auto operator<=>(const ConstIterator&) const = default;
	protected:
		const_pointer m_ptr_;
	};

	/**
	 * \brief Standard iterator. You cant change value that being pointed by this iterator.It getting inherited from const-iterator. All operations same as in constant iterator.
	 * @see const_iterator
	 */
	class Iterator final : public ConstIterator {
	public:
		using _Mybase = ConstIterator;
		using value_type = BufferType;
		using reference = BufferType&;
		using pointer = BufferType*;
		using size_type = unsigned int;
		using iterator_category = std::random_access_iterator_tag;
		using difference_type = std::ptrdiff_t;

		Iterator(pointer ptr) : ConstIterator(ptr) {
		};

		reference operator*() noexcept {
			return const_cast<reference>(_Mybase::operator*());
		}

		pointer operator->() noexcept { return this->m_ptr; }

		Iterator& operator++() noexcept override {
			_Mybase::operator++();
			return *this;
		}

		Iterator operator++(int) noexcept {
			Iterator tmp = *this;
			_Mybase::operator++();
			return tmp;
		}

		Iterator& operator--() noexcept {
			_Mybase::operator--();
			return *this;
		}

		Iterator operator--(int) noexcept {
			Iterator tmp = *this;
			_Mybase::operator--();
			return tmp;
		}


		Iterator& operator+=(const difference_type amount) noexcept {
			_Mybase::operator+=(amount);
			return *this;
		}

		Iterator operator+(const difference_type amount) noexcept {
			auto tmp = *this;
			return tmp += amount;
		}

		friend Iterator operator+(const difference_type amount,
								  Iterator it) noexcept {
			return it += amount;
		}

		Iterator& operator-=(const difference_type amount) noexcept {
			_Mybase::operator-=(amount);
			return *this;
		}

		using _Mybase::operator-;

		Iterator operator-(const difference_type amount) noexcept {
			auto tmp = *this;
			return tmp -= amount;
		}


		reference operator[](difference_type amount) noexcept {
			return const_cast<reference>(_Mybase::operator[](amount));
		}
	};

	/**
	 * \return constant iterator to begin of buffer
	 */
	ConstIterator cbegin() const noexcept {
		return ConstIterator(&array_[head_]);
	}

	/**
		 * \return constant iterator to end of buffer
		 */
	ConstIterator cend() const noexcept {
		return ConstIterator(&array_[size_]);
	}

	/**
	 * \return  iterator to begin of buffer
	 */
	Iterator begin() noexcept { return Iterator(&array_[head_]); }


	ConstIterator begin() const noexcept {
		return ConstIterator(&array_[head_]);
	}

	/**
		* \return  iterator to end of buffer
		 */
	Iterator end() noexcept { return Iterator(&array_[size_]); }

	ConstIterator end() const noexcept {
		return ConstIterator(&array_[size_]);
	}

	/**
	 * \return reverse iterator to begin of buffer. So,it points to end.
	 */
	std::reverse_iterator<typename Buffer<BufferType>::Iterator> rbegin() {
		return std::make_reverse_iterator((*this).end());
	}
	/**
	 * \return reverse iterator to end of buffer. So,it points to begin.
	 */
	std::reverse_iterator<typename Buffer<BufferType>::Iterator> rend() {
		return std::make_reverse_iterator((*this).begin());
	}

	/**
	 * \return const reverse iterator to begin of buffer. So,it points to end.
	 */
	std::reverse_iterator<typename Buffer<BufferType>::ConstIterator>
	crbegin() { return rbegin(); }

	/**
		 * \return const reverse iterator to end of buffer. So,it points to begin.
		 */
	std::reverse_iterator<typename Buffer<BufferType>::ConstIterator> cend() {
		return cend();
	}

	friend Iterator operator+(typename Iterator::difference_type amount,
							  const ConstIterator& it) noexcept {
		return it += amount;
	}

	/**
	 * \param first  buffer
	 * \param second  buffer
	 * \return Boolean value that defines whether  they equal element by element
	 */
	friend bool operator==(const Buffer& first, const Buffer& second) noexcept {
		return std::equal(first.begin(), first.end(), second.begin());
	}
	/**
	 * \param first  buffer
	 * \param second  buffer
	 * \return Boolean value that defines whether  they not equal element by element
	 */
	friend bool operator!=(const Buffer& first, const Buffer& second) noexcept {
		return !(first == second);
	}

};

/**
 * \brief Prints all values
 */
template <typename BufferType>
std::ostream& operator
<<(std::ostream& stream, const Buffer<BufferType>& value) {
	for (auto i = value.cbegin(); i != value.cend(); ++i)
		stream << *i << " ";

	return stream;
}
/**
 @see swap
 */
template <typename BufferType>
void Swap(Buffer<BufferType>& left, Buffer<BufferType>& right) noexcept {
	left.Swap(right);
}
