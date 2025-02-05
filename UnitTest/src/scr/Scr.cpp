#include <TrynCppUnitTest.h>
#include <Core/src/scr/ScriptVariable.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace tryn::scr;

namespace Scr
{
	TEST_CLASS(ScriptingTests)
	{
	public:
		TEST_METHOD(CreateVariableBag)
		{
			ScriptVariableBag bag;

			bag.Insert<float>("hitPoints");
			bag.Insert<float>("manaPoints");
			bag.Insert<uint16_t>("level");

			auto& hp = bag.Get<float>("hitPoints");
			auto& mana = bag.Get<float>("manaPoints");
			auto& level = bag.Get<uint16_t>("level");

			hp = 481;
			mana = 931;
			level = 60;

			auto& hp2 = bag.Get<float>("hitPoints");
			auto& mana2 = bag.Get<float>("manaPoints");
			auto& level2 = bag.Get<uint16_t>("level");
		}
	};
}