#include "TrynCppUnitTest.h"
#include <Core/src/gfx/Render/RenderGraph.h>
#include <Core/src/gfx/Render/Passes/ForwardLambertianPass.h>
#include <Core/src/app/App.h>
#include <Core/src/app/Initialization.h>
#include <Core/src/win/Window.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>

using namespace tryn::gfx;
using namespace tryn;
namespace Gfx
{
	class TestPass : public IRenderPass
	{
	public:
		TestPass(const std::string& name)
			:
			IRenderPass(std::move(name))
		{
			// declare sink and source
			pSink = std::make_unique<SinkType>(In<IRenderTargetView>("rtv"));
			pSource = std::make_unique<SourceType>(Out<IRenderTargetView>("rtv"));

			// declare queues to utilize
			queueNames.push_back("TestQueue");
		}
		void Execute(IGraphics& gfx)
		{
			// bind Render Target View
			auto& concreteSink = *reinterpret_cast<SinkType*>(pSink.get());
			auto pRTV = concreteSink.Get<IRenderTargetView>("rtv");
			pRTV->Bind();

			auto& concreteSource = *reinterpret_cast<SourceType*>(pSource.get());
			concreteSource.Set<IRenderTargetView>(pRTV, "rtv");
		}
	private:
		using SinkType = Sink<In<IRenderTargetView>>;
		using SourceType = Source<Out<IRenderTargetView>>;
	};

	class TestRenderGraph : public IRenderGraph
	{
	public:
		TestRenderGraph(IGraphics& gfx)
			:
			IRenderGraph(gfx)
		{
			AddPass<ForwardLambertianPass>("lambertian");
			AddLinkage(LinkageParam{ .passName = "global", .resourceName = "rtv" },
				LinkageParam{ .passName = "lambertian", .resourceName = "rtv" });
			
			AddLinkage(LinkageParam{ .passName = "lambertian", .resourceName = "rtv" },
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
		}
	private:
		std::unique_ptr<gfx::dx11::Graphics> pGfx;
		std::unique_ptr<win::Window> pWnd;
	};
}