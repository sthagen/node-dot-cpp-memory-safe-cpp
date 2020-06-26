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
// https://github.com/electronicarts/EASTL/blob/3.15.00/include/EASTL/hash_map.h

///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file is based on the TR1 (technical report 1) reference implementation
// of the unordered_set/unordered_map C++ classes as of about 4/2005. Most likely
// many or all C++ library vendors' implementations of this classes will be 
// based off of the reference version and so will look pretty similar to this
// file as well as other vendors' versions. 
///////////////////////////////////////////////////////////////////////////////


#ifndef SAFE_MEMORY_EASTL_HASH_MAP_H
#define SAFE_MEMORY_EASTL_HASH_MAP_H


#include <safe_memory/EASTL/internal/hashtable.h>
#include <functional>
#include <utility>

// #if defined(EA_PRAGMA_ONCE_SUPPORTED)
// 	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
// #endif



namespace safe_memory::detail
{
	/// hash_map
	///
	/// Implements a hash_map, which is a hashed associative container.
	/// Lookups are O(1) (that is, they are fast) but the container is 
	/// not sorted. Note that lookups are only O(1) if the hash table
	/// is well-distributed (non-colliding). The lookup approaches
	/// O(n) behavior as the table becomes increasingly poorly distributed.
	///
	/// set_max_load_factor
	/// If you want to make a hashtable never increase its bucket usage,
	/// call set_max_load_factor with a very high value such as 100000.f.
	///
	/// bCacheHashCode
	/// We provide the boolean bCacheHashCode template parameter in order 
	/// to allow the storing of the hash code of the key within the map. 
	/// When this option is disabled, the rehashing of the table will 
	/// call the hash function on the key. Setting bCacheHashCode to true 
	/// is useful for cases whereby the calculation of the hash value for
	/// a contained object is very expensive.
	///
	/// find_as
	/// In order to support the ability to have a hashtable of strings but
	/// be able to do efficiently lookups via char pointers (i.e. so they 
	/// aren't converted to string objects), we provide the find_as 
	/// function. This function allows you to do a find with a key of a
	/// type other than the hashtable key type.
	///
	/// Example find_as usage:
	///     hash_map<string, int> hashMap;
	///     i = hashMap.find_as("hello");    // Use default hash and compare.
	///
	/// Example find_as usage (namespaces omitted for brevity):
	///     hash_map<string, int> hashMap;
	///     i = hashMap.find_as("hello", hash<char*>(), equal_to_2<string, char*>());
	///
	template <typename Key, typename T, typename Hash = hash<Key>, typename Predicate = equal_to<Key>, 
			  memory_safety Safety = safeness_declarator<std::pair<const Key, T>>::is_safe, bool bCacheHashCode = false>
	class SAFE_MEMORY_DEEP_CONST_WHEN_PARAMS hash_map
		: public hashtable<Key, std::pair<const Key, T>, Safety, use_first<std::pair<const Key, T> >, Predicate,
							Hash, mod_range_hashing, default_ranged_hash, prime_rehash_policy, bCacheHashCode, true, true>
	{
	public:
		typedef hashtable<Key, std::pair<const Key, T>, Safety, 
						  use_first<std::pair<const Key, T> >, 
						  Predicate, Hash, mod_range_hashing, default_ranged_hash, 
						  prime_rehash_policy, bCacheHashCode, true, true>        base_type;
		typedef hash_map<Key, T, Hash, Predicate, Safety, bCacheHashCode>      this_type;
		typedef typename base_type::size_type                                     size_type;
		typedef typename base_type::key_type                                      key_type;
		typedef T                                                                 mapped_type;
		typedef typename base_type::value_type                                    value_type;     // NOTE: 'value_type = pair<const key_type, mapped_type>'.
		// typedef typename base_type::allocator_type                                allocator_type;
		typedef typename base_type::node_type                                     node_type;
		typedef typename base_type::insert_return_type                            insert_return_type;
		typedef typename base_type::iterator                                      iterator;
		typedef typename base_type::const_iterator                                const_iterator;

		using base_type::insert;

	public:
		/// hash_map
		///
		/// Default constructor.
		///
		explicit hash_map(/*const allocator_type& allocator = EASTL_HASH_MAP_DEFAULT_ALLOCATOR*/)
			: base_type(0, Hash(), mod_range_hashing(), default_ranged_hash(), 
						Predicate(), use_first<std::pair<const Key, T> >()/*, allocator*/)
		{
			// Empty
		}


		/// hash_map
		///
		/// Constructor which creates an empty container, but start with nBucketCount buckets.
		/// We default to a small nBucketCount value, though the user really should manually 
		/// specify an appropriate value in order to prevent memory from being reallocated.
		///
		explicit hash_map(size_type nBucketCount, const Hash& hashFunction = Hash(), 
						  const Predicate& predicate = Predicate()/*, const allocator_type& allocator = EASTL_HASH_MAP_DEFAULT_ALLOCATOR*/)
			: base_type(nBucketCount, hashFunction, mod_range_hashing(), default_ranged_hash(), 
						predicate, use_first<std::pair<const Key, T> >()/*, allocator*/)
		{
			// Empty
		}


		hash_map(const this_type& x)
		  : base_type(x)
		{
		}


		hash_map(this_type&& x)
		  : base_type(std::move(x))
		{
		}


		// hash_map(this_type&& x, const allocator_type& allocator)
		//   : base_type(std::move(x), allocator)
		// {
		// }


		/// hash_map
		///
		/// initializer_list-based constructor. 
		/// Allows for initializing with brace values (e.g. hash_map<int, char*> hm = { {3,"c"}, {4,"d"}, {5,"e"} }; )
		///     
		hash_map(std::initializer_list<value_type> ilist, size_type nBucketCount = 0, const Hash& hashFunction = Hash(), 
				   const Predicate& predicate = Predicate()/*, const allocator_type& allocator = EASTL_HASH_MAP_DEFAULT_ALLOCATOR*/)
			: base_type(nBucketCount, hashFunction, mod_range_hashing(), default_ranged_hash(), 
						predicate, use_first<std::pair<const Key, T> >()/*, allocator*/)
		{
			//TODO: mb: improve, since we know the list size before construction
			base_type::insert_unsafe(ilist.begin(), ilist.end());
		}


		/// hash_map
		///
		/// An input bucket count of <= 1 causes the bucket count to be equal to the number of 
		/// elements in the input range.
		///
		// template <typename ForwardIterator>
		// hash_map(ForwardIterator first, ForwardIterator last, size_type nBucketCount = 0, const Hash& hashFunction = Hash(), 
		// 		 const Predicate& predicate = Predicate()/*, const allocator_type& allocator = EASTL_HASH_MAP_DEFAULT_ALLOCATOR*/)
		// 	: base_type(first, last, nBucketCount, hashFunction, mod_range_hashing(), default_ranged_hash(), 
		// 				predicate, use_first<std::pair<const Key, T> >()/*, allocator*/)
		// {
		// 	// Empty
		// }


		this_type& operator=(const this_type& x)
		{
			return static_cast<this_type&>(base_type::operator=(x));
		}


		this_type& operator=(std::initializer_list<value_type> ilist)
		{
			return static_cast<this_type&>(base_type::operator=(ilist));
		}


		this_type& operator=(this_type&& x)
		{
			return static_cast<this_type&>(base_type::operator=(std::move(x)));
		}


		/// insert
		///
		/// This is an extension to the C++ standard. We insert a default-constructed 
		/// element with the given key. The reason for this is that we can avoid the 
		/// potentially expensive operation of creating and/or copying a mapped_type
		/// object on the stack.
		insert_return_type insert(const key_type& key)
		{
			return base_type::DoInsertKey(std::true_type(), key);
		}

		T& at(const key_type& k)
		{
			iterator it = base_type::find(k);

			if (it == base_type::end())
			{
				// #if EASTL_EXCEPTIONS_ENABLED
					// throw exeption if exceptions enabled
					base_type::ThrowRangeException("invalid hash_map<K, T> key");
				// #else
				// 	// assert false if asserts enabled
				// 	EASTL_ASSERT_MSG(false, "invalid hash_map<K, T> key");
				// #endif
			}
			// undefined behaviour if exceptions and asserts are disabled and it == end()
			return it->second;
		}


		const T& at(const key_type& k) const
		{
			const_iterator it = base_type::find(k);

			if (it == base_type::end())
			{
				// #if EASTL_EXCEPTIONS_ENABLED
					// throw exeption if exceptions enabled
					base_type::ThrowRangeException("invalid hash_map<K, T> key");
				// #else
				// 	// assert false if asserts enabled
				// 	EASTL_ASSERT_MSG(false, "invalid hash_map<K, T> key");
				// #endif
			}
			// undefined behaviour if exceptions and asserts are disabled and it == end()
			return it->second;
		}


		insert_return_type insert(key_type&& key)
		{
			return base_type::DoInsertKey(std::true_type(), std::move(key));
		}


		mapped_type& operator[](const key_type& key)
		{
			return (*base_type::DoInsertKey(std::true_type(), key).first).second;

			// Slower reference version:
			//const typename base_type::iterator it = base_type::find(key);
			//if(it != base_type::end())
			//    return (*it).second;
			//return (*base_type::insert(value_type(key, mapped_type())).first).second;
		}

		mapped_type& operator[](key_type&& key)
		{
			// The Standard states that this function "inserts the value value_type(std::move(key), mapped_type())"
			return (*base_type::DoInsertKey(std::true_type(), std::move(key)).first).second;
		}


	}; // hash_map






