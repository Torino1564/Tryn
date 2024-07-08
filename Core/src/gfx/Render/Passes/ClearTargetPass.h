#pragma once
#include <Core/src/gfx/Render/RenderPass.h>

#include "Core/src/gfx/Bindables/IBuffer.h"
#include <Core/src/utl/StatefulMeta/TemplateData.h>

namespace tryn::gfx
{
	template <typename T>
	concept IsRenderTarget = std::derived_from<T, IGenericRenderTargetView> || std::is_same_v<T, IGenericRenderTargetView>;

	template <typename T>
	concept IsDepthStencil = std::derived_from<T, IGenericDepthStencil> || std::is_same_v<T, IGenericDepthStencil>;

	template <typename T>
	concept RenderTargetOrDepthStencil = IsDepthStencil<T> || IsRenderTarget<T>;

	template <typename T, utl::StaticString Name>
		requires RenderTargetOrDepthStencil<T>
	struct TargetIn
	{
		using Type = T;
		static constexpr const utl::StaticString<Name.n> staticString = Name;
		static constexpr const char* name = Name.v;
	};

	template <typename T>
	struct IsTargetIn : std::false_type{};

	template <typename T>
	requires RenderTargetOrDepthStencil<typename T::Type>
	struct IsTargetIn<T> : std::true_type{};

	template <typename T>
	concept TargetInType = IsTargetIn<T>::value;

	template <TargetInType... Param>
	class ClearTargetPass : public IRenderPass
	{
	public:
		ClearTargetPass(const std::string& name)
			:
			IRenderPass(name)
		{
			pSink = std::make_unique<SinkType>(In<typename Param::Type>(Param::name)...);
			pSource = std::make_unique<SourceType>(Out<typename Param::Type>(Param::name)...);
		}
		void Execute(IGraphics& gfx) override
		{
			auto pConcreteSink = reinterpret_cast<SinkType*>(pSink.get());

			auto& tupleInQuestion = pConcreteSink->GetDependencyTuple();

			std::apply([&](auto&&... ppArg){ ((**ppArg)->Clear(), ...); BindSourceElement(forward_as_tuple(ppArg...));}, tupleInQuestion);
		}

		template <unsigned N = 0, typename... Args>
		void BindSourceElement(std::tuple<Args...>&& ppArg)
		{
			if constexpr (N < std::tuple_size_v<std::tuple<Args...>>)
			{
				auto pConcreteSource = reinterpret_cast<SourceType*>(pSource.get());
				auto& pTarget = **std::get<N>(ppArg);
				auto nthDesignator = std::tuple_element_t<N, BufferParamTuple>::name;
				pConcreteSource->Set(pTarget, nthDesignator);
				return BindSourceElement<N + 1>(std::forward<std::tuple<Args...>>(ppArg));
			}
		}

	private:
		using BufferParamTuple = std::tuple<Param...>;
		using SinkType = Sink<In<typename Param::Type>...>;
		using SourceType = Source<Out<typename Param::Type>...>;
	};
}
