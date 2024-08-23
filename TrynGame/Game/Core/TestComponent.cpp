#include "TrynGameEngine.h"
#include "TestComponent.h"

namespace tgame::cmp
{
	const unsigned int TestComponent::index = ecs::ComponentManager::RegisterComponent<TestComponent>();

	const std::vector<utl::CTM::ElementData>& TestComponent::GetReflectData_()
	{
		static const std::vector<tryn::utl::CTM::ElementData> singleton = tryn::ecs::CreateElementDataVector<
			ComponentType>();
		return singleton;
	}
}
