#include <TrynCppUnitTest.h>
#include <Core/src/gfx/Render/SinkAndSource.h>
#include <Core/src/gfx/Bindables/Texture.h>
#include <Core/src/gfx/Bindables/JITUpdateBuffer.h>
#include <type_traits>
#include <memory>

using namespace tryn::gfx;

namespace Gfx
{
	TEST_CLASS(SinkAndSourceTests)
	{
	public:
		TEST_METHOD(SinkCreation)
		{
			auto testSink = Sink(In<ITexture>("tex1"), In<JITUpdateBuffer>("buf1"));

			static_assert(std::is_same<std::tuple_element_t<0,decltype(testSink)::DependencyTypeTuple>, std::shared_ptr<ITexture>::element_type>::value);
			static_assert(std::is_same<std::tuple_element_t<1, decltype(testSink)::DependencyTypeTuple>, std::shared_ptr<JITUpdateBuffer>::element_type>::value);
		}
		TEST_METHOD(SourceCreation)
		{
			auto testSource = Source(Out<ITexture>("tex1"), Out<JITUpdateBuffer>("buf1"));
			decltype(testSource)::ExposureTuple testTuple;
			for (auto& element : testSource.names)
			{
				std::cout << element << "\n";
			}
		}
	};
}