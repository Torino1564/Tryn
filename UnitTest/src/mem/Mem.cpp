#include <TrynCppUnitTest.h>
#include <Core/src/mem/BasicPoolAllocator.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace tryn;
using namespace std::string_literals;

namespace Mem
{
	TEST_CLASS(BasicPoolAllocatorTests)
	{
	public:
		TEST_METHOD(GenericTest)
		{
			mem::BasicPoolAllocator<> pool(16);
			pool.Request(1);
		}
	};
}