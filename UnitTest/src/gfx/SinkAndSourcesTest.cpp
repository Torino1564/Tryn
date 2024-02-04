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
			decltype(testSink)::DependencyTuple testTuple;
			for (auto& element : testSink.namesAndPolicy)
			{
				std::cout << element.first << "Policy: [" << (int)element.second << "]\n";
			}

			static_assert(std::is_same<std::remove_reference_t<decltype(std::get<0>(testTuple))>, std::shared_ptr<ITexture>>::value);
			static_assert(std::is_same<std::remove_reference_t<decltype(std::get<1>(testTuple))>, std::shared_ptr<JITUpdateBuffer>>::value);
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