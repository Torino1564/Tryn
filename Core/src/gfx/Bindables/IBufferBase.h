#pragma once
#include <any>
#include <Core/src/gfx/ConstantBuffer.h>
#include <Core/src/gfx/IBufferFwd.h>
#include <Core/src/gfx/Render/TechniqueProbe.h>
#include <Core/src/gfx/Vertex.h>
#include <Core/src/utl/EmptyType.h>
#include <memory>
#include "Bindable.h"

ZT_EX_DEF(BufferMissmatchException);

namespace tryn::gfx
{
	template <typename T>
	struct IsBufferType_t : std::false_type{};

	template <BufferType Type, CachingPolicy Policy>
	struct IsBufferType_t<IBufferBase<Type, Policy>> : std::true_type{};

	template <typename T>
	concept Buffer_T = IsBufferType_t<T>::value;

	template <BufferType Type>
	constexpr const char* GetTypeString() {return nullptr;}

	template <>
	constexpr const char* GetTypeString<BufferType::VtxConstant>() { return "VtxConstant"; }

	template <>
	constexpr const char* GetTypeString<BufferType::Vertex>() { return "Vertex"; }

	template <>
	constexpr const char* GetTypeString<BufferType::PxConstant>() { return "PxConstant"; }

	template <>
	constexpr const char* GetTypeString<BufferType::Instance>() { return "Instance"; }

	template <>
	constexpr const char* GetTypeString<BufferType::Index>() { return "Index"; }

	class IBuffer : public IBindable
	{
	public:
		virtual CPUBuffer& GetCPUBuffer() = 0;

	protected:
		std::shared_ptr<CPUBuffer> pCPUBuffer;
		std::string path;
		std::string tag;
	};

	template<BufferType Type, CachingPolicy Policy>
	class IBufferBase : public IBuffer
	{
	public:
		static std::shared_ptr<IVtxConstantBuffer> Resolve(const IGraphics& gfx, ConstantBufferLayout&& cbl, int slot = 0, const std::string& tag = "?")
			requires (Type == BufferType::VtxConstant && Policy == CachingPolicy::Caching);
		static std::shared_ptr<IVtxConstantBufferNCach> Resolve(const IGraphics& gfx, ConstantBufferLayout&& cbl, int slot = 0, const std::string& tag = "?")
			requires (Type == BufferType::VtxConstant && Policy == CachingPolicy::NonCaching);
		static std::shared_ptr<IPxConstantBuffer> Resolve(const IGraphics& gfx, ConstantBufferLayout&& cbl, int slot = 0, const std::string& tag = "?")
			requires (Type == BufferType::PxConstant && Policy == CachingPolicy::Caching);
		static std::shared_ptr<IPxConstantBufferNCach> Resolve(const IGraphics& gfx, ConstantBufferLayout&& cbl, int slot = 0, const std::string& tag = "?")
			requires (Type == BufferType::PxConstant && Policy == CachingPolicy::NonCaching);
		static std::shared_ptr<IVertexBuffer> Resolve(const IGraphics& gfx, const std::shared_ptr<VertexBuffer>& cpuBuffer, const std::string& tag = "?")
			requires (Type == BufferType::Vertex && Policy == CachingPolicy::Caching);
		static std::shared_ptr<IIndexBuffer> Resolve(const IGraphics& gfx, std::shared_ptr<IndexBuffer> indices, const std::string& tag = "?")
			requires (Type == BufferType::Index && Policy == CachingPolicy::Caching);
		static std::shared_ptr<IInstanceBuffer> Resolve(const IGraphics& gfx, const ConstantBufferLayout::Node& node, int slot, std::size_t size)
			requires (Type == BufferType::Instance);
		static std::string GenerateID(const IGraphics& gfx, const std::shared_ptr<VertexBuffer>& cpuBuffer, const std::string& tag = "?")
			requires (Type == BufferType::Vertex);
		static std::string GenerateID(const IGraphics& gfx, const std::shared_ptr<IndexBuffer>& indices, const std::string& tag = "?")
			requires (Type == BufferType::Index);
		static std::string GenerateID(const IGraphics& gfx, ConstantBufferLayout& cbl, int slot = 0, const std::string& tag = "?")
			requires (Type == BufferType::PxConstant || Type == BufferType::VtxConstant);
		void Bind() override;
		void Bind(const IContext& context) override;
		virtual std::vector<std::any> GetLayoutFromVB() const;
		virtual std::vector<std::any> GetSlottedLayoutFromVB(int slot) const;
		virtual std::string_view Test() const;
		static constexpr const char* GetType();
		std::string_view GetPath() const;
		std::string_view GetTag() const;
		const VertexLayout& GetLayout() const requires (Type == BufferType::Vertex);
		virtual void Resize(const std::size_t newSize);
		ElementView operator[](const std::string& id) const
			requires (Type == BufferType::VtxConstant || Type == BufferType::PxConstant);
		void Accept_(TechniqueProbe& probe)
			requires (Type == BufferType::VtxConstant || Type == BufferType::PxConstant);
		CPUBuffer& GetCPUBuffer() override;
		ConstantBuffer& GetConstantBuffer() const requires (Type != BufferType::Index && Type != BufferType::Vertex);
		VertexBuffer& GetVertexBuffer() requires (Type == BufferType::Vertex);
		const VertexBuffer& GetVertexBuffer() const requires (Type == BufferType::Vertex);
		static constexpr BufferType bufferType = Type;
		static constexpr CachingPolicy policy = Policy;

	protected:
		uint16_t slot = 0;
		size_t gpuSize = 0;
	};

	template <BufferType Type, CachingPolicy Policy>
	constexpr const char* IBufferBase<Type, Policy>::GetType()
	{
		return GetTypeString<Type>();
	}
}
