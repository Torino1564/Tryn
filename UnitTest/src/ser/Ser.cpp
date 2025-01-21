#include <TrynCppUnitTest.h>
#include <vector>
#include <sstream>
#include <fstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace tryn;
using namespace std::string_literals;

namespace Ser
{
	TEST_CLASS(SerialzierTest)
	{
	public:
		
	public:
		std::shared_ptr<win::IWindow> pWnd;
		std::shared_ptr<gfx::IGraphics> pGfx;
		std::ostringstream oss;
		std::unique_ptr<ser::StreamWriter> pStreamWriter;
	};
}