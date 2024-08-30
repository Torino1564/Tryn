#include <TrynCppUnitTest.h>

namespace tryn::ecs
{
	ZT_DEFINE_COMPONENT(TestComponent1)
	{
		ZT_COMPONENT_FIELDS(
			bool onFire = false;
		);
	};

	const unsigned int TestComponent1::index = ComponentManager::RegisterComponent<TestComponent1>();

	const std::vector<utl::CTM::ElementData>& TestComponent1::GetReflectData_()
	{
		static const std::vector<tryn::utl::CTM::ElementData> singleton = tryn::ecs::CreateElementDataVector<ComponentType>();
		return singleton;
	}

	ZT_DEFINE_COMPONENT(TestComponent2)
	{
		ZT_COMPONENT_FIELDS(
			std::string_view text;
			bool onFire;
			float fireIntensity;
		);
	};

	const unsigned int TestComponent2::index = ComponentManager::RegisterComponent<TestComponent2>();

	const std::vector<utl::CTM::ElementData>& TestComponent2::GetReflectData_()
	{
		static const std::vector<tryn::utl::CTM::ElementData> singleton = tryn::ecs::CreateElementDataVector<ComponentType>();
		return singleton;
	}

	ZT_DEFINE_COMPONENT(TestComponent3)
	{
		ZT_COMPONENT_FIELDS(
			float velocity_x = .0f;
			float velocity_y = .0f;
			float velocity_z = .0f;
			float whatever = .0f;
		);
	};

	const unsigned int TestComponent3::index = ComponentManager::RegisterComponent<TestComponent3>();

	const std::vector<utl::CTM::ElementData>& TestComponent3::GetReflectData_()
	{
		static const std::vector<tryn::utl::CTM::ElementData> singleton = tryn::ecs::CreateElementDataVector<ComponentType>();
		return singleton;
	}

	ZT_DEFINE_SYSTEM(System1)
	{
	public:
		System1(const sys::SystemGraph & pGraph) : SystemImpl(pGraph) {}

		void Execute() override
		{
			trylog.info(L"System1 says Execute!");
		}
	};

	ZT_DEFINE_SYSTEM(System2)
	{
	public:
		System2(const sys::SystemGraph & pGraph) : SystemImpl(pGraph) {}

		void Execute() override
		{
			trylog.info(L"System2 says Execute!");
		}
	};

	ZT_DEFINE_SYSTEM(System3)
	{
	public:
		System3(const sys::SystemGraph & pGraph) : SystemImpl(pGraph) {}

		static void InitDependencies(sys::System* self)
		{
			self->AddDependency<System1>();
			self->AddDependency<System2>();
		}
		void Execute() override
		{
			trylog.info(L"System3 says Execute!");
		}
	};

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
			auto ent9 = Entity::CreateNew<TestComponent1, TestComponent2>();
			auto ent10 = Entity::CreateNew<TestComponent1, TestComponent2>();
			auto ent11 = Entity::CreateNew<TestComponent1, TestComponent2>();

			ECS::Get().allocator.Wipe();

			std::vector<Entity> entityVec2;
			entityVec2.resize(100000);
			ent3.Instanciate({ entityVec2.begin(), entityVec2.size() });

			auto data = ArchetypeManager::Get().GetComponentGroups<ReadOnly<TestComponent2>, WriteOnly<TestComponent1>, ReadOnly<TestComponent3>>();
		}
		TEST_METHOD(SystemTests)
		{
			ecs::sys::SystemGraph sysGraph;

			sysGraph.RegisterSystem<System1>();
			sysGraph.RegisterSystem<System2>();
			sysGraph.RegisterSystem<System3>();

			sysGraph.Finalize();
			sysGraph.Execute();
		}
	private:
		std::unique_ptr<gfx::dx11::Graphics> pGfx;
		std::unique_ptr<win::Window> pWnd;
		std::vector<std::unique_ptr<Entity>> entityPtrs;
	};
}