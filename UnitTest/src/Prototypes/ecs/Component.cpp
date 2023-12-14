#include <TrynCppUnitTest.h>
#include "Component.h"
#include <memory>
#include <vector>

using namespace tryn::ent;

namespace tryn::ent::prot
{
	TEST_CLASS(ECSProt)
	{
	public:
		TEST_METHOD_INITIALIZE(Init)
		{
			pComponents.push_back(std::make_unique<TestComponent1>());
			pComponents.push_back(std::make_unique<TestComponent2>());
		}
		TEST_METHOD(CRTP)
		{
			for (auto& pComponent : pComponents)
			{
				pComponent->OnUpdate();
			}
		}
	private:
		std::vector<std::unique_ptr<Component>> pComponents;
	};
}