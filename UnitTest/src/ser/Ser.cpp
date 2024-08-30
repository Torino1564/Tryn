#include <TrynCppUnitTest.h>
#include <vector>
#include <sstream>
#include <fstream>

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
			template <typename Data = void>
			static TrivialClass Read(const ser::StreamReader& streamReader, const bool binary = true, const Data* pExtraData = nullptr)
			{
				TrivialClass newClass;
				newClass.var1 = streamReader.ReadSerialized<int>(binary);
				newClass.var2 = streamReader.ReadSerialized<float>(binary);
				newClass.floatArray = streamReader.ReadSerialized<std::vector<float>>(binary);
				return newClass;
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
			static NonTrivialClass Read(const ser::StreamReader& streamReader, const bool binary = true, const void* pExtraData = nullptr)
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
			app::BootCore();
			pStreamWriter = std::make_unique<ser::StreamWriter>(oss);
			pWnd = ioc::Get().Resolve<win::IWindow>(win::IWindow::IocParams{ .size = spa::DimensionsI{.width = (1024), .height = (768) } });
			pWnd->SetTitle(L"TrynGame");
			pGfx = ioc::Get().Resolve<gfx::IGraphics>(gfx::IGraphics::IocParams{ pWnd->GetClientDimensions().width, pWnd->GetClientDimensions().height, pWnd->GetHandle() });
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
		TEST_METHOD(SerializeString)
		{
			std::ostringstream oss;
			ser::StreamWriter streamWriter(oss);

			std::string str1 = "Hello World";
			std::string str2 = "TrynEngine Coming in";

			streamWriter.Serialize(str1, true);
			streamWriter.Serialize(str2, true);

			auto fileWrite = std::ofstream("SerializeStringTestOutput.txt", std::ios::binary);
			fileWrite << oss.str();
			fileWrite.close();

			auto fileRead = std::ifstream("SerializeStringTestOutput.txt", std::ios::binary);

			std::stringstream buffer;
			buffer << fileRead.rdbuf();

			fileRead.close();

			std::string fileContent = buffer.str();

			std::istringstream iss(fileContent);

			ser::StreamReader streamReader(iss);
			
			auto str1FromFile = streamReader.ReadSerialized<std::string>(true);
			auto str2FromFile = streamReader.ReadSerialized<std::string>(true);
		}
		TEST_METHOD(SerliazeEntity)
		{
			auto ent1 = ecs::Entity::CreateNew<ecs::cmp::ActiveComponent, ecs::cmp::PositionComponent>("ent1");

			std::ostringstream oss2;
			ser::StreamWriter streamWriter2(oss2);

			ent1.GetComponent<ecs::cmp::ActiveComponent>().active = true;
			ent1.GetComponent<ecs::cmp::PositionComponent>().position = {69.0f, 420.0f, 1337.7f};

			streamWriter2.Serialize(ent1);

			auto fileWrite = std::ofstream("SerializeEntityOutput.txt", std::ios::binary);
			fileWrite << oss2.str();
			fileWrite.close();

			auto fileRead = std::ifstream("SerializeEntityOutput.txt", std::ios::binary);

			std::stringstream buffer;
			buffer << fileRead.rdbuf();

			fileRead.close();

			std::string fileContent = buffer.str();

			std::istringstream iss(fileContent);

			ser::StreamReader streamReader(iss);

			struct EntityExtraData
			{
				gfx::IGraphics* pGfx;
			};

			ser::ExtraDataPack data;

			data.AddElement(ser::ElementDataView(*pGfx.get(), "gfx"));

			auto ent2 = streamReader.ReadSerialized<ecs::Entity>(true, &data);

			auto [activeComp] = ent2.GetComponent<ecs::cmp::ActiveComponent>();
			auto [positionComp] = ent2.GetComponent<ecs::cmp::PositionComponent>();
			auto componentSpan = ent2.GetComponents();
		}
	public:
		std::shared_ptr<win::IWindow> pWnd;
		std::shared_ptr<gfx::IGraphics> pGfx;
		std::ostringstream oss;
		std::unique_ptr<ser::StreamWriter> pStreamWriter;
	};
}