#pragma once
#include <Core/src/app/App.h>
#include <Core/src/ecs/Entity.h>
#include <Core/src/gfx/PointLight.h>
#include <Core/src/gfx/Camera.h>
#include <vector>

using namespace tryn;

class TestApp final : public app::App
{
public:
	TestApp(std::shared_ptr<win::IWindow> wnd, std::shared_ptr<gfx::IGraphics> gfx);

	void DoFrame() override;
private:
	std::unique_ptr<gfx::PointLight> pPointLight;
	std::vector<ecs::Entity> entities;
	gfx::Camera camera;

	static constexpr const int entityCount1D = 10;
};