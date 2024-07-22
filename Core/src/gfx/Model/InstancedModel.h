#pragma once
#include <optional>
#include <span>
#include <Core/third/dynamic_bitset.hpp>
#include "Core/src/gfx/ConstantBuffer.h"
#include "Core/third/glm/fwd.hpp"
#include <Core/src/ser/StreamIO.h>
#include <Core/src/gfx/Model/Model.h>

namespace tryn::gfx
{
	class InstancedModelChild;
	class IGraphics;

	class InstancedModelParent
	{
		template <typename T> friend struct ser::TypeSerializer;
		friend class InstancedModelChild;
	public:
		InstancedModelParent(gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f }, std::optional<std::uint32_t> numInstances = std::nullopt);
		InstancedModelParent();
		~InstancedModelParent();
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

		struct Serializer : public tryn::ser::Serializer<InstancedModelChild>
		{
			static void Write(const tryn::ser::StreamWriter& streamWriter, const InstancedModelChild& data, const bool binary = true,
			                  const std::string& name = "");

			template <typename Data = void>
			static InstancedModelChild Read(const tryn::ser::StreamReader& streamReader, const bool binary = true, const Data* pExtraData = nullptr)
			{
				auto pParentModel = streamReader.ReadSerialized<InstancedModelParent*>(binary, pExtraData);
				
				return pParentModel->Instanciate();
			}

			template <typename Data = void>
			static void Read(InstancedModelChild& data, const tryn::ser::StreamReader& streamReader, const bool binary = true, const Data* pExtraData = nullptr)
			{
				auto pParentModel = streamReader.ReadSerialized<InstancedModelParent*>(binary, pExtraData);
				data = pParentModel->Instanciate();
			}
		};
	};
}
namespace tryn::ser
{
	template <typename T>
		concept HasGfxPointer = requires (T t) {
			std::same_as<decltype(t.pGfx), gfx::IGraphics*>;
		};

	template <typename T>
	concept InstancedModelParentPointer = requires (T t)
	{
		std::is_same_v<decltype(*t), gfx::InstancedModelParent>;
	};

	template <InstancedModelParentPointer T>
	struct TypeSerializer<T>
	{
		static void Write(const StreamWriter& streamWriter, T const& pData, const bool binary = true, const std::string& name = "")
		{
			streamWriter.Serialize(pData->pBase);
		}
		template <typename Data = void>
		static T Read(const StreamReader& streamReader, const bool binary = true, const Data* extraData = nullptr)
		{
			static_assert(HasGfxPointer<Data> && extraData != nullptr, "The InstanceModelParent* Serializer requires extra data of type tryn::gfx::IGraphics*!");
			return {nullptr};
		}
	};
}