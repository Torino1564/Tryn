#include "TrynCppUnitTest.h"

using namespace tryn::gfx;

using cbType = ConstantBufferLayout::Type;

namespace Gfx
{
	TEST_CLASS(GfxTests)
	{
	public:
		TEST_METHOD(DcbTest)
		{
			ConstantBufferLayout layout;
			layout.Append(cbType::Array, "InstanceArray");

			ConstantBufferLayout::Node arrayElement(cbType::Struct, "arrayElement");
			arrayElement.Append(cbType::Float, "intensity");

			layout["InstanceArray"].Set(arrayElement, 100);
			layout.Solidify();

			ConstantBuffer testBuffer(std::move(layout));
			testBuffer["InstanceArray"][0]["intensity"].Get<float>() = 69.0f;
			testBuffer["InstanceArray"][1]["intensity"].Get<float>() = 420.0f;
			testBuffer["InstanceArray"][2]["intensity"].Get<float>() = 69.420f;
			testBuffer["InstanceArray"][3]["intensity"].Get<float>() = 69.420f;
			testBuffer["InstanceArray"][4]["intensity"].Get<float>() = 69.420f;

			auto test1 = testBuffer["InstanceArray"][0]["intensity"].Get<float>();
			auto test2 = testBuffer["InstanceArray"][1]["intensity"].Get<float>();
			auto test3 = testBuffer["InstanceArray"][2]["intensity"].Get<float>();

			testBuffer["InstanceArray"].Resize(150);
			testBuffer["InstanceArray"][149]["intensity"].Get<float>() = 69696969.420420f;
			
			test1 = testBuffer["InstanceArray"][0]["intensity"].Get<float>();
			test2 = testBuffer["InstanceArray"][1]["intensity"].Get<float>();
			test3 = testBuffer["InstanceArray"][2]["intensity"].Get<float>();
		}
	};
}