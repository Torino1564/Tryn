#include "TrynPCH.h"
#include "HoverEntitySystem.h"

#include "Core/src/ecs/cmp/ComponentManager.h"
#include "Core/src/gfx/IGraphics.h"
#include "Core/src/win/IWindow.h"
#include <Core/src/gfx/Render/Passes/EntityIDPass.h>

#include "Core/src/gfx/Bindables/TextureResource.h"

namespace tryn::ecs
{
	template <>
	struct SystemState<HoverEntitySystem>
	{
		const gfx::IGraphics* gfx;
		const win::IWindow* wnd;
		const gfx::IRenderGraph* renderGraph;
		gfx::IRenderPass* pPass;
		const gfx::IRenderTargetView* pRTV;
		std::shared_ptr<gfx::ITexture> pStagingTexture;
	};

	HoverEntitySystem::HoverEntitySystem(const SystemGraph& graph)
		:
	SystemImpl(graph)
	{
		const auto& pGfx = pEcs->GetComponentManager().RequestSingleton<std::shared_ptr<gfx::IGraphics>>();
		const auto& renderGraph = pGfx->GetRenderGraph();
		const auto it = std::ranges::find_if(renderGraph.GetPasses(), [](const auto& pPass)
			{
				return pPass->GetName() == gfx::EntityIDPass::PASS_NAME;
			});

		trynass(it != renderGraph.GetPasses().end());
		
		auto& rtv = it->get()->GetSource().Get<gfx::IRenderTargetView>(gfx::EntityIDPass::RESOURCE_NAME);

		auto pTex = std::shared_ptr<gfx::Texture>(new gfx::Texture({ .width = 50, .height = 50 }, gfx::TextureFormat::R32G32B32A32_UINT));
		

		SystemState<HoverEntitySystem> state{
			.gfx = pGfx.get(),
			.wnd = pEcs->GetComponentManager().RequestSingleton<std::shared_ptr<win::IWindow>>().get(),
			.renderGraph = &renderGraph,
			.pPass = it->get(),
			.pRTV = &rtv
		};

		pEcs->GetComponentManager().RegisterSingleton(std::move(state));
	}

	void HoverEntitySystem::InitDependencies(System* self)
	{
	}

	void HoverEntitySystem::Execute()
	{
		// Check if its time to update
		if (Tick() < tickInterval)
			return;

		// Logic
		auto& [gfx, wnd, renderGraph, pass, rtv, pTex] = pEcs->GetComponentManager().RequestSingleton<SystemState<HoverEntitySystem>>();


	}
}
