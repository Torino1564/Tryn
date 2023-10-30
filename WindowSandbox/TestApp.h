#pragma once
#include <Core/src/app/App.h>
#include <Core/src/ent/Entity.h>
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
	std::vector<std::unique_ptr<ent::IEntity>> entities;
	std::vector<std::future<void>> futures;
	std::unique_ptr<ent::IEntity> suzanne1;
	std::unique_ptr<ent::IEntity> suzanne2;
	std::unique_ptr<ent::IEntity> suzanne3;
	std::unique_ptr<ent::IEntity> suzanne4;
	std::unique_ptr<ent::IEntity> sponza;
	std::unique_ptr<ent::IEntity> wall;
	std::unique_ptr<ent::IEntity> desk;
	std::unique_ptr<ent::IEntity> gobber;
	gfx::Camera camera;

	static constexpr const int entityCount1D = 3;

	int tallyX = 0, tallyY = 0;
};