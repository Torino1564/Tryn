#include "TrynPCH.h"
#include "IGraphics.h"
#include <Core/src/log/Log.h>
#include <Core/src/gfx/Render/DefaultRenderGraphs/DefaultRenderGraph.h>
#include <Core/third/glm/ext/matrix_clip_space.hpp>
#include <Core/src/win/TrynWin.h>

namespace tryn::gfx
{
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

	const spa::DimensionsI& IGraphics::GetDimensions() const
	{
		return dimensions;
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
