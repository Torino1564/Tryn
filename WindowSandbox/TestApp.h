#pragma once
#include <Core/src/app/App.h>
#include <Core/src/ent/Entity.h>
#include <vector>
#include <core/src/gfx/ImguiManager.h>

using namespace tryn;

class TestApp : public app::App
{
public:
	TestApp(std::shared_ptr<win::IWindow> wnd, std::shared_ptr<gfx::IGraphics> gfx);

	void DoFrame() override;
private:
	std::vector<ent::Entity> entities;
	gfx::ImguiManager imgui;
};