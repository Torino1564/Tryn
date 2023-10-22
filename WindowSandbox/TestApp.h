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
	std::vector<ent::IEntity> entities;
	std::unique_ptr<ent::IEntity> suzanne1;
	std::unique_ptr<ent::IEntity> suzanne2;
	std::unique_ptr<ent::IEntity> suzanne3;
	std::unique_ptr<ent::IEntity> suzanne4;
	std::unique_ptr<ent::IEntity> sponza;
	std::unique_ptr<ent::IEntity> wall;
	std::unique_ptr<ent::IEntity> desk;
	gfx::Camera camera;

	int tallyX = 0, tallyY = 0;
};