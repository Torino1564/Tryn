#pragma once
#include <Core/src/app/App.h>
#include <Core/src/gfx/Camera.h>

using namespace tryn;

class TrynGameApp : public tryn::app::App
{
public:
	TrynGameApp(std::shared_ptr<win::IWindow>, std::shared_ptr<gfx::IGraphics>);
	void DoFrame() override;

private:
	std::unique_ptr<class Player> pPlayer;
	gfx::Camera camera;
};