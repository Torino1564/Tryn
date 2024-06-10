#include <TrynCppUnitTest.h>
#include <memory>
#include <Core/src/app/Initialization.h>
#include <Core/src/win/Window.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/ecs/Entity.h>
#include <Core/src/gfx/Render/RenderGraph.h>
#include <Core/src/ecs/cmp/ComponentManager.h>
#include <Core/src/ecs/sys/SystemManager.h>
#include <Core/src/utl/Timer.h>
#include <Core/src/ecs/cmp/Components.h>

namespace tryn::ecs
{
	ZT_DEFINE_COMPONENT(TestComponent1)
	{
		ZT_COMPONENT_FIELDS(int testField = 0;);
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
		TEST_METHOD(StatefullBS)
		{
			using ComponentList_t = typename ComponentManager::ComponentList<>;
			ComponentList_t componentTuple = {};

			static constexpr auto velocityID = cmp::VelocityComponent::ctcID;
			using VelType = ComponentManager::ComponentByIndex<velocityID>;

			VelType velCmp = {};
		}
	private:
		std::unique_ptr<gfx::dx11::Graphics> pGfx;
		std::unique_ptr<win::Window> pWnd;
		std::vector<std::unique_ptr<Entity>> entityPtrs;
	};
}