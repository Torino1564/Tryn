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
	void StateMachine();
	void ShowStateInfo();

private:
	std::unique_ptr<gfx::PointLight> pPointLight;
	std::unique_ptr<class Player> pPlayer;
	gfx::Camera camera;

	gfx::Camera player;

	gfx::Camera* pActiveCamera = &camera;

	enum class Mode
	{
		Ghost,
		Player,
		Mouse
	};

	Mode state = Mode::Mouse;
};