	/// hash_multimap
	///
	/// Implements a hash_multimap, which is the same thing as a hash_map 
	/// except that contained elements need not be unique. See the 
	/// documentation for hash_set for details.
	///
	template <typename Key, typename T, typename Hash = hash<Key>, typename Predicate = equal_to<Key>,
			  memory_safety Safety = safeness_declarator<std::pair<const Key, T>>::is_safe, bool bCacheHashCode = false>
	class SAFE_MEMORY_DEEP_CONST_WHEN_PARAMS hash_multimap
		: public hashtable<Key, std::pair<const Key, T>, Safety, use_first<std::pair<const Key, T> >, Predicate,
						   Hash, mod_range_hashing, default_ranged_hash, prime_rehash_policy, bCacheHashCode, true, false>
	{
	public:
		typedef hashtable<Key, std::pair<const Key, T>, Safety, 
						  use_first<std::pair<const Key, T> >, 
						  Predicate, Hash, mod_range_hashing, default_ranged_hash, 
						  prime_rehash_policy, bCacheHashCode, true, false>           base_type;
		typedef hash_multimap<Key, T, Hash, Predicate, Safety, bCacheHashCode>     this_type;
		typedef typename base_type::size_type                                         size_type;
		typedef typename base_type::key_type                                          key_type;
		typedef T                                                                     mapped_type;
		typedef typename base_type::value_type                                        value_type;     // Note that this is pair<const key_type, mapped_type>.
		// typedef typename base_type::allocator_type                                    allocator_type;
		typedef typename base_type::node_type                                         node_type;
		typedef typename base_type::insert_return_type                                insert_return_type;
		typedef typename base_type::iterator                                          iterator;

		using base_type::insert;

	private:
		using base_type::try_emplace;
		using base_type::insert_or_assign;

	public:
		/// hash_multimap
		///
		/// Default constructor.
		///
		explicit hash_multimap(/*const allocator_type& allocator = EASTL_HASH_MULTIMAP_DEFAULT_ALLOCATOR*/)
			: base_type(0, Hash(), mod_range_hashing(), default_ranged_hash(), 
						Predicate(), use_first<std::pair<const Key, T> >()/*, allocator*/)
		{
			// Empty
		}


		/// hash_multimap
		///
		/// Constructor which creates an empty container, but start with nBucketCount buckets.
		/// We default to a small nBucketCount value, though the user really should manually 
		/// specify an appropriate value in order to prevent memory from being reallocated.
		///
		explicit hash_multimap(size_type nBucketCount, const Hash& hashFunction = Hash(), 
							   const Predicate& predicate = Predicate()/*, const allocator_type& allocator = EASTL_HASH_MULTIMAP_DEFAULT_ALLOCATOR*/)
			: base_type(nBucketCount, hashFunction, mod_range_hashing(), default_ranged_hash(), 
						predicate, use_first<std::pair<const Key, T> >()/*, allocator*/)
		{
			// Empty
		}


		hash_multimap(const this_type& x)
		  : base_type(x)
		{
		}


		hash_multimap(this_type&& x)
		  : base_type(std::move(x))
		{
		}


		// hash_multimap(this_type&& x, const allocator_type& allocator)
		//   : base_type(std::move(x), allocator)
		// {
		// }


		/// hash_multimap
		///
		/// initializer_list-based constructor. 
		/// Allows for initializing with brace values (e.g. hash_multimap<int, char*> hm = { {3,"c"}, {3,"C"}, {4,"d"} }; )
		///     
		hash_multimap(std::initializer_list<value_type> ilist, size_type nBucketCount = 0, const Hash& hashFunction = Hash(), 
				   const Predicate& predicate = Predicate()/*, const allocator_type& allocator = EASTL_HASH_MULTIMAP_DEFAULT_ALLOCATOR*/)
			: base_type(nBucketCount, hashFunction, mod_range_hashing(), default_ranged_hash(), 
						predicate, use_first<std::pair<const Key, T> >()/*, allocator*/)
		{
			//TODO: mb: improve, since we know the list size before construction
			base_type::insert_unsafe(ilist.begin(), ilist.end());
		}


		/// hash_multimap
		///
		/// An input bucket count of <= 1 causes the bucket count to be equal to the number of 
		/// elements in the input range.
		///
		// template <typename ForwardIterator>
		// hash_multimap(ForwardIterator first, ForwardIterator last, size_type nBucketCount = 0, const Hash& hashFunction = Hash(), 
		// 			  const Predicate& predicate = Predicate()/*, const allocator_type& allocator = EASTL_HASH_MULTIMAP_DEFAULT_ALLOCATOR*/)
		// 	: base_type(first, last, nBucketCount, hashFunction, mod_range_hashing(), default_ranged_hash(), 
		// 				predicate, use_first<std::pair<const Key, T> >()/*, allocator*/)
		// {
		// 	// Empty
		// }


		this_type& operator=(const this_type& x)
		{
			return static_cast<this_type&>(base_type::operator=(x));
		}


		this_type& operator=(std::initializer_list<value_type> ilist)
		{
			return static_cast<this_type&>(base_type::operator=(ilist));
		}


		this_type& operator=(this_type&& x)
		{
			return static_cast<this_type&>(base_type::operator=(std::move(x)));
		}


		/// insert
		///
		/// This is an extension to the C++ standard. We insert a default-constructed 
		/// element with the given key. The reason for this is that we can avoid the 
		/// potentially expensive operation of creating and/or copying a mapped_type
		/// object on the stack.
		insert_return_type insert(const key_type& key)
		{
			return base_type::DoInsertKey(std::false_type(), key);
		}


		insert_return_type insert(key_type&& key)
		{
			return base_type::DoInsertKey(std::false_type(), std::move(key));
		}


	}; // hash_multimap



	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template <typename Key, typename T, typename Hash, typename Predicate, memory_safety Safety, bool bCacheHashCode>
	inline bool operator==(const hash_map<Key, T, Hash, Predicate, Safety, bCacheHashCode>& a, 
						   const hash_map<Key, T, Hash, Predicate, Safety, bCacheHashCode>& b)
	{
		typedef typename hash_map<Key, T, Hash, Predicate, Safety, bCacheHashCode>::const_iterator const_iterator;

		// We implement branching with the assumption that the return value is usually false.
		if(a.size() != b.size())
			return false;

		// For map (with its unique keys), we need only test that each element in a can be found in b,
		// as there can be only one such pairing per element. multimap needs to do a something more elaborate.
		for(const_iterator ai = a.begin(), aiEnd = a.end(), biEnd = b.end(); ai != aiEnd; ++ai)
		{
			const_iterator bi = b.find(ai->first);

			if((bi == biEnd) || !(*ai == *bi))  // We have to compare the values, because lookups are done by keys alone but the full value_type of a map is a key/value pair. 
				return false;                   // It's possible that two elements in the two containers have identical keys but different values.
		}

		return true;
	}

