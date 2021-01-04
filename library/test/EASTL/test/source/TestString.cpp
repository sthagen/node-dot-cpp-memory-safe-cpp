/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"
#include <EABase/eabase.h>
#include <EAStdC/EAMemory.h>
#include <EAStdC/EAString.h>
#include <string>
#include <algorithm>
//#include <EASTL/allocator_malloc.h>
#include <safe_memory/string.h>
#include <safe_memory/string_literal.h>

// namespace safe_memory {
// 	template<class T, class Alloc>
// 	using basic_string = std::basic_string<T, Alloc>;

// 	using string = std::string;
// 	using wstring = std::wstring;
// 	using u16string = std::u16string;
// 	using u32string = std::u32string;
// }

template<typename StringType>
bool validate(const StringType& str) {
	return str.validate();
}

//using namespace eastl;

// inject string literal string conversion macros into the unit tests
#define TEST_STRING_NAME TestBasicString
#define LITERAL_CHAR(x) x
#define LITERAL(x) StringType::literal_type(x)

#include "TestString.inl"

#define TEST_STRING_NAME TestBasicStringW
#define LITERAL_CHAR(x) EA_WCHAR(x) 
#define LITERAL(x) StringType::literal_type(EA_WCHAR(x))
#include "TestString.inl"

#define TEST_STRING_NAME TestBasicString16
#define LITERAL_CHAR(x) EA_CHAR16(x) 
#define LITERAL(x) StringType::literal_type(EA_CHAR16(x))
#include "TestString.inl"

#define TEST_STRING_NAME TestBasicString32
#define LITERAL_CHAR(x) EA_CHAR32(x) 
#define LITERAL(x) StringType::literal_type(EA_CHAR32(x))
#include "TestString.inl"

int TestString()
{
	int nErrorCount = 0;

	nErrorCount += TestBasicString<safe_memory::basic_string<char, safe_memory::memory_safety::safe>>();
	nErrorCount += TestBasicString<safe_memory::basic_string<char, safe_memory::memory_safety::none>>();
	nErrorCount += TestBasicString<safe_memory::basic_string_safe<char, safe_memory::memory_safety::safe>>();
	nErrorCount += TestBasicString<safe_memory::basic_string_safe<char, safe_memory::memory_safety::none>>();

	nErrorCount += TestBasicStringW<safe_memory::basic_string<wchar_t, safe_memory::memory_safety::safe>>();
	nErrorCount += TestBasicStringW<safe_memory::basic_string<wchar_t, safe_memory::memory_safety::none>>();
	nErrorCount += TestBasicStringW<safe_memory::basic_string_safe<wchar_t, safe_memory::memory_safety::safe>>();
	nErrorCount += TestBasicStringW<safe_memory::basic_string_safe<wchar_t, safe_memory::memory_safety::none>>();

	nErrorCount += TestBasicString16<safe_memory::basic_string<char16_t, safe_memory::memory_safety::safe>>();
	nErrorCount += TestBasicString16<safe_memory::basic_string<char16_t, safe_memory::memory_safety::none>>();
	nErrorCount += TestBasicString16<safe_memory::basic_string_safe<char16_t, safe_memory::memory_safety::safe>>();
	nErrorCount += TestBasicString16<safe_memory::basic_string_safe<char16_t, safe_memory::memory_safety::none>>();

#if EA_CHAR32_NATIVE
	nErrorCount += TestBasicString32<safe_memory::basic_string<char32_t, safe_memory::memory_safety::safe>>();
	nErrorCount += TestBasicString32<safe_memory::basic_string<char32_t, safe_memory::memory_safety::none>>();
	nErrorCount += TestBasicString32<safe_memory::basic_string_safe<char32_t, safe_memory::memory_safety::safe>>();
	nErrorCount += TestBasicString32<safe_memory::basic_string_safe<char32_t, safe_memory::memory_safety::none>>();
#endif

	// Check for memory leaks by using the 'CountingAllocator' to ensure no active allocation after tests have completed.
// 	CountingAllocator::resetCount();
// 	nErrorCount += TestBasicString<eastl::basic_string<char, CountingAllocator>>();
// 	VERIFY(CountingAllocator::getActiveAllocationCount() == 0); 

// 	nErrorCount += TestBasicStringW<eastl::basic_string<wchar_t, CountingAllocator>>();
// 	VERIFY(CountingAllocator::getActiveAllocationCount() == 0); 

// 	nErrorCount += TestBasicString16<eastl::basic_string<char16_t, CountingAllocator>>();
// 	VERIFY(CountingAllocator::getActiveAllocationCount() == 0); 

// #if EA_CHAR32_NATIVE
// 	nErrorCount += TestBasicString32<eastl::basic_string<char32_t, CountingAllocator>>();
// 	VERIFY(CountingAllocator::getActiveAllocationCount() == 0); 
// #endif

	// to_string
	// {
	// 	VERIFY(eastl::to_string(42)    == "42");
	// 	VERIFY(eastl::to_string(42l)   == "42");
	// 	VERIFY(eastl::to_string(42ll)  == "42");
	// 	VERIFY(eastl::to_string(42u)   == "42");
	// 	VERIFY(eastl::to_string(42ul)  == "42");
	// 	VERIFY(eastl::to_string(42ull) == "42");
	// 	VERIFY(eastl::to_string(42.f)  == "42.000000");
	// 	VERIFY(eastl::to_string(42.0)  == "42.000000");
	// #ifndef EA_COMPILER_GNUC
	// 	// todo:  long double sprintf functionality is unrealiable on unix-gcc, requires further debugging.  
	// 	VERIFY(eastl::to_string(42.0l) == "42.000000");
	// #endif
	// }

	// to_wstring
	// {
	// 	VERIFY(eastl::to_wstring(42)    == L"42");
	// 	VERIFY(eastl::to_wstring(42l)   == L"42");
	// 	VERIFY(eastl::to_wstring(42ll)  == L"42");
	// 	VERIFY(eastl::to_wstring(42u)   == L"42");
	// 	VERIFY(eastl::to_wstring(42ul)  == L"42");
	// 	VERIFY(eastl::to_wstring(42ull) == L"42");
	// 	VERIFY(eastl::to_wstring(42.f)  == L"42.000000");
	// 	VERIFY(eastl::to_wstring(42.0)  == L"42.000000");
	// #ifndef EA_COMPILER_GNUC
	// 	// todo:  long double sprintf functionality is unrealiable on unix-gcc, requires further debugging.  
	// 	VERIFY(eastl::to_wstring(42.0l) == L"42.000000");
	// #endif
	// }

	// #if EASTL_USER_LITERALS_ENABLED 
	// {
	// 	VERIFY("cplusplus"s == "cplusplus");
	// 	VERIFY(L"cplusplus"s == L"cplusplus");
	// 	VERIFY(u"cplusplus"s == u"cplusplus");
	// 	VERIFY(U"cplusplus"s == U"cplusplus");
	// }
	// #endif


	// {
	// 	// CustomAllocator has no data members which reduces the size of an eastl::basic_string via the empty base class optimization.
	// 	typedef eastl::basic_string<char, CustomAllocator> EboString;

	// 	// this must match the eastl::basic_string heap memory layout struct which is a pointer and 2 size_t.
	// 	const int expectedSize = sizeof(EboString::pointer) + (2 * sizeof(EboString::size_type));

	// 	static_assert(sizeof(EboString) == expectedSize, "unexpected layout size of basic_string");
	// }

	return nErrorCount;
}


