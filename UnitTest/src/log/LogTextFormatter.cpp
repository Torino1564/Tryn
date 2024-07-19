#include <TrynCppUnitTest.h>
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

	};
}