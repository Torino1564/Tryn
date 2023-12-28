#pragma once
#include "Model.h"
#include <Core/src/gfx/Bindables/IBuffer.h>
#include <optional>
#include <span>

namespace tryn::gfx
{
	class InstancedModelChild;

	class InstancedModelParent : public Model
	{
	public:
		InstancedModelParent(gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f }, Techniques defaultTechnique = Techniques::Phong, std::optional<std::uint32_t> numInstances = std::nullopt);
		void Submit(const glm::mat4& entityTransform) override;
		InstancedModelChild Instanciate();
		void Instanciate(std::span<InstancedModelChild> childSpan);
	private:
		std::string instancedGroup;
		std::uint32_t numInstanced;
		std::unique_ptr<IInstanceBuffer> pTransformationBuffer;
	};

	class InstancedModelChild
	{
		void Submit(const glm::mat4& transformation);
	public:
		int instanceID;
		InstancedModelParent& pParentModel;
	};
}