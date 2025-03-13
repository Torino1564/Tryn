#include <TrynCppUnitTest.h>
#include <Core/src/win/WindowClass.h>

namespace tryn::ecs
{
	ZT_DEFINE_SYSTEM(System1)
	{
	public:
		System1(const SystemGraph & pGraph) : SystemImpl(pGraph) {}

		void Execute() override
		{
			trylog.info(L"System1 says Execute!");
		}
	};

	ZT_DEFINE_SYSTEM(System2)
	{
	public:
		System2(const SystemGraph & pGraph) : SystemImpl(pGraph) {}

		void Execute() override
		{
			trylog.info(L"System2 says Execute!");
		}
	};

	ZT_DEFINE_SYSTEM(System3)
	{
	public:
		System3(const SystemGraph & pGraph) : SystemImpl(pGraph) {}

		static void InitDependencies(System* self)
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

		}
		TEST_METHOD(SystemTests)
		{
			ecs::SystemGraph sysGraph;

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