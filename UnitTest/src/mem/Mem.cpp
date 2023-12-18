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
			auto ptr1 = allocator.Allocate(40);
			auto ptr2 = allocator.Allocate(240);
			auto ptr3 = allocator.Allocate(430);
			auto ptr4 = allocator.Allocate(440);
			auto ptr5 = allocator.Allocate(140);

			auto pArray = allocator.MakeNew<std::array<int, 100>>();
		}
	public:
		mem::ArenaAllocator<> allocator;
	};
}