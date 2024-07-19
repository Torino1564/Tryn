#include <TrynCppUnitTest.h>
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
		std::vector<float> floatArray = {420.0f, 69.9f, 1337.7f};

		void DoSomething() {}

		ZT_DEFINE_SERIALIZER(TrivialClass)
		{
			static void Write(const ser::StreamWriter& streamWriter, const TrivialClass& data, const bool binary = true, const std::string& name = "")
			{
				streamWriter.Serialize(data.var1, binary, name);
				streamWriter.Serialize(data.var2, binary, name);
				streamWriter.Serialize(data.floatArray, binary, name);
			}
			static TrivialClass Read(const ser::StreamReader& streamReader, const bool binary = true)
			{
				TrivialClass newClass;
				newClass.var1 = streamReader.ReadSerialized<int>(binary);
				newClass.var2 = streamReader.ReadSerialized<float>(binary);
				newClass.floatArray = streamReader.ReadSerialized<std::vector<float>>(binary);
				return std::move(newClass);
			}
		};
	};

	struct NonTrivialClass
	{
		int var1;
		float var2;
		std::unique_ptr<int> pInt = std::make_unique<int>(69);

		ZT_DEFINE_SERIALIZER(NonTrivialClass)
		{
			static void Write(const ser::StreamWriter& streamWriter, const NonTrivialClass& data, const bool binary = true, const std::string& name = "")
			{
				streamWriter.Serialize(data.var1, binary, name);
				streamWriter.Serialize(data.var2, binary, name);
				streamWriter.Serialize(data.pInt, binary, name);
			}
			static NonTrivialClass Read(const ser::StreamReader& streamReader, const bool binary = true)
			{
				NonTrivialClass newClass;
				newClass.var1 = streamReader.ReadSerialized<int>(binary);
				newClass.var2 = streamReader.ReadSerialized<float>(binary);
				streamReader.ReadSerialized<std::unique_ptr<int>>(newClass.pInt, binary);

				return std::move(newClass);
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
		TEST_METHOD(SerializeTest1)
		{
			TrivialClass tc;
			tc.var1 = 420;
			tc.var2 = 6.9f;
			NonTrivialClass ntc;
			ntc.var1 = 1337;
			ntc.var2 = 1337.7777f;

			static_assert(ser::HasTypeSerializer<std::vector<int>>);

			pStreamWriter->Serialize(tc);
			pStreamWriter->Serialize(ntc);


			auto fileWrite = std::ofstream("SerializeTestOutput.txt", std::ios::binary);
			fileWrite << oss.str();
			fileWrite.close();

			auto fileRead = std::ifstream("SerializeTestOutput.txt", std::ios::binary);

			std::stringstream buffer;
			buffer << fileRead.rdbuf();

			fileRead.close();

			std::string fileContent = buffer.str();

			std::istringstream iss(fileContent);

			ser::StreamReader streamReader(iss);

			auto tc1 = streamReader.ReadSerialized<TrivialClass>();
			auto tc2 = streamReader.ReadSerialized<NonTrivialClass>();
		}
		TEST_METHOD(SerliazeTest2)
		{
			auto ent1 = ecs::Entity::CreateNew<ecs::cmp::ActiveComponent, ecs::cmp::PositionComponent>("ent1");

			std::ostringstream oss2;
			ser::StreamWriter streamWriter2(oss2);

			const ecs::Archetype* pArch = nullptr;
			ser::TypeSerializer<ecs::Archetype*>::Write(streamWriter2, pArch);

			streamWriter2.Serialize(ent1);

			auto fileWrite = std::ofstream("SerializeTestOutput2.txt", std::ios::binary);
			fileWrite << oss2.str();
			fileWrite.close();

			auto fileRead = std::ifstream("SerializeTestOutput.txt", std::ios::binary);

			std::stringstream buffer;
			buffer << fileRead.rdbuf();

			fileRead.close();

			std::string fileContent = buffer.str();

			std::istringstream iss(fileContent);

			ser::StreamReader streamReader(iss);

			auto ent2 = streamReader.ReadSerialized<ecs::Entity>();
			auto componentSpan = ent2.GetComponents();
	
		}
	public:
		std::ostringstream oss;
		std::unique_ptr<ser::StreamWriter> pStreamWriter;
	};
}