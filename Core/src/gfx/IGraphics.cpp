#include "IGraphics.h"
#include <Core/src/log/Log.h>

namespace tryn::gfx
{
	void IGraphics::SetRenderGraph(std::unique_ptr<IRenderGraph>&& renderGraph_p)
	{
		renderGraph = std::move(renderGraph_p);
	}

	IRenderGraph& IGraphics::GetRenderGraph()
	{
		return *renderGraph;
	}

	void IGraphics::ExecuteFrame()
	{
		trynass_msg(renderGraph != nullptr, L"Tried to execute frame with no render graph set!");
		renderGraph->ExecuteFrame(*this);
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
}
