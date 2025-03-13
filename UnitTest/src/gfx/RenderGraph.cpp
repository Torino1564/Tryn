#include "TrynCppUnitTest.h"
#include <Core/src/win/WindowClass.h>

using namespace tryn::gfx;
using namespace tryn;
namespace Gfx
{
	class TestPass : public RenderQueuePass
	{
	public:
		TestPass(IRenderGraph& graph, std::string name)
			:
			RenderQueuePass(std::move(name), graph, {"TestQueue"})
		{
			// declare sink and source
			pSink = std::make_unique<SinkType>(In<IShaderResourceRenderTargetView>("rtv"));
			pSource = std::make_unique<SourceType>(Out<IShaderResourceRenderTargetView>("rtv"));

			// declare queues to utilize
			queueNames.push_back("TestQueue");
		}
		void Execute(const IGraphics& gfx) override
		{
			// bind Render Target View
			auto& concreteSink = *reinterpret_cast<SinkType*>(pSink.get());
			auto& pRTV = concreteSink.Get<IShaderResourceRenderTargetView>("rtv");
			pRTV->Bind();

			auto& concreteSource = *reinterpret_cast<SourceType*>(pSource.get());
			concreteSource.Set<IShaderResourceRenderTargetView>(pRTV, "rtv");
		}
	private:
		using SinkType = Sink<In<IShaderResourceRenderTargetView>>;
		using SourceType = Source<Out<IShaderResourceRenderTargetView>>;
	};

	class TestRenderGraph : public IRenderGraph
	{
	public:
		TestRenderGraph(const IGraphics& gfx)
			:
			IRenderGraph(gfx)
		{
			AddPass(TestPass(*this, "testPass"));
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