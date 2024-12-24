#pragma once
#include <Core/src/ecs/EcsClass.h>
#include <Core/src/ser/Serializer.h>
#include <Core/third/dynamic_bitset.hpp>

namespace tryn::ecs
{
	class IComponentDescriptor
	{
	public:
		virtual ~IComponentDescriptor() = default;
		virtual const char* Name() const = 0;
		virtual size_t Size() const = 0;
		virtual void Construct(void* pData) const = 0;
		virtual void Destroy(void* pData) const = 0;
		virtual uint32_t Index() const = 0;
		virtual void Serialize(ser::StreamWriter& streamWriter, void* data) = 0;
		virtual void ImGuiPrint(void* data) = 0;
	};

	class ComponentArray
	{
	public:
		ComponentArray(IComponentDescriptor& descriptor, std::size_t initialSize);
		void Resize(std::size_t newSize);
		void* At(std::size_t index);
		void* operator[](std::size_t index);

	private:
		std::unique_ptr<IComponentDescriptor> pDescriptor;
		std::vector<std::byte> buffer;
	};
}
