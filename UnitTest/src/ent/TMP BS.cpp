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

	template <typename T>
	struct LogMemberVariable
	{
		void operator()()
		{
			typename T::TypeName_t typeNameFunc;
			typename T::VarName_t varNameFunc;
			typename T::VarSize_t varSizeFunc;
			typename T::ElementNumber_t elNumberFunc;
			typename T::ByteOffset_t byteOffsetFunc;
			trylog.info(utl::ToWide(std::format("Element Number: {}\n Type Name: {}\n Var Name: {}\n Var Size: {}\n Byte Offset: {}\n\n", elNumberFunc(), typeNameFunc(), varNameFunc(), varSizeFunc(), byteOffsetFunc())));
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
		TEST_METHOD(StatefullBS)
		{
			using ComponentList_t = typename ComponentManager::ComponentList<>;
			ComponentList_t componentTuple = {};

			static constexpr auto animatedID = cmp::AnimatedComponent::UUID;
			using AnimatedType = ComponentManager::ComponentByIndex<animatedID>;

			AnimatedType animatedCmp = {};

			ComponentManager::IterateComponentMembers<LogMemberVariable>(AnimatedType::UUID);

		}
	private:
		std::unique_ptr<gfx::dx11::Graphics> pGfx;
		std::unique_ptr<win::Window> pWnd;
		std::vector<std::unique_ptr<Entity>> entityPtrs;
	};
}