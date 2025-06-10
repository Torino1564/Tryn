#pragma once
#include <Core/src/gfx/Render/RenderPass.h>

#include "Core/src/gfx/Bindables/IBufferBase.h"
#include <Core/src/utl/StatefulMeta/TemplateData.h>
#include <Core/src/gfx/RTVDSFwd.h>
#include <type_traits>
#include <functional>

namespace tryn::gfx
{
	template <typename T>
	concept IsRenderTarget = std::derived_from<T, IGenericRenderTargetView> || std::is_same_v<T, IGenericRenderTargetView>;

	template <typename T>
	concept IsDepthStencil = std::derived_from<T, IGenericDepthStencil> || std::is_same_v<T, IGenericDepthStencil>;

	template <typename T>
	concept RenderTargetOrDepthStencil = IsDepthStencil<T> || IsRenderTarget<T>;

	class ClearTargetPass : public IRenderPass
	{
	public:
		ClearTargetPass(const std::string& name)
			:
			IRenderPass(name) {}
		template <RenderTargetOrDepthStencil T, typename Func = void>
			requires std::invocable<Func> || std::is_same_v<Func, void>
		void AddTarget(const std::string& name);

		void Execute(const IGraphics& gfx) override
		{
			for (const auto& func : pFuncs)
			{
				func(pSink.get(), pSource.get());
			}
		}

		std::vector<std::function<void(const Sink*, Source*)>> pFuncs;
	};

	template <RenderTargetOrDepthStencil T, typename Func>
		requires std::invocable<Func> || std::is_same_v<Func, void>
	void ClearTargetPass::AddTarget(const std::string& name)
	{
		using namespace std::placeholders;
		pSink->AddDependency<T>(name);
		pSource->AddExposure<T>(name);
		if constexpr (std::is_same_v<Func, void>)
		{
			auto func = std::bind([](const Sink* pSink, Source* pSource, const std::string& name)
				{
					auto& target = pSink->Get<T>(name);
					target.Clear();
					pSource->Set(target, name);
				}, _1, _2, std::move(name));

			pFuncs.emplace_back(func);
		}
		else
		{
			static constexpr Func func;
			pFuncs.emplace_back(func);
		}
		
	}
}
