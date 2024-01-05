#pragma once
#include "Model.h"
#include <Core/src/gfx/Bindables/IBuffer.h>
#include <optional>
#include <span>

namespace tryn::gfx
{
	class InstancedModelChild;
	
	class InstancedModelParent
	{
		friend class InstancedModelChild;
	public:
		InstancedModelParent(gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f }, Techniques defaultTechnique = Techniques::Phong, std::optional<std::uint32_t> numInstances = std::nullopt);
		void Submit(const glm::mat4& entityTransform);
		InstancedModelChild Instanciate();
		void Instanciate(std::span<InstancedModelChild> childSpan);
		IInstanceBuffer& RequestInstanceBuffer(std::uint16_t key);
	private:
		std::string instancedGroup;
		std::uint32_t numInstanced;
		std::unique_ptr<Model> pBase;
		std::vector<glm::mat4> transforms;
		std::vector<std::unique_ptr<IInstanceBuffer>> pTransformationBuffers;
	};

	class InstancedModelChild
	{
	public:	
		void Submit(const glm::mat4& transformation);
	public:
		std::uint16_t instanceID;
		InstancedModelParent& parentModel;
	};
}