#include <TrynCppUnitTest.h>
#include <memory>
#include <Core/src/app/Initialization.h>
#include <Core/src/win/Window.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/ent/Entity.h>
#include <Core/src/gfx/RenderGraph.h>

class ThunkRenderGraph : public tryn::gfx::IRenderGraph
{
public:
	ThunkRenderGraph()
	{
		AddRenderQueue("Lambertian");
	}
};

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
			pGfx->SetRenderGraph(std::move(std::make_unique<ThunkRenderGraph>()));
		}
		TEST_METHOD(ECSTest)
		{
			auto& manager = tryn::ent::ComponentManager::Get();
			entityPtrs.push_back(std::make_unique<BasicEntity>(*reinterpret_cast<gfx::IGraphics*>(pGfx.get()), "testEnt2", "D:/dev/Tryn/UnitTest/resources/models/gobber/GoblinX.obj", glm::vec3{ 0.1f,0.1f,0.1f }));
			entityPtrs.push_back(std::make_unique<BasicEntity>(*reinterpret_cast<gfx::IGraphics*>(pGfx.get()), "testEnt3", "D:/dev/Tryn/UnitTest/resources/models/gobber/GoblinX.obj", glm::vec3{ 0.1f,0.1f,0.1f }));
			entityPtrs.push_back(std::make_unique<BasicEntity>(*reinterpret_cast<gfx::IGraphics*>(pGfx.get()), "testEnt4", "D:/dev/Tryn/UnitTest/resources/models/gobber/GoblinX.obj", glm::vec3{ 0.1f,0.1f,0.1f }));
			entityPtrs.push_back(std::make_unique<BasicEntity>(*reinterpret_cast<gfx::IGraphics*>(pGfx.get()), "testEnt1", "D:/dev/Tryn/UnitTest/resources/models/gobber/GoblinX.obj", glm::vec3{ 0.1f,0.1f,0.1f }));

			while (true)
			{
				for (auto& pEntity : entityPtrs)
				{

				}
			}
		}
	private:
		std::unique_ptr<gfx::dx11::Graphics> pGfx;
		std::unique_ptr<win::Window> pWnd;
		std::vector<std::unique_ptr<IEntity>> entityPtrs;
	};
}