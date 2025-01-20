#include "TrynPCH.h"
#include "Component.h"

#include "EntityID.h"

namespace tryn::ecs
{
	std::string_view ComponentWrapper::Name() const
	{
		return name;
	}

	utl::UUID_t ComponentWrapper::Uuid() const
	{
		return uuid;
	}


	uint16_t ComponentWrapper::ByteSize() const
	{
		return componentSize;
	}

	ComponentArray ComponentWrapper::MakeArray(const uint16_t newSize) const
	{
		return ComponentArray(*this);
	}

	void ComponentWrapper::Delete(void* pData) const
	{
		delete_(pData);
	}

	void ComponentWrapper::New(void* pData) const
	{
		new_(pData);
	}

	uint32_t ComponentArray::ByteSize() const
	{
		return buffer.size();
	}

	uint32_t ComponentArray::ElementCount() const
	{
		return ByteSize() / parentWrapper.ByteSize();
	}

	void ComponentArray::Resize(const uint32_t elementCount)
	{
		buffer.resize(elementCount * parentWrapper.ByteSize());
	}

	std::byte* ComponentArray::data()
	{
		return buffer.data();
	}

	void ComponentArray::Delete(const EntityID id)
	{
		trynass(id.ID < ElementCount());
		const auto pData = data() + id.ID * parentWrapper.ByteSize();
		parentWrapper.Delete(static_cast<void*>(pData));
	}

	std::byte* ComponentArray::operator[](const std::uint16_t elNumber)
	{
		trynass(elNumber < ElementCount());
		return data() + elNumber * parentWrapper.ByteSize();
	}

	ComponentArray::ComponentArray(const ComponentWrapper& parent):
		parentWrapper(parent)
	{}
}
