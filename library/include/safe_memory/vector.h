/* -------------------------------------------------------------------------------
* Copyright (c) 2019, OLogN Technologies AG
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

// Initial vesion from:
// https://github.com/electronicarts/EASTL/blob/3.15.00/include/EASTL/vector.h


#ifndef SAFE_MEMORY_VECTOR_H
#define SAFE_MEMORY_VECTOR_H

#include <EASTL/vector.h>
#include <safe_memory/detail/allocator_to_eastl.h>

namespace safe_memory
{
	template <typename T, memory_safety Safety = safeness_declarator<T>::is_safe>
	class SAFE_MEMORY_DEEP_CONST_WHEN_PARAMS vector : protected eastl::vector<T, detail::allocator_to_eastl_vector<T, Safety>>
	{
		typedef vector<T, Safety> 										this_type;
		typedef eastl::vector<T, detail::allocator_to_eastl_vector<T, Safety>>    base_type;

		template <typename T, memory_safety Safety>
		friend bool operator==(const vector<T, Safety>& a, const vector<T, Safety>& b);

		template <typename T, memory_safety Safety>
		friend bool operator!=(const vector<T, Safety>& a, const vector<T, Safety>& b);

		template <typename T, memory_safety Safety>
		friend bool operator<(const vector<T, Safety>& a, const vector<T, Safety>& b);

		template <typename T, memory_safety Safety>
		friend bool operator>(const vector<T, Safety>& a, const vector<T, Safety>& b);

		template <typename T, memory_safety Safety>
		friend bool operator<=(const vector<T, Safety>& a, const vector<T, Safety>& b);

		template <typename T, memory_safety Safety>
		friend bool operator>=(const vector<T, Safety>& a, const vector<T, Safety>& b);

	public:
		using typename base_type::value_type;
		using typename base_type::pointer;
		using typename base_type::const_pointer;
		using typename base_type::reference;
		using typename base_type::const_reference;

		typedef typename base_type::iterator 						iterator_base;
		typedef typename base_type::const_iterator 					const_iterator_base;
		typedef typename base_type::reverse_iterator 				reverse_iterator_base;
		typedef typename base_type::const_reverse_iterator 			const_reverse_iterator_base;
		using typename base_type::size_type;
		using typename base_type::difference_type;

		using typename base_type::allocator_type;
		using typename base_type::array_type;

		typedef std::conditional_t<Safety == memory_safety::none,
			iterator_base,
			detail::array_of_iterator_stack<T>>           iterator;
		typedef std::conditional_t<Safety == memory_safety::none,
			const_iterator_base,
			detail::const_array_of_iterator_stack<T>>            	const_iterator;
		typedef std::reverse_iterator<iterator>                		reverse_iterator;
		typedef std::reverse_iterator<const_iterator>          		const_reverse_iterator;

		typedef detail::array_of_iterator_heap<T, Safety>           iterator_safe;
		typedef detail::const_array_of_iterator_heap<T, Safety>     const_iterator_safe;
		typedef std::reverse_iterator<iterator_safe>                reverse_iterator_safe;
		typedef std::reverse_iterator<const_iterator_safe>          const_reverse_iterator_safe;


		typedef const const_iterator&								csafe_it_arg;
		typedef const const_reverse_iterator&						crsafe_it_arg;
		
		typedef std::pair<const_iterator_base, const_iterator_base>				const_iterator_base_pair;


		using base_type::npos;
		static constexpr memory_safety is_safe = Safety;

	public:
		vector() /*EA_NOEXCEPT_IF(EA_NOEXCEPT_EXPR(EASTL_VECTOR_DEFAULT_ALLOCATOR))*/ : base_type(allocator_type()) {}
		explicit vector(size_type n) : base_type(n, allocator_type()) {}
		vector(size_type n, const value_type& value) : base_type(n, value, allocator_type()) {}
		vector(const this_type& x) : base_type(x) {}
		vector(this_type&& x) noexcept : base_type(std::move(x)) {}
		vector(std::initializer_list<value_type> ilist) : base_type(ilist, allocator_type()) {}
