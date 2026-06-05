#pragma once
#include <optional>
#include <span>
#include "Core/src/gfx/Bindables/InstanceBuffer.h"
#include <Core/third/dynamic_bitset.hpp>
#include "glm/fwd.hpp"
#include <Core/src/ser/StreamIO.h>

#include "Core/src/utl/StringHasher.h"

namespace tryn::gfx
{
	class IGraphics;
	class InstancedModelChild;

	class InstancedModelParent
	{
		//friend void ser::Serialize(ser::StreamIO& io, gfx::InstancedModelParent* pData, bool binary, const std::string& name);
		friend class InstancedModelChild;

	public:
		InstancedModelParent(const gfx::IGraphics& gfx, std::string_view path, std::span<utl::UUID_t> techniqueUUIDs = {} ,glm::vec3 scale = { 1.0f,1.0f,1.0f }, std::optional<std::uint32_t> numInstances = std::nullopt);
		~InstancedModelParent();
		void Submit(const glm::mat4& entityTransform);
		InstancedModelChild Instanciate();
		void Instanciate(std::span<InstancedModelChild> childSpan);
		IInstanceBuffer& RequestInstanceBuffer(std::uint16_t key) const;
	private:
		std::uint32_t ResolveID();
		void Resize(std::size_t newSize);
		std::string instancedGroup;
		std::uint32_t numInstanced = 0;
		std::uint32_t upperLimit = 0;
		std::uint32_t bookerPointer;
		std::unique_ptr<class Model> pBase;
		std::vector<glm::mat4> transforms;
		sul::dynamic_bitset<> booker;
		gfx::ConstantBufferLayout::Node arrayElement;
		std::vector<std::shared_ptr<IInstanceBuffer>> pTransformationBuffers;
	};


	class InstancedModelChild
	{
		friend class InstancedModelParent;
	public:	
		~InstancedModelChild();
		void Submit(const glm::mat4& transformation) const;

		std::uint16_t instanceID = {};
		InstancedModelParent* pParentModel = nullptr;
	};
}
