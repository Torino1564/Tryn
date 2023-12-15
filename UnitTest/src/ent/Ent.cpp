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

	ZT_DEFINE_COMPONENT(TestComponent3)
	{
		ZT_COMPONENT_FIELDS(
			float velocity_x = .0f;
			float velocity_y = .0f;
			float velocity_z = .0f;
			float whatever = .0f;
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
			BasicEntity ent1("Test entity 1");
			BasicEntity ent2("Test entity 2");
			BasicEntity ent3("Test entity 3");

			auto& tc1 = ent1.AddComponent<TestComponent1>();
			tc1.onFire = true;
			tc1.active = true;
			auto& tc2 = ent1.AddComponent<TestComponent2>();
			tc2.fireIntensity = 100.0f;
			tc2.text = "Hello there";
			auto& tc3 = ent1.AddComponent<TestComponent3>();
			tc3.velocity_x = 10.0f;
			tc3.velocity_y = -69.0f;
			tc3.velocity_z = 420.0f;
			auto& tc4 = ent2.AddComponent<TestComponent1>();
			tc4.onFire = true;
			auto& tc5 = ent3.AddComponent<TestComponent1>();
			tc5.active = true;

			auto& tc1a = ent1.GetComponent<TestComponent1>();
			auto& tc2a = ent1.GetComponent<TestComponent2>();
			auto& tc3a = ent1.GetComponent<TestComponent3>();
		}
	private:
		std::unique_ptr<gfx::dx11::Graphics> pGfx;
		std::unique_ptr<win::Window> pWnd;
		std::vector<std::unique_ptr<IEntity>> entityPtrs;
	};
}