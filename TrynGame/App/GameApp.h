#pragma once
#include <memory>
#include <Core/src/app/App.h>
#include <Core/src/gfx/CoreGraphics.h>
#include <Core/src/ecs/Entity.h>

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
	std::vector<std::unique_ptr<ecs::Entity>> entities;
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