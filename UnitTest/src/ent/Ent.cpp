#include <TrynCppUnitTest.h>
#include <memory>
#include <Core/src/app/Initialization.h>
#include <Core/src/win/Window.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/ent/Entity.h>
#include <Core/src/gfx/RenderGraph.h>
#include <Core/src/ent/Component/Component.h>
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
	class TestComponent1
	{
		DECLARE_UUID;
		DECLARE_SRD()
	};
	class TestComponent2
	{
		DECLARE_UUID;
		DECLARE_SRD(
			std::string text;
			bool onFire;
			float fireIntensity;
		)
	};
	class TestComponent3
	{
		DECLARE_UUID;
		DECLARE_SRD(
			std::string text;
			bool onFire;
			float fireIntensity;
			long double test1;
			long double test2;
			long long int test3;
		)
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
			TestComponent3 tc3;
			const auto& uuid3 = tc3.UUID;

			auto& cm = ent::ComponentManager::Get();

			cm.RegisterComponent<TestComponent1>();
			cm.RegisterComponent<TestComponent2>();
			cm.RegisterComponent<TestComponent3>();

			auto& srd1 = cm.AddComponent<TestComponent1>(50);
			auto& srd2 = cm.AddComponent<TestComponent1>(2);
			auto& srd3 = cm.AddComponent<TestComponent2>(1);
			auto& srd4 = cm.AddComponent<TestComponent3>(4);
		}
	private:
		std::unique_ptr<gfx::dx11::Graphics> pGfx;
		std::unique_ptr<win::Window> pWnd;
		std::vector<std::unique_ptr<IEntity>> entityPtrs;
	};
}