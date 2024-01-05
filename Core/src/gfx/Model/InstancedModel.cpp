#include "InstancedModel.h"
#include <Core/src/mem/ArenaAllocator.h>

namespace tryn::gfx
{
	InstancedModelParent::InstancedModelParent(gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale, Techniques defaultTechnique, std::optional<std::uint32_t> numInstances)
		:
		pBase(std::make_unique<Model>(gfx, path, scale, defaultTechnique, true))
	{
		instancedGroup = "InstaceGroup";
		instancedGroup += path.data();

		numInstanced = numInstances.value_or(0);
		transforms.resize(numInstances.value_or(100));
		ConstantBufferLayout::Node arrayElement(ConstantBufferLayout::Type::Struct, "arrayStruct");
		arrayElement["arrayStruct"].Append(ConstantBufferLayout::Type::Matrix4, "transform");

		for (auto i = 0; i < pBase->GetMeshAmount(); i++)
		{
			pTransformationBuffers[i] = gfx.CreateInstanceBuffer(arrayElement, numInstanced);
		}
	}
	void InstancedModelParent::Submit(const glm::mat4& transformation)
	{
		auto& settings = pBase->settings;
		const auto rotation = glm::yawPitchRoll(settings.angles.x, settings.angles.y, settings.angles.z);
		const auto translation = glm::translate(glm::mat4(1.0f), settings.position);
		const auto transform = translation * rotation;
		auto modifiedTransforms = mem::ArenaAllocator<>::GP().MakeNewArray<glm::mat4>(numInstanced);
		for (auto [index, modifiedTransform] : std::ranges::views::enumerate(modifiedTransforms))
		{
			modifiedTransform = transforms[index] * transform;
		}
		pBase->root->Submit(pBase->gfx, { modifiedTransforms }, *this);
	}
	IInstanceBuffer& InstancedModelParent::RequestInstanceBuffer(std::uint16_t key)
	{
		trynass_msg(key < pTransformationBuffers.size(), L"Out of bounds access!");
		return *pTransformationBuffers[key].get();
	}
	void InstancedModelChild::Submit(const glm::mat4& transformation)
	{
		parentModel.transforms[instanceID] = transformation;
	}
}
