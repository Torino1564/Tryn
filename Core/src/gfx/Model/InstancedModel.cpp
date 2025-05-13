#include "TrynPCH.h"
#include "InstancedModel.h"
#include <Core/src/mem/ArenaAllocator.h>
#include "Core/third/glm/gtx/euler_angles.hpp"
#include <Core/src/gfx/IGraphics.h>
#include <Core/src/gfx/Bindables/IBufferBase.h>
#include <Core/src/gfx/Model/Model.h>

namespace tryn::gfx
{
	InstancedModelParent::InstancedModelParent(const gfx::IGraphics& gfx, std::string_view path, std::span<utl::UUID_t> techniqueUUIDs, glm::vec3 scale, std::optional<std::uint32_t> numInstances)
		:
		pBase(std::make_unique<Model>(gfx, path, techniqueUUIDs, scale, true))
	{
		instancedGroup = "InstanceGroup";
		instancedGroup += path.data();

		trynass_msg(numInstances.value_or(10) != 0, L"numInstances cannot be 0!");

		numInstanced = 0;
		upperLimit = numInstances.value_or(10);
		ConstantBufferLayout::Node arrayElement_(ConstantBufferLayout::Type::Struct, "arrayStruct");
		arrayElement_.Append(ConstantBufferLayout::Type::Matrix4, "transform");
		this->arrayElement = std::move(arrayElement_);
		pTransformationBuffers.reserve(pBase->GetMeshAmount());

		Resize(upperLimit);
	}

	InstancedModelParent::~InstancedModelParent() = default;

	void InstancedModelParent::Submit(const glm::mat4& transformation)
	{
		auto& settings = pBase->settings;
		const auto rotation = glm::yawPitchRoll(settings.angles.x, settings.angles.y, settings.angles.z);
		const auto translation = glm::translate(glm::mat4(1.0f), settings.position);
		const auto transform = translation * rotation;
		auto modifiedTransforms = mem::ArenaAllocator<>::GP().MakeNewArray<glm::mat4>(upperLimit);
		for (auto [index, modifiedTransform] : std::ranges::views::enumerate(modifiedTransforms))
		{
			modifiedTransform = transforms[index] * transform;
		}
		pBase->nodes[pBase->rootId].Submit(*pBase->GetGfx(), { modifiedTransforms }, *this);
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
	IInstanceBuffer& InstancedModelParent::RequestInstanceBuffer(const std::uint16_t key) const
	{
		trynass_msg(key < pTransformationBuffers.size(), L"Out of bounds access!");
		return *pTransformationBuffers[key].get();
	}
	std::uint32_t InstancedModelParent::ResolveID()
	{
		size_t slot = 0;
		if (numInstanced == 0)
		{
			slot = 0;
			numInstanced++;
		}
		else
		{
			slot = booker.find_next(numInstanced - 1);
			if (slot == booker.npos)
			{
				Resize((booker.size() + 10) * 2.0f);
				slot = booker.find_next(numInstanced - 1);
			}
			numInstanced = slot;
			numInstanced++;
			booker[slot].flip();
			if (numInstanced + 1 > upperLimit)
			{
				upperLimit = numInstanced + 1;
			}
		}

		return slot;
	}
	void InstancedModelParent::Resize(const std::size_t newSize)
	{
		transforms.resize(newSize, glm::mat4{ 0.0f });
		booker.resize(newSize, true);
		pTransformationBuffers.clear();
		for (auto i = 0; i < pBase->GetMeshAmount(); i++)
		{
			pTransformationBuffers.emplace_back(pBase->GetGfx()->CreateInstanceBuffer(arrayElement, upperLimit));
		}
	}
	InstancedModelChild::~InstancedModelChild()
	{
		if (pParentModel != nullptr)
			pParentModel->booker[instanceID].flip();
	}
	void InstancedModelChild::Submit(const glm::mat4& transformation) const
	{
		pParentModel->transforms[instanceID] = transformation;
	}
}

void tryn::ser::Serialize(StreamIO& io, gfx::InstancedModelParent* pData, const bool binary, const std::string& name)
{
	io.Field(&pData->pBase, binary);
}

void tryn::ser::Serialize(StreamWriter& sw, gfx::InstancedModelChild* pData, bool binary, const std::string& name)
{
	
}

void tryn::ser::Serialize(StreamReader& io, gfx::InstancedModelChild* pData, bool binary, const std::string& name)
{

}
