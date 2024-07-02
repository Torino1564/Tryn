#pragma once
#include <optional>
#include <span>
#include <Core/third/dynamic_bitset.hpp>
#include "Core/src/gfx/ConstantBuffer.h"
#include "Core/third/glm/fwd.hpp"

namespace tryn::gfx
{
	class InstancedModelChild;
	class Model;
	class IGraphics;

	class InstancedModelParent
	{
		friend class InstancedModelChild;
	public:
		InstancedModelParent(gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f }, std::optional<std::uint32_t> numInstances = std::nullopt);
		void Submit(const glm::mat4& entityTransform);
		InstancedModelChild Instanciate();
		void Instanciate(std::span<InstancedModelChild> childSpan);
		IInstanceBuffer& RequestInstanceBuffer(std::uint16_t key);
	private:
		std::uint32_t ResolveID();
		void Resize(std::size_t newSize);
		std::string instancedGroup;
		std::uint32_t numInstanced = 0;
		std::uint32_t upperLimit = 0;
		std::uint32_t bookerPointer;
		std::unique_ptr<Model> pBase;
		std::vector<glm::mat4> transforms;
		sul::dynamic_bitset<> booker;
		gfx::ConstantBufferLayout::Node arrayElement;
		std::vector<std::unique_ptr<IInstanceBuffer>> pTransformationBuffers;
	};

	class InstancedModelChild
	{
		friend class InstancedModelParent;
	public:	
		~InstancedModelChild();
		void Submit(const glm::mat4& transformation);
	public:

		std::uint16_t instanceID;
		InstancedModelParent* pParentModel;
	};
}
