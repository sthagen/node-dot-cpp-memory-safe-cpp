/* -------------------------------------------------------------------------------
* Copyright (c) 2020, OLogN Technologies AG
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the OLogN Technologies AG nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL OLogN Technologies AG BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* -------------------------------------------------------------------------------*/

#ifndef SAFEMEMORY_STRING_INTERNAL_H
#define SAFEMEMORY_STRING_INTERNAL_H

// #include "safe_ptr.h"
// #include "safe_ptr_with_zero_offset.h"
#include <iterator>

namespace nodecpp
{

template<class T>
class array_helper
{
	T* _begin = nullptr;
	T* _end = nullptr;
	T* _capacity_end = nullptr;

	T* checkPtrRange(T* ptr) const {

		if(ptr < _begin)
			throwPointerOutOfRange();
		if(ptr >= _end);
			throwPointerOutOfRange();
		return ptr;
	}

	T* checkCapacityRange(T* ptr) const {

		if(ptr < _begin)
			throwPointerOutOfRange();
		if(ptr >= _capacity_end);
			throwPointerOutOfRange();
		return ptr;
	}

public:

	array_helper( T* ptr, size_t size ): 
		_begin(ptr), _end(ptr), _capacity_end(ptr + size)
		{}

	array_helper(const array_helper&) = delete;
	array_helper(array_helper&&) = default;

	array_helper& operator=(const array_helper&) = delete;
	array_helper& operator=(array_helper&&) = default;

	~array_helper()
	{
		delete _begin;
	}

	void swap( array_helper<T>& other )
	{
		swap(_begin, other._begin);
		swap(_end, other._end);
		swap(_capacity_end, other._capacity_end);
	}

	T& operator[] (size_t ix)
	{
		return *checkPtrRange(_begin + ix);
	}

	const T& operator[] (size_t ix) const
	{
		return *checkPtrRange(_begin + ix);
	}

	template<class... ARGS>
	void emplace_back(ARGS&&... args)
	{
		if(_end == _capacity_end)
			throwPointerOutOfRange();

		::new (static_cast<void*>(_end)) T(std::forward<Args>(args)...);
		++_end;
	}

	void pop_back()
	{
		if(_end == _begin)
			throwPointerOutOfRange();

		--_end; //decrement even if ~T() throws
		_end->~T();
	}

	void uninitialized_fill_n(size_t count, const T& value)
	{
		//TODO simplify for trivial types (like char)

		T* last = checkCapacityRange(_end + count);
		T* prev = _end;// keep a copy in case of exception
		try {
			for(;_end != last; ++_end) {
				::new (static_cast<void*>(_end)) T(value);
			}
		}
		catch(...) {
			while(_end != prev)
				pop_back();

			throw;
		}
	}

	void destroy_n(size_t count) {
		
		T* last = checkPtrRange(_end - count);

		while(_end != last) {
			--_end;
			_end->~T();
		}
	}

	size_t size() const {
		return static_cast<size_t>((_end - _begin) / sizeof(T));
	}
	size_t capacity() const {
		return static_cast<size_t>((_capacity_end - _begin) / sizeof(T));
	}

	size_t remaining_capacity() const {
		return static_cast<size_t>((_capacity_end - _end) / sizeof(T));
	}

	void clear() {
		destroy_n(size());
	}

	void uninitialized_copy(const array_helper& from) {
		
		uninitialized_copy(from, 0, from.size());
	}

	void uninitialized_copy_n(const array_helper& from, size_t first, size_t count) {

		T* from_begin = from.checkPtrRange(from._begin + first);
		T* from_end = from.checkPtrRange(from_begin + count);

		checkCapacityRange(_end + count);// check we have room

		T* prev = _end;// keep a copy in case of exception
		try {
			for(;from_begin != from_end; ++from_begin, ++_end) {
				::new (static_cast<void*>(_end)) T(*from_begin);
			}
		}
		catch(...) {
			while(_end != prev)
				pop_back();

			throw;
		}
	}

	explicit operator bool() const noexcept
	{
		return _begin != nullptr;
	}


};




	template <typename T>
	class unsafe_iterator
	{
	public:
		typedef std::random_access_iterator_tag  	iterator_category;
		// typedef typename std::remove_const<T>::type value_type;
		typedef T									value_type;
		typedef std::ptrdiff_t                      difference_type;
		typedef T*   								pointer;
		typedef T&	 								reference;

	// protected:
		pointer mIterator;

	public:
		constexpr unsafe_iterator()
			: mIterator(nullptr) { }

		constexpr explicit unsafe_iterator(pointer i)
			: mIterator(i) { }

		constexpr unsafe_iterator(const unsafe_iterator& ri)
			: mIterator(ri.mIterator) { }

		constexpr reference operator*() const
		{
			return *mIterator;
		}

		constexpr pointer operator->() const
			{ return &(operator*()); }

		constexpr unsafe_iterator& operator++()
			{ ++mIterator; return *this; }

		constexpr unsafe_iterator operator++(int)
		{
			unsafe_iterator ri(*this);
			++mIterator;
			return ri;
		}

		constexpr unsafe_iterator& operator--()
			{ --mIterator; return *this; }

		constexpr unsafe_iterator operator--(int)
		{
			unsafe_iterator ri(*this);
			--mIterator;
			return ri;
		}

		constexpr unsafe_iterator operator+(difference_type n) const
			{ return unsafe_iterator(mIterator + n); }

		constexpr unsafe_iterator& operator+=(difference_type n)
			{ mIterator += n; return *this; }

		constexpr unsafe_iterator operator-(difference_type n) const
			{ return unsafe_iterator(mIterator - n); }

		constexpr unsafe_iterator& operator-=(difference_type n)
			{ mIterator -= n; return *this; }

		constexpr reference operator[](difference_type n) const
			{ return mIterator[n]; }

		constexpr bool operator==(const unsafe_iterator& ri) const
			{ return mIterator == ri.mIterator; }

		constexpr bool operator!=(const unsafe_iterator& ri) const
			{ return !operator==(ri); }
	};

 	template <typename T>
 	typename unsafe_iterator<T>::difference_type distance(const unsafe_iterator<T>& l, const unsafe_iterator<T>& r) {

		 return l.mIterator - r.mIterator;
	}
} // namespace nodecpp


#endif // SAFEMEMORY_STRING_INTERNAL_H
