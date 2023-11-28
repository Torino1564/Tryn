#include <TrynCppUnitTest.h>
#include <memory>
#include <Core/src/ent/Landscape.h>

using namespace tryn;


namespace ent
{
	TEST_CLASS(IocContainerTests)
	{
	public:
		TEST_METHOD_INITIALIZE(Init)
		{
		}
		TEST_METHOD(HeightMapTest)
		{
			auto test = tryn::ent::Landscape("D:/dev/Tryn/UnitTest/resources/levels/HeightmapTest.png");
		}
	private:
	};
}