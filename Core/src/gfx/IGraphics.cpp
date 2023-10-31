#include "IGraphics.h"
#include <Core/src/log/Log.h>

glm::mat4& tryn::gfx::IGraphics::GetCameraMatrix()
{
	return camera;
}

void tryn::gfx::IGraphics::SetCamera(glm::mat4 camera)
{
	this->camera = std::move(camera);
}

glm::mat4& tryn::gfx::IGraphics::GetProjectionMatrix()
{
	return projection;
}

void tryn::gfx::IGraphics::Wait() const
{
	while (!frameReady_);
}

void tryn::gfx::IGraphics::SetProjection(glm::mat4 projection)
{
	this->projection = std::move(projection);
}

void tryn::gfx::IGraphics::InitThread()
{
	kernelThread_ = std::jthread(&IGraphics::KernelLoop_, this);
}

void tryn::gfx::IGraphics::KernelLoop_()
{
	startSignal_.acquire();
	std::unique_lock<std::mutex> lock(mtx);

	while (!closing_)
	{
		cv.wait(lock, [&] {return !tasks_.Empty(); });
		while (!tasks_.Empty())
		{
			tasks_.PopExecute();
		}
	}
}