//		vector(csafe_it_arg first, csafe_it_arg last);

	   ~vector() {}

		this_type& operator=(const this_type& x) { base_type::operator=(x); return *this; }
		this_type& operator=(std::initializer_list<value_type> ilist) { base_type::operator=(ilist); return *this; }
		this_type& operator=(this_type&& x) noexcept { base_type::operator=(std::move(x)); return *this; }

		void swap(this_type& x) noexcept { base_type::swap(x); }

		void assign(size_type n, const value_type& value) { base_type::assign(n, value); }

		template <typename InputIterator>
		void assign_unsafe(InputIterator first, InputIterator last) { base_type::assign(first, last); }
		
		void assign(csafe_it_arg first, csafe_it_arg last);

		void assign(std::initializer_list<value_type> ilist) { base_type::assign(ilist); }

		pointer       begin_unsafe() noexcept { return base_type::begin(); }
		const_pointer begin_unsafe() const noexcept { return base_type::begin(); }
		const_pointer cbegin_unsafe() const noexcept { return base_type::cbegin(); }

		pointer       end_unsafe() noexcept { return base_type::end(); }
		const_pointer end_unsafe() const noexcept { return base_type::end(); }
		const_pointer cend_unsafe() const noexcept { return base_type::cend(); }

		reverse_iterator_base       rbegin_unsafe() noexcept { return base_type::rbegin(); }
		const_reverse_iterator_base rbegin_unsafe() const noexcept { return base_type::rbegin(); }
		const_reverse_iterator_base crbegin_unsafe() const noexcept { return base_type::crbegin(); }

		reverse_iterator_base       rend_unsafe() noexcept { return base_type::rend(); }
		const_reverse_iterator_base rend_unsafe() const noexcept { return base_type::rend(); }
		const_reverse_iterator_base crend_unsafe() const noexcept { return base_type::crend(); }

		iterator       begin() noexcept { return makeIt(base_type::begin()); }
		const_iterator begin() const noexcept { return makeIt(base_type::begin()); }
		const_iterator cbegin() const noexcept { return makeIt(base_type::cbegin()); }

		iterator       end() noexcept { return makeIt(base_type::end()); }
		const_iterator end() const noexcept { return makeIt(base_type::end()); }
		const_iterator cend() const noexcept { return makeIt(base_type::cend()); }

		reverse_iterator       rbegin() noexcept { return makeIt(base_type::rbegin()); }
		const_reverse_iterator rbegin() const noexcept { return makeIt(base_type::rbegin()); }
		const_reverse_iterator crbegin() const noexcept { return makeIt(base_type::crbegin()); }

		reverse_iterator       rend() noexcept { return makeIt(base_type::rend()); }
		const_reverse_iterator rend() const noexcept { return makeIt(base_type::rend()); }
		const_reverse_iterator crend() const noexcept { return makeIt(base_type::crend()); }

		using base_type::empty;
		using base_type::size;
		using base_type::capacity;

		//TODO: mb this is not really well checked at eastl::vector
		size_type max_size() const { return base_type::kMaxSize; }

		using base_type::resize;
		using base_type::reserve;
		using base_type::set_capacity;
		using base_type::shrink_to_fit;

		pointer       data_unsafe() noexcept { return base_type::data(); }
		const_pointer data_unsafe() const noexcept { return base_type::data(); }

		reference       operator[](size_type n);
		const_reference operator[](size_type n) const;

		reference       at(size_type n);
		const_reference at(size_type n) const;

		reference       front();
		const_reference front() const;

		reference       back();
		const_reference back() const;

		using base_type::push_back;

		void pop_back();

		//not allowed
