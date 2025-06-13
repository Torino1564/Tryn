#include "TrynPCH.h"
#include "IGraphics.h"
#include <Core/src/log/Log.h>
#include <Core/src/gfx/Render/DefaultRenderGraphs/DefaultRenderGraph.h>
#include <Core/third/glm/ext/matrix_clip_space.hpp>
#include <Core/src/win/TrynWin.h>
#include <Core/src/gfx/Bindables/IBufferBase.h>
#include <Core/src/gfx/Bindables/SOAVertexBuffer.h>
#include <Core/src/gfx/Bindables/DepthStencil.h>
#include <Core/src/gfx/Bindables/RenderTargetView.h>
#include <Core/src/gfx/Bindables/TextureResource.h>
#include <Core/src/gfx/Bindables/PixelShader.h>
#include <Core/src/gfx/Bindables/VertexShader.h>
#include <Core/src/gfx/Bindables/InputLayout.h>
#include <Core/src/gfx/Bindables/Sampler.h>
#include <Core/src/gfx/Bindables/Rasterizer.h>
#include <Core/src/gfx/Bindables/JITUpdateBuffer.h>
#include <Core/src/gfx/Bindables/PrimitiveTopology.h>
#include <Core/src/gfx/Bindables/TransformCBuf.h>

namespace tryn::gfx
{
	template <typename Interface, typename ParameterTupleTuples, unsigned N = 0>
	static void AppendResolver(std::array<std::any, IGraphics::MAX_CONSTRUCTORS_ALLOWED>& rttiTableEntry)
	{
		if constexpr (N < std::min(std::tuple_size_v<ParameterTupleTuples>, static_cast<size_t>(IGraphics::MAX_CONSTRUCTORS_ALLOWED)))
		{
			using ParameterTuple = std::tuple_element_t<N, ParameterTupleTuples>;
			rttiTableEntry[N] = std::any( static_cast<std::shared_ptr<Interface>(*)(const IGraphics&, ParameterTuple)>(nullptr) );
			return AppendResolver<Interface, ParameterTupleTuples, N + 1>(rttiTableEntry);
		}
	}

	template <unsigned N = 0>
	static void AppendBindableRTTI(std::array<std::array<std::any, IGraphics::MAX_CONSTRUCTORS_ALLOWED>, std::tuple_size_v<IGraphics::SupportedBindables>>& rttiTable)
	{
		if constexpr (N < std::tuple_size_v<IGraphics::SupportedBindables>)
		{
			using Register = std::tuple_element_t<N, IGraphics::SupportedBindables>;
			using Interface = typename Register::Bindable_t;
			using ParameterTuplesTuple = typename Register::ParameterTuplesTuple_t;
			auto& rttiTableEntry = rttiTable[N];
			if constexpr (std::tuple_size_v<ParameterTuplesTuple> == 0)
			{
				// The case where no tuple parameters are specified means that it should get the static Resolve Method
				rttiTableEntry[0] = std::any{ static_cast<std::shared_ptr<Interface>(*)(const IGraphics&, typename utl::MethodArgTupleMinusFirst<decltype(&Interface::Resolve)>::t)>(nullptr) };
			}
			else
			{
				// Else loop over tuple elements and append them
				AppendResolver<Interface, ParameterTuplesTuple>(rttiTableEntry);
			}
			return AppendBindableRTTI<N + 1>(rttiTable);
		}
	}
	
	IGraphics::IGraphics()
	{
		// Init bindable constructors RTTI
		AppendBindableRTTI<>(bindableConstructorsRTTI);
	}

	void IGraphics::SetRenderGraph(std::unique_ptr<IRenderGraph>&& renderGraph_p)
	{
		pRenderGraph = std::move(renderGraph_p);
	}

	IRenderGraph& IGraphics::GetRenderGraph() const
	{
		return *pRenderGraph;
	}

	void IGraphics::ExecuteFrame()
	{
		trynass_msg(pRenderGraph != nullptr, L"Tried to execute frame with no render graph set!");
		pRenderGraph->ExecuteFrame(*this);
	}

	const glm::mat4& IGraphics::GetCameraMatrix() const
	{
		return camera;
	}

	glm::mat4& IGraphics::GetCameraMatrix()
	{
		return camera;
	}

	void IGraphics::SetCamera(glm::mat4 camera)
	{
		this->camera = std::move(camera);
	}

	const glm::mat4& IGraphics::GetProjectionMatrix() const
	{
		return projection;
	}

	glm::mat4& IGraphics::GetProjectionMatrix()
	{
		return projection;
	}

	void IGraphics::SetProjection(glm::mat4 projection)
	{
		this->projection = std::move(projection);
	}

	bool IGraphics::GetVsyncFlag() const
	{
		return vsync;
	}

	void IGraphics::SetVsyncFlag(const bool vsync_)
	{
		vsync = vsync_;
	}

	const spa::DimensionsI& IGraphics::GetDimensions() const
	{
		return dimensions;
	}

	const std::vector<std::string>& IGraphics::GetApiArray()
	{
		static std::vector<std::string> graphicApiString = {
#define X(el) GENERATE_STRING(el)
			GRAPHIC_APIS
#undef X
		};
		return graphicApiString;
	}

	IContext& IGraphics::GetContextInterface() const
	{
		return *pContext;
	}

	const char* IGraphics::GetShaderRootPath()
	{
		static bool isInitialized = false;
		static std::string shaderRootPath;
		if (!isInitialized)
		{
			shaderRootPath += __FILE__;
			size_t trynPos = shaderRootPath.rfind("Tryn");
			if (trynPos != std::string::npos)
			{
				shaderRootPath.erase(trynPos + 4);
			}

			shaderRootPath += "\\bin\\Shaders\\";
			isInitialized = true;
		}

		return shaderRootPath.data();
	}

	const float* IGraphics::GetBackgroundColor() const
	{
		return bgcolor;
	}

	void IGraphics::SetBackgroundColor(const float r, const float g, const float b, const float a)
	{
		bgcolor[0] = r;
		bgcolor[1] = g;
		bgcolor[2] = b;
		bgcolor[3] = a;
	}

	const IGraphics::BindableVTable& IGraphics::GetBindableVTable() const
	{
		return bindableVtable;
	}

	void IGraphics::InitThread()
	{
		kernelThread_ = std::jthread(&IGraphics::KernelLoop_, this);
	}

	void IGraphics::KernelLoop_()
	{
		startSignal_.acquire();
		std::unique_lock<std::mutex> lk(mtx);

		while (!closing_)
		{
			cv.wait(lk, [this] {
				return !tasks_.Empty() || closing_;
				});
			tasks_.PopExecute();
		}
	}
	void IGraphics::InitDefaults()
	{
		// Init Projection Matrix
		SetProjection(glm::perspectiveFovLH(glm::radians(90.0f), static_cast<float>(dimensions.width), static_cast<float>(dimensions.height), 0.1f, 10000000000.0f));

		InitDefaultRenderGraph();
	}
	void IGraphics::InitDefaultRenderGraph()
	{
		// Default render graph creation
		pRenderGraph = std::make_unique<DefaultRenderGraph>(*this);
	}
}
