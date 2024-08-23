#pragma once
#include <optional>
#include <span>
#include <Core/third/dynamic_bitset.hpp>
#include "Core/src/gfx/ConstantBuffer.h"
#include "Core/third/glm/fwd.hpp"
#include <Core/src/ser/StreamIO.h>

namespace tryn::gfx
{
	class InstancedModelChild;
	class IGraphics;

	class InstancedModelParent
	{
		template <typename T> friend struct ser::TypeSerializer;
		friend class InstancedModelChild;
	public:
		InstancedModelParent(const gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f }, std::optional<std::uint32_t> numInstances = std::nullopt);
		InstancedModelParent();
		~InstancedModelParent();
		InstancedModelParent(const InstancedModelParent&) = delete;
		InstancedModelParent(InstancedModelParent&&);

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
		std::vector<std::unique_ptr<IInstanceBuffer>> pTransformationBuffers;
	};


	class InstancedModelChild
	{
		friend class InstancedModelParent;
	public:	
		~InstancedModelChild();
		void Submit(const glm::mat4& transformation) const;

		std::uint16_t instanceID = {};
		InstancedModelParent* pParentModel = nullptr;

		struct Serializer : public tryn::ser::Serializer<InstancedModelChild>
		{
			static void Write(const tryn::ser::StreamWriter& streamWriter, const InstancedModelChild& data, const bool binary = true,
			                  const std::string& name = "");

			static InstancedModelChild Read(const tryn::ser::StreamReader& streamReader, const bool binary = true, const ser::ExtraDataPack* pExtraData = nullptr);

			static void Read(InstancedModelChild& data, const tryn::ser::StreamReader& streamReader, const bool binary = true, const ser::ExtraDataPack* pExtraData = nullptr);
		};
	};
}
namespace tryn::ser
{
	template <typename T>
		concept HasGfxPointer = requires (T t) {
			std::same_as<decltype(t.pGfx), gfx::IGraphics*>;
		};

	template <typename Ptr>
	concept PointerLike = std::is_pointer_v<Ptr> || requires (Ptr p) {
    { *p };
    { static_cast<bool>(p) };
    { p.operator->() } -> std::convertible_to<decltype( &*p )>;
};

	template <typename T>
	concept InstancedModelParentPointer = PointerLike<T> && std::is_same_v<std::remove_pointer_t<T>, gfx::InstancedModelParent> || std::is_same_v<std::remove_reference_t<decltype(*std::declval<T>())>, gfx::InstancedModelParent>;

	template <>
	struct TypeSerializer<std::unique_ptr<gfx::InstancedModelParent>>
	{
		static void Write(const StreamWriter& streamWriter, const std::unique_ptr<gfx::InstancedModelParent>& pData, const bool binary = true, const std::string& name = "");
		static std::unique_ptr<gfx::InstancedModelParent> Read(const StreamReader& streamReader, const bool binary = true, const ser::ExtraDataPack* pExtraData = nullptr);

		static void Read(std::unique_ptr<gfx::InstancedModelParent>& data, const StreamReader& streamReader, const bool binary = true, const ser::ExtraDataPack* = nullptr);
	};
}