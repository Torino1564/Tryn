#include <TrynCppUnitTest.h>
#include <Core/src/log/Entry.h>
#include <Core/src/log/TextFormatter.h>
#include <chrono>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace tryn;
using namespace std::string_literals;

#define trylog log::EntryBuilder{ __FILEW__ , __FUNCTIONW__, __LINE__ }

namespace Log
{
	TEST_CLASS(LogTextFormatterTests)
	{
	public:
		TEST_METHOD(TestFormat)
		{
			const log::Entry e{
				.level_ = log::Level::Info,
					.note_ = L"HI",
					.sourceFile_ = __FILEW__,
					.sourceFunctionName_ = __FUNCTIONW__,
					.sourceLine_ = __LINE__,
					.timestamp_ = std::chrono::system_clock::time_point{
						std::chrono::days{10'000} }
			};

			Assert::AreEqual(L"@Info {1997-05-18 21:00:00.0000000 GMT-3} HI\n  >> at Log::LogTextFormatterTests::TestFormat\n     D:\\dev\\Tryn\\UnitTest\\src\\log\\LogTextFormatter.cpp(26)\n"s,
				log::TextFormatter{}.Format(e)
			);
		}
	};
}