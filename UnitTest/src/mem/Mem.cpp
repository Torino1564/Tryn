#include <TrynCppUnitTest.h>
#include <Core/src/mem/ArenaAllocator.h>
#include <array>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace tryn;
using namespace std::string_literals;

namespace Mem
{
	TEST_CLASS(AllocatorTests)
	{
	public:
		TEST_METHOD_INITIALIZE(Init)
		{
			
		}
		TEST_METHOD(Arena)
		{
			auto ptr1 = allocator.MakeNew<int>();
			*ptr1 = 1;
			auto ptr2 = allocator.MakeNew<int>();
			*ptr2 = 2;
			auto ptr3 = allocator.MakeNew<int>();
			*ptr3 = 3;
			auto ptr4 = allocator.MakeNew<int>();
			*ptr4 = 4;
			auto ptr5 = allocator.MakeNew<int>();
			*ptr5 = 5;

			auto pArray = allocator.MakeNew<std::array<int, 100>>();
		}
	public:
		mem::ArenaAllocator<> allocator;
	};
}