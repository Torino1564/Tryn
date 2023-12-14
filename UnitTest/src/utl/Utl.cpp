#include <TrynCppUnitTest.h>
#include <Core/src/utl/UUID.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace tryn;

namespace Utl
{
	TEST_CLASS(DiverseUtlTests)
	{
		TEST_METHOD(UUIDTests)
		{
			const auto s1 = utl::UUIDGenerator::Generate();
			const auto s2 = utl::UUIDGenerator::Generate();
			const auto s3 = utl::UUIDGenerator::Generate();
			const auto s4 = utl::UUIDGenerator::Generate();
		}
	};
}