#include "TrynCppUnitTest.h"
#include <Core/src/win/WindowClass.h>

using namespace tryn::gfx;
using namespace tryn;
namespace Gfx
{
	class TestRenderGraph : public IRenderGraph
	{
	public:
		TestRenderGraph(const IGraphics& gfx)
			:
			IRenderGraph(gfx)
		{
			AddLinkage(LinkageParam{ .passName = "global", .resourceName = "rtv" },
				LinkageParam{ .passName = "testPass", .resourceName = "rtv" });
			
			AddLinkage(LinkageParam{ .passName = "testPass", .resourceName = "rtv" },
				LinkageParam{ .passName = "global",.resourceName = "rtv" });
		}
	};

	TEST_CLASS(RenderGraphTests)
	{
		TEST_METHOD_INITIALIZE(Initt)
		{
			app::BootCore();
			pWnd = std::make_unique<win::Window>(std::make_shared<win::WindowClass>(), L"Test window", spa::DimensionsI(300, 300));
			pGfx = std::make_unique<gfx::dx11::Graphics>(pWnd->GetHandle(), pWnd->GetClientDimensions().width, pWnd->GetClientDimensions().height);
		}
		TEST_METHOD(RenderGraphTest)
		{
			TestRenderGraph renderGraph(*pGfx);
			renderGraph.AddCamera(nullptr);

			renderGraph.ExecuteFrame(*pGfx);
			
		}
	private:
		std::unique_ptr<gfx::IGraphics> pGfx;
		std::unique_ptr<win::Window> pWnd;
	};
}