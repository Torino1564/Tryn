#include <TrynCppUnitTest.h>
#include <memory>
#include <Core/src/app/Initialization.h>
#include <Core/src/win/Window.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/ent/Entity.h>
#include <Core/src/gfx/RenderGraph.h>
#include <Core/src/ent/Component/ComponentManager.h>
#include <Core/src/ent/sys/SystemManager.h>
#include <Core/src/ent/Component/ComponentPack.h>
#include <Core/src/utl/Timer.h>

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
			std::string_view text;
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

	ZT_DEFINE_SYSTEM(System1)
	{
	public:
		System1()
		{
			
		}
		static void Execute()
		{
			trylog.info(L"System1 says Execute!");
		}
		ZT_SYSTEM_UUID;
	};

	ZT_DEFINE_SYSTEM(System2)
	{
	public:
		System2()
		{
		}
		static void Execute()
		{
			trylog.info(L"System2 says Execute!");
		}
		ZT_SYSTEM_UUID;
	};

	ZT_DEFINE_SYSTEM(System3)
	{
	public:
		System3()
		{
		}
		static void Execute()
		{
			trylog.info(L"System3 says Execute!");
		}
		ZT_SYSTEM_UUID;
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
			Entity ent1("Test entity 1");
			Entity ent2("Test entity 2");
			Entity ent3("Test entity 3");

		}
		TEST_METHOD(ArchetypeTests)
		{
			auto ent1 = Entity::CreateNew<TestComponent1, TestComponent2, TestComponent3>("ent1");
			std::vector<Entity> entityVec1;
			entityVec1.resize(100);
			ent1.Instanciate({ entityVec1.begin(), entityVec1.size() });
			auto ent2 = Entity::CreateNew<TestComponent1, TestComponent2, TestComponent3>();
			{
				auto ent5 = Entity::CreateNew<TestComponent1, TestComponent2, TestComponent3>();
				auto ent6 = Entity::CreateNew<TestComponent1, TestComponent2, TestComponent3>();
			}
			auto ent7 = Entity::CreateNew<TestComponent1, TestComponent2, TestComponent3>();
			auto ent8 = Entity::CreateNew<TestComponent1, TestComponent2, TestComponent3>();

			auto ent3 = Entity::CreateNew<TestComponent2, TestComponent3>();
			auto ent4 = Entity::CreateNew<TestComponent1, TestComponent2>();

			ECS::Get().allocator.Wipe();

			std::vector<Entity> entityVec2;
			entityVec2.resize(100000000);
			ent3.Instanciate({ entityVec2.begin(), entityVec2.size() });
			

		}
		TEST_METHOD(SystemTests)
		{
			ent::sys::SystemGraph sysGraph;

			System1 sys1;

			System2 sys2;
			
			System3 sys3;
			sys3.AddDependency<System1>();
			sys3.AddDependency<System2>();

			sysGraph.RegisterSystem(sys1);
			sysGraph.RegisterSystem(sys2);
			sysGraph.RegisterSystem(sys3);

			sysGraph.Finalize();
			sysGraph.Execute();
		}
	private:
		std::unique_ptr<gfx::dx11::Graphics> pGfx;
		std::unique_ptr<win::Window> pWnd;
		std::vector<std::unique_ptr<Entity>> entityPtrs;
	};
}