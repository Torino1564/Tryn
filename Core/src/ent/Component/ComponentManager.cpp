#include "ComponentManager.h"

void tryn::ent::ComponentManager::ActivateComponent(std::uint16_t componentUUID, std::uint16_t componentIndex)
{
	auto activationProxy = reinterpret_cast<ActivationComponent::SubresourceData&>((*(bufferPtrs[componentUUID]))[componentIndex]);
	activationProxy.active = true;
}
