#pragma once
#include "Model.h"
#include <Core/src/gfx/Bindables/IBuffer.h>
#include <optional>
#include <span>
#include <Core/third/dynamic_bitset.hpp>
#include <Core/src/gfx/Material.h>
namespace tryn::gfx
{
	class InstancedModelChild;
	
	class InstancedModelParent
	{
		friend class InstancedModelChild;
	public:
		template <TechniqueClass... T>
		static InstancedModelParent Make(gfx::IGraphics& gfx, const std::string& path, glm::vec3 scale = { 1.0f,1.0f,1.0f }, std::optional<std::uint32_t> numInstances = std::nullopt)
		{
			InstancedModelParent tempObject;

			tempObject.pBase = std::unique_ptr<Model>(Model::MakeNew<T...>(gfx, path, scale, true));

			tempObject.instancedGroup = "InstaceGroup";
			tempObject.instancedGroup += path;

			trynass_msg(numInstances.value_or(10) != 0, L"numInstances cannot be 0!");

			tempObject.numInstanced = 0;
			tempObject.upperLimit = numInstances.value_or(10);
			ConstantBufferLayout::Node arrayElement_(ConstantBufferLayout::Type::Struct, "arrayStruct");
			arrayElement_.Append(ConstantBufferLayout::Type::Matrix4, "transform");
			tempObject.arrayElement = std::move(arrayElement_);
			tempObject.pTransformationBuffers.reserve(tempObject.pBase->GetMeshAmount());

			tempObject.Resize(tempObject.upperLimit);

			return tempObject;
		}
		void Submit(const glm::mat4& entityTransform);
		InstancedModelChild Instanciate();
		void Instanciate(std::span<InstancedModelChild> childSpan);
		IInstanceBuffer& RequestInstanceBuffer(std::uint16_t key);
	private:
		InstancedModelParent() = default;
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