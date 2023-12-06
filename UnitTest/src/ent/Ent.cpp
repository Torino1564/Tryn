#include <TrynCppUnitTest.h>
#include <memory>
#include <Core/src/app/Initialization.h>
#include <Core/src/win/Window.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/ent/Entity.h>


namespace tryn::ent
{
	TEST_CLASS(EntitySystem)
	{
	public:
		TEST_METHOD_INITIALIZE(Init)
		{
			app::BootCore();
			pWnd = std::make_unique<win::Window>(std::make_shared<win::WindowClass>(), L"Test window", spa::DimensionsI(300, 300));
			pGfx = std::make_unique<gfx::dx11::Graphics>(pWnd->GetHandle(), pWnd->GetClientDimensions().width, pWnd->GetClientDimensions().height);
		}
		TEST_METHOD(ECSTest)
		{
			auto& manager = tryn::ent::ComponentManager::Get();
			BasicEntity testEnt1(*reinterpret_cast<gfx::IGraphics*>(pGfx.get()), "testEnt1", "resources/models/gobber/GoblinX.obj", glm::vec3{ 0.1f,0.1f,0.1f });
		}
	private:
		std::unique_ptr<gfx::dx11::Graphics> pGfx;
		std::unique_ptr<win::Window> pWnd;
	};
}