#include <TrynCppUnitTest.h>
#include <Core/src/ser/Serializer.h>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace tryn;
using namespace std::string_literals;

namespace Ser
{
	struct TrivialClass
	{
		int var1 = 69;
		float var2 = 420.0f;
		void DoSomething() {}
	};

	struct NonTrivialClass
	{
		std::unique_ptr<int> pInt = std::make_unique<int>(69);

		class Serializer : public ser::Serializer<NonTrivialClass>
		{
		public:
			static void Write(ser::StreamWriter& streamWriter, const NonTrivialClass& data)
			{
			/*	streamWriter.Serialize(data.var1);
				streamWriter.Serialize(data.var2);*/
				streamWriter.Serialize(data.pInt);
			}
		};
	};

	TEST_CLASS(SerialzierTest)
	{
	public:
		TEST_METHOD_INITIALIZE(Init)
		{
			pStreamWriter = std::make_unique<ser::StreamWriter>(oss);
		}
		TEST_METHOD(SerializeTest)
		{
			static_assert(std::is_trivially_copyable_v<TrivialClass>);
			static_assert(!std::is_trivially_copyable_v<NonTrivialClass>);

			TrivialClass tc;
			NonTrivialClass ntc;
			//ntc.var1 = 1337;
			//ntc.var2 = 1337.7777f;


			pStreamWriter->Serialize(tc);
			pStreamWriter->Serialize(ntc);

			auto file = std::ofstream("SerializeTestOutput.txt", std::ios::binary);
			file << oss.str();
		}
	public:
		std::ostringstream oss;
		std::unique_ptr<ser::StreamWriter> pStreamWriter;
	};
}