#include "IGraphics.h"
#include <Core/src/log/Log.h>
#include <Core/src/gfx/Render/DefaultRenderGraphs/DefaultRenderGraph.h>

namespace tryn::gfx
{
	void IGraphics::SetRenderGraph(std::unique_ptr<IRenderGraph>&& renderGraph_p)
	{
		pRenderGraph = std::move(renderGraph_p);
	}

	IRenderGraph& IGraphics::GetRenderGraph()
	{
		return *pRenderGraph;
	}

	void IGraphics::ExecuteFrame()
	{
		trynass_msg(pRenderGraph != nullptr, L"Tried to execute frame with no render graph set!");
		pRenderGraph->ExecuteFrame(*this);
	}

	glm::mat4& IGraphics::GetCameraMatrix()
	{
		return camera;
	}

	void IGraphics::SetCamera(glm::mat4 camera)
	{
		this->camera = std::move(camera);
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
	void IGraphics::InitDefaultRenderGraph()
	{
		// Default render graph creation
		pRenderGraph = std::make_unique<DefaultRenderGraph>(*this);
	}
}
