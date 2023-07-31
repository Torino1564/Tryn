#pragma once
#include <Core/src/app/App.h>
#include <Core/src/ent/Entity.h>
#include <vector>

using namespace tryn;

class TestApp final : public app::App
{
public:
	TestApp(std::shared_ptr<win::IWindow> wnd, std::shared_ptr<gfx::IGraphics> gfx);

	void DoFrame() override;
private:
	std::vector<ent::IEntity> entities;
};