//		void*     push_back_uninitialized();

		template<class... Args>
		pointer emplace_unsafe(const_pointer position, Args&&... args) { return base_type::emplace(position, std::forward<Args>(args)...); }
		template<class... Args>
		iterator emplace(csafe_it_arg position, Args&&... args);

		using base_type::emplace_back;

		pointer insert_unsafe(const_pointer position, const value_type& value) { return base_type::insert(position, value); }
		pointer insert_unsafe(const_pointer position, size_type n, const value_type& value) { return base_type::insert(position, n, value); }
		pointer insert_unsafe(const_pointer position, value_type&& value) { return base_type::insert(position, std::move(value)); }
		pointer insert_unsafe(const_pointer position, std::initializer_list<value_type> ilist) { return base_type::insert(position, ilist); }

		template <typename InputIterator>
		pointer insert_unsafe(const_pointer position, InputIterator first, InputIterator last) { return base_type::insert(position, first, last); }

		iterator insert(csafe_it_arg position, const value_type& value);
		iterator insert(csafe_it_arg position, size_type n, const value_type& value);
		iterator insert(csafe_it_arg position, value_type&& value);
		iterator insert(csafe_it_arg position, std::initializer_list<value_type> ilist);

		// template <typename InputIterator>
		iterator insert(csafe_it_arg position, csafe_it_arg first, csafe_it_arg last);

		// iterator erase_first(const T& value);
		// iterator erase_first_unsorted(const T& value); // Same as erase, except it doesn't preserve order, but is faster because it simply copies the last item in the vector over the erased position.
		// reverse_iterator erase_last(const T& value);
		// reverse_iterator erase_last_unsorted(const T& value); // Same as erase, except it doesn't preserve order, but is faster because it simply copies the last item in the vector over the erased position.


		pointer erase_unsafe(const_pointer position) { return base_type::erase(position); }
		pointer erase_unsafe(const_pointer first, const_pointer last) { return base_type::erase(first, last); }
		iterator erase(csafe_it_arg position);
		iterator erase(csafe_it_arg first, csafe_it_arg last);
		pointer erase_unsorted_unsafe(const_pointer position) { return base_type::erase_unsorted(position); }

		reverse_iterator_base erase_unsafe(const_reverse_iterator_base position) { return base_type::erase(position); }
		reverse_iterator_base erase_unsafe(const_reverse_iterator_base first, const_reverse_iterator_base last) { return base_type::erase(first, last); }
		reverse_iterator_base erase_unsorted_unsafe(const_reverse_iterator_base position) { return base_type::erase_unsorted(position); }

		using base_type::clear;
		//not allowed
		// void reset_lose_memory() noexcept;

		using base_type::validate;
		using base_type::validate_iterator;

		detail::iterator_validity  validate_iterator2(csafe_it_arg i) const noexcept;

	protected:
		[[noreturn]] static void ThrowRangeException(const char* msg) { throw std::out_of_range(msg); }
		[[noreturn]] static void ThrowInvalidArgumentException(const char* msg) { throw std::invalid_argument(msg); }
		[[noreturn]] static void ThrowMaxSizeException(const char* msg) { throw std::out_of_range(msg); }


        const base_type& toBase() const noexcept { return *this; }

		// Safety == none
		const_iterator_base toBase(const_iterator_base it) const { return it; }
		const_iterator_base_pair toBase(const_iterator_base it, const_iterator_base it2) const { return { it, it2 }; }
		const_iterator_base_pair toBaseOther(const_iterator_base it, const_iterator_base it2) const { return { it, it2 }; }
		
		// Safety == safe
		const_iterator_base toBase(const detail::const_array_of_iterator_stack<T>& it) const {
			return it.toRaw(base_type::mpBegin);
		}

		const_iterator_base_pair toBase(const detail::const_array_of_iterator_stack<T>& it, const detail::const_array_of_iterator_stack<T>& it2) const {
			return it.toRaw(base_type::mpBegin, it2);
		}

		const_iterator_base_pair toBaseOther(const detail::const_array_of_iterator_stack<T>& it, const detail::const_array_of_iterator_stack<T>& it2) const {
			return it.toRawOther(it2);
		}

		const_iterator_base toBase(const detail::const_array_of_iterator_heap<T, Safety>& it) const {
			return it.toRaw(base_type::mpBegin);
		}

		const_iterator_base_pair toBase(const detail::const_array_of_iterator_heap<T, Safety>& it, const detail::const_array_of_iterator_heap<T, Safety>& it2) const {
			return it.toRaw(base_type::mpBegin, it2);
		}

		const_iterator_base_pair toBaseOther(const detail::const_array_of_iterator_heap<T, Safety>& it, const detail::const_array_of_iterator_heap<T, Safety>& it2) const {
			return it.toRawOther(it2);
		}


		iterator makeIt(iterator_base it) {
			if constexpr (Safety == memory_safety::none)
				return it;
			else
				return iterator::makePtr(allocator_type::to_raw(base_type::mpBegin), it, base_type::capacity());
		}
		const_iterator makeIt(const_iterator_base it) const {
			if constexpr (Safety == memory_safety::none)
				return it;
			else
				return const_iterator::makePtr(allocator_type::to_raw(base_type::mpBegin), it, base_type::capacity());
		}

		reverse_iterator makeIt(const typename base_type::reverse_iterator& it) {
			return reverse_iterator(makeIt(it.base()));
		}
		const_reverse_iterator makeIt(const typename base_type::const_reverse_iterator& it) const {
			return const_reverse_iterator(makeIt(it.base()));
		}





		iterator_safe makeSafeIt(iterator_base it) {
			return iterator_safe::makePtr(allocator_type::to_soft(base_type::mpBegin), it, base_type::capacity());
		}
		
		const_iterator_safe makeSafeIt(const_iterator_base it) const {
			return const_iterator_safe::makePtr(allocator_type::to_soft(base_type::mpBegin), it, base_type::capacity());
		}

		reverse_iterator_safe makeSafeIt(const typename base_type::reverse_iterator& it) {
			return reverse_iterator_safe(makeSafeIt(it.base()));
		}
		const_reverse_iterator_safe makeSafeIt(const typename base_type::const_reverse_iterator& it) const {
			return const_reverse_iterator_safe(makeSafeIt(it.base()));
		}

	}; // class vector


	// template <typename T, memory_safety Safety>
	// inline vector<T, Safety>::vector(csafe_it_arg first, csafe_it_arg last)
	// 	: base_type()
	// {
	// 	const_pointer_pair p = CheckAndGet(first, last);
	// 	size_type sz = static_cast<size_type>(p.second - p.first);

	// 	SetNewHeap(DoAllocate(sz));
	// 	mpEnd = std::uninitialized_copy(p.first, p.second, mpBegin);
	// }



	template <typename T, memory_safety Safety>
	inline void vector<T, Safety>::assign(csafe_it_arg first, csafe_it_arg last)
	{
		auto p = toBaseOther(first, last);
		base_type::assign(p.first, p.second);
	}

	template <typename T, memory_safety Safety>
	inline typename vector<T, Safety>::reference
	vector<T, Safety>::operator[](size_type n)
	{
		if constexpr(is_safe == memory_safety::safe) {
			if(NODECPP_UNLIKELY(n >= size()))
				ThrowRangeException("vector::operator[] -- out of range");
		}

		return base_type::operator[](n);
	}


	template <typename T, memory_safety Safety>
	inline typename vector<T, Safety>::const_reference
	vector<T, Safety>::operator[](size_type n) const
	{
		if constexpr(is_safe == memory_safety::safe) {
			if(NODECPP_UNLIKELY(n >= size()))
				ThrowRangeException("vector::operator[] -- out of range");
		}

		return base_type::operator[](n);
	}

	template <typename T, memory_safety Safety>
	inline typename vector<T, Safety>::reference
	vector<T, Safety>::at(size_type n)
	{
		if constexpr(is_safe == memory_safety::safe) {
			if(NODECPP_UNLIKELY(n >= size()))
				ThrowRangeException("vector::at -- out of range");

			//now return unckeched
			return base_type::operator[](n);
		}
		else
			return base_type::at(n);
	}


	template <typename T, memory_safety Safety>
	inline typename vector<T, Safety>::const_reference
	vector<T, Safety>::at(size_type n) const
	{
		if constexpr(is_safe == memory_safety::safe) {
			if(NODECPP_UNLIKELY(n >= size()))
				ThrowRangeException("vector::at -- out of range");

			//now return unckeched
			return base_type::operator[](n);
		}
		else
			return base_type::at(n);
	}


	template <typename T, memory_safety Safety>
	inline typename vector<T, Safety>::reference
	vector<T, Safety>::front()
	{
		if constexpr(is_safe == memory_safety::safe) {
			if(NODECPP_UNLIKELY(empty()))
				ThrowRangeException("vector::front -- empty vector");
		}

		return base_type::front();
	}


	template <typename T, memory_safety Safety>
	inline typename vector<T, Safety>::const_reference
	vector<T, Safety>::front() const
	{
		if constexpr(is_safe == memory_safety::safe) {
			if(NODECPP_UNLIKELY(empty()))
				ThrowRangeException("vector::front -- empty vector");
		}

		return base_type::front();
	}


	template <typename T, memory_safety Safety>
	inline typename vector<T, Safety>::reference
	vector<T, Safety>::back()
	{
		if constexpr(is_safe == memory_safety::safe) {
			if(NODECPP_UNLIKELY(empty()))
				ThrowRangeException("vector::front -- empty vector");
		}

		return base_type::back();
	}


	template <typename T, memory_safety Safety>
	inline typename vector<T, Safety>::const_reference
	vector<T, Safety>::back() const
	{
		if constexpr(is_safe == memory_safety::safe) {
			if(NODECPP_UNLIKELY(empty()))
				ThrowRangeException("vector::front -- empty vector");
		}

		return base_type::back();
	}


	template <typename T, memory_safety Safety>
	inline void vector<T, Safety>::pop_back()
	{
		if constexpr(is_safe == memory_safety::safe) {
			if(NODECPP_UNLIKELY(empty()))
				ThrowRangeException("vector::pop_back -- empty vector");
		}

		return base_type::pop_back();
	}


	template <typename T, memory_safety Safety>
	template<class... Args>
	inline typename vector<T, Safety>::iterator 
	vector<T, Safety>::emplace(csafe_it_arg position, Args&&... args)
	{
		return makeIt(base_type::emplace(toBase(position), std::forward<Args>(args)...));
	}

	template <typename T, memory_safety Safety>
	inline typename vector<T, Safety>::iterator
	vector<T, Safety>::insert(csafe_it_arg position, const value_type& value)
	{
		return makeIt(base_type::insert(toBase(position), value));
	}


	template <typename T, memory_safety Safety>       
	inline typename vector<T, Safety>::iterator
	vector<T, Safety>::insert(csafe_it_arg position, value_type&& value)
	{
		return makeIt(base_type::insert(toBase(position), std::move(value)));
	}


	template <typename T, memory_safety Safety>
	inline typename vector<T, Safety>::iterator
	vector<T, Safety>::insert(csafe_it_arg position, size_type n, const value_type& value)
	{
		return makeIt(base_type::insert(toBase(position), n, value));
	}


	template <typename T, memory_safety Safety>
	inline typename vector<T, Safety>::iterator
	vector<T, Safety>::insert(csafe_it_arg position, csafe_it_arg first, csafe_it_arg last)
	{
		auto other = toBaseOther(first, last);
		return makeIt(base_type::insert(toBase(position), other.first, other.second));
	}


	template <typename T, memory_safety Safety>       
	inline typename vector<T, Safety>::iterator
	vector<T, Safety>::insert(csafe_it_arg position, std::initializer_list<value_type> ilist)
	{
		return makeIt(base_type::insert(toBase(position), ilist));
	}


	template <typename T, memory_safety Safety>
	inline typename vector<T, Safety>::iterator
	vector<T, Safety>::erase(csafe_it_arg position)
	{
		return makeIt(base_type::erase(toBase(position)));
	}


	template <typename T, memory_safety Safety>
	inline typename vector<T, Safety>::iterator
	vector<T, Safety>::erase(csafe_it_arg first, csafe_it_arg last)
	{
		auto p = toBase(first, last);
		return makeIt(base_type::erase(p.first, p.second));
	}


	template <typename T, memory_safety Safety>
	inline detail::iterator_validity vector<T, Safety>::validate_iterator2(csafe_it_arg i) const noexcept
	{
		if constexpr (Safety == memory_safety::none) {
			if(i == nullptr)
				return detail::iterator_validity::Null;
			else if(i >= begin_unsafe())
			{
				if(i < end_unsafe())
					return detail::iterator_validity::ValidCanDeref;

				else if(i == end_unsafe())
					return detail::iterator_validity::ValidEnd;

				else if(i < begin_unsafe() + capacity())
					return detail::iterator_validity::InvalidZoombie;
			}

			return detail::iterator_validity::xxx_Broken_xxx;
		}
		else {
			//TODO 
			return detail::iterator_validity::ValidCanDeref;
			// i.validate_iterator(cbegin(), cend());
		}
	}


	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template <typename T, memory_safety Safety>
	inline bool operator==(const vector<T, Safety>& a, const vector<T, Safety>& b)
	{
		return operator==(a.toBase(), b.toBase());
	}


	template <typename T, memory_safety Safety>
	inline bool operator!=(const vector<T, Safety>& a, const vector<T, Safety>& b)
	{
		return operator!=(a.toBase(), b.toBase());
	}


	template <typename T, memory_safety Safety>
	inline bool operator<(const vector<T, Safety>& a, const vector<T, Safety>& b)
	{
		return operator<(a.toBase(), b.toBase());
	}


	template <typename T, memory_safety Safety>
	inline bool operator>(const vector<T, Safety>& a, const vector<T, Safety>& b)
	{
		return operator>(a.toBase(), b.toBase());
	}


	template <typename T, memory_safety Safety>
	inline bool operator<=(const vector<T, Safety>& a, const vector<T, Safety>& b)
	{
		return operator<=(a.toBase(), b.toBase());
	}


	template <typename T, memory_safety Safety>
	inline bool operator>=(const vector<T, Safety>& a, const vector<T, Safety>& b)
	{
		return operator>=(a.toBase(), b.toBase());
	}


	template <typename T, memory_safety Safety>
	inline void swap(vector<T, Safety>& a, vector<T, Safety>& b) noexcept
	{
		a.swap(b);
	}


} // namespace safe_memory

#endif // Header include guard
