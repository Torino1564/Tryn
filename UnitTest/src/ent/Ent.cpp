#include <TrynCppUnitTest.h>
#include <memory>
#include <Core/src/app/Initialization.h>
#include <Core/src/win/Window.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/ent/Entity.h>
#include <Core/src/gfx/RenderGraph.h>
#include <Core/src/ent/Component/ComponentManager.h>

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
	ZT_DEFINE_COMPONENT(TestComponent1)
	{
		ZT_COMPONENT_FIELDS(
			bool onFire = false;
		);
	};

	ZT_DEFINE_COMPONENT(TestComponent2)
	{
		ZT_COMPONENT_FIELDS(
			std::string text;
			bool onFire;
			float fireIntensity;
		);
	};

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
			TestComponent1 tc1;
			const auto& uuid1 = tc1.UUID;
			TestComponent2 tc2;
			const auto& uuid2 = tc2.UUID;


			auto& cm = ent::ComponentManager::Get();

			const auto h1 = cm.AddComponent<TestComponent1>();
			const auto h2 = cm.AddComponent<TestComponent1>();
			const auto h3 = cm.AddComponent<TestComponent2>();

			const auto& srd1 = cm.GetComponent<TestComponent1>(h1);
			const auto& srd2 = cm.GetComponent<TestComponent1>(h2);
			const auto& srd3 = cm.GetComponent<TestComponent2>(h3);
		}
	private:
		std::unique_ptr<gfx::dx11::Graphics> pGfx;
		std::unique_ptr<win::Window> pWnd;
		std::vector<std::unique_ptr<IEntity>> entityPtrs;
	};
}