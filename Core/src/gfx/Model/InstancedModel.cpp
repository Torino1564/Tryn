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

		trynass_msg(numInstances.value_or(10) != 0, L"numInstances cannot be 0!");

		numInstanced = 0;
		upperLimit = numInstances.value_or(10);
		transforms.resize(numInstances.value_or(10));
		booker.resize(numInstances.value_or(10), true);
		ConstantBufferLayout::Node arrayElement(ConstantBufferLayout::Type::Struct, "arrayStruct");
		arrayElement.Append(ConstantBufferLayout::Type::Matrix4, "transform");

		pTransformationBuffers.reserve(pBase->GetMeshAmount());

		for (auto i = 0; i < pBase->GetMeshAmount(); i++)
		{
			pTransformationBuffers.emplace_back(gfx.CreateInstanceBuffer(arrayElement, upperLimit));
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
	InstancedModelChild InstancedModelParent::Instanciate()
	{
		InstancedModelChild child;
		child.instanceID = ResolveID();
		child.pParentModel = this;
		return child;
	}
	void InstancedModelParent::Instanciate(std::span<InstancedModelChild> childSpan)
	{
		if (upperLimit + childSpan.size() < booker.size())
		{
			Resize((booker.size() + childSpan.size()) * 1.3f);
		}	

		for (auto& child : childSpan)
		{
			child.instanceID = ResolveID();
			child.pParentModel = this;
		}
	}
	IInstanceBuffer& InstancedModelParent::RequestInstanceBuffer(std::uint16_t key)
	{
		trynass_msg(key < pTransformationBuffers.size(), L"Out of bounds access!");
		return *pTransformationBuffers[key].get();
	}
	std::uint32_t InstancedModelParent::ResolveID()
	{
		auto slot = booker.find_next(numInstanced);
		if (slot == booker.npos)
		{
			Resize((booker.size() + 10) * 1.5f);
			slot = booker.find_next(numInstanced);
		}
		booker[slot].flip();
		numInstanced = slot;
		if (numInstanced > upperLimit)
		{
			upperLimit = numInstanced;
		}

		return slot;
	}
	void InstancedModelParent::Resize(std::size_t newSize)
	{
		transforms.resize(newSize, glm::mat4{ 0.0f });
		booker.resize(newSize, true);
	}
	InstancedModelChild::~InstancedModelChild()
	{
		pParentModel->booker[instanceID].flip();
	}
	void InstancedModelChild::Submit(const glm::mat4& transformation)
	{
		pParentModel->transforms[instanceID] = transformation;
	}
}