	template <typename Key, typename T, typename Hash, typename Predicate, memory_safety Safety, bool bCacheHashCode>
	inline bool operator!=(const hash_map<Key, T, Hash, Predicate, Safety, bCacheHashCode>& a, 
						   const hash_map<Key, T, Hash, Predicate, Safety, bCacheHashCode>& b)
	{
		return !(a == b);
	}


	template <typename Key, typename T, typename Hash, typename Predicate, memory_safety Safety, bool bCacheHashCode>
	inline bool operator==(const hash_multimap<Key, T, Hash, Predicate, Safety, bCacheHashCode>& a, 
						   const hash_multimap<Key, T, Hash, Predicate, Safety, bCacheHashCode>& b)
	{
		typedef typename hash_multimap<Key, T, Hash, Predicate, Safety, bCacheHashCode>::const_iterator const_iterator;
		typedef typename std::iterator_traits<const_iterator>::difference_type difference_type;

		// We implement branching with the assumption that the return value is usually false.
		if(a.size() != b.size())
			return false;

		// We can't simply search for each element of a in b, as it may be that the bucket for 
		// two elements in a has those same two elements in b but in different order (which should 
		// still result in equality). Also it's possible that one bucket in a has two elements which 
		// both match a solitary element in the equivalent bucket in b (which shouldn't result in equality).
		// std::pair<const_iterator, const_iterator> aRange;
		// std::pair<const_iterator, const_iterator> bRange;

		const_iterator ai = a.begin();
		const_iterator aiEnd = a.end();
		while(ai != aiEnd) // For each element in a...
		{
			std::pair<const_iterator, const_iterator> aRange = a.equal_range(ai->first); // Get the range of elements in a that are equal to ai.
			std::pair<const_iterator, const_iterator> bRange = b.equal_range(ai->first); // Get the range of elements in b that are equal to ai.

			// We need to verify that aRange == bRange. First make sure the range sizes are equivalent...
			const difference_type aDistance = std::distance(aRange.first, aRange.second);
			const difference_type bDistance = std::distance(bRange.first, bRange.second);

			if(aDistance != bDistance)
				return false;

			// At this point, aDistance > 0 and aDistance == bDistance.
			// Implement a fast pathway for the case that there's just a single element.
			if(aDistance == 1)
			{
				if(!(*aRange.first == *bRange.first)) // We have to compare the values, because lookups are done by keys alone but the full value_type of a map is a key/value pair. 
					return false;                     // It's possible that two elements in the two containers have identical keys but different values. Ditto for the permutation case below.
			}
			else
			{
				// Check to see if these aRange and bRange are any permutation of each other. 
				// This check gets slower as there are more elements in the range.
				if(!std::is_permutation(aRange.first, aRange.second, bRange.first))
					return false;
			}

			ai = aRange.second;
		}

		return true;
	}

	template <typename Key, typename T, typename Hash, typename Predicate, memory_safety Safety, bool bCacheHashCode>
	inline bool operator!=(const hash_multimap<Key, T, Hash, Predicate, Safety, bCacheHashCode>& a, 
						   const hash_multimap<Key, T, Hash, Predicate, Safety, bCacheHashCode>& b)
	{
		return !(a == b);
	}


} // namespace safe_memory::detail


#endif // Header include guard






