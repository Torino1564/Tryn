#pragma once
#include "Bindable.h"
#include <Core/src/gfx/CPUBuffer.h>

namespace tryn::gfx
{
	enum class CachingPolicy
	{
		Caching,
		NonCaching
	};

	enum class BufferType
	{
		Vertex,
		VtxConstant,
		PxConstant,
		Instance
	};

	template <BufferType Type>
	constexpr const char* GetTypeString() {}

	template <>
	constexpr const char* GetTypeString<BufferType::VtxConstant>() { return "VtxConstant"; }

	template <>
	constexpr const char* GetTypeString<BufferType::Vertex>() { return "Vertex"; }

	template <>
	constexpr const char* GetTypeString<BufferType::PxConstant>() { return "PxConstant"; }

	template <>
	constexpr const char* GetTypeString<BufferType::Instance>() { return "Instance"; }

	template<int>
	struct empty_t {};

	template<BufferType Type, CachingPolicy Policy = CachingPolicy::Caching>
	class IBuffer : public IBindable
	{
	public:
		static std::shared_ptr<IVtxConstBufCach> Resolve(IGraphics& gfx, ConstantBufferLayout&& cbl, int slot = 0, std::string tag = "?");
		static std::shared_ptr<IVtxConstBufNCach> Resolve(IGraphics& gfx, ConstantBufferLayout&& cbl, int slot = 0, std::string tag = "?");
		static std::shared_ptr<IPxConstBufCach> Resolve(IGraphics& gfx, ConstantBufferLayout&& cbl, int slot = 0, std::string tag = "?");
		static std::shared_ptr<IPxConstBufNCach> Resolve(IGraphics& gfx, ConstantBufferLayout&& cbl, int slot = 0, std::string tag = "?");
		static std::shared_ptr<IVertexBufCach> Resolve(IGraphics& gfx, std::shared_ptr<VertexBuffer> cpuBuffer, std::string tag = "?");

		static std::string GenerateID(IGraphics& gfx, const std::string& path)
		{
			decltype(auto) typeStr = IGraphics::GetAPIString();
			std::string UID(typeStr);
			UID += std::format("#{}#{}#", GetTypeString<Type>(), path);

			if constexpr (Policy == CachingPolicy::Caching)
			{
				UID += "#Caching";
			}

			return UID;
		}
		constexpr BufferType GetType() const
		{
			return GetTypeString<Type>();
		}
		virtual ~IBuffer() = default;

		std::string_view GetPath() const
		{
			return path;
		}
	protected:
		std::string path;
		std::string tag;
		std::shared_ptr<CPUBuffer> pCPUBuffer;
		[[no_unique_address]] std::conditional<Type == BufferType::PxConstant || Type == BufferType::VtxConstant, int, empty_t<0>>::type slot;
		[[no_unique_address]] std::conditional<Type == BufferType::PxConstant || Type == BufferType::VtxConstant, ConstantBufferLayout, empty_t<1>>::type layout;
	};

	using IVtxConstBufCach = IBuffer<BufferType::VtxConstant, CachingPolicy::Caching>;
	using IVtxConstBufNCach = IBuffer<BufferType::VtxConstant, CachingPolicy::NonCaching>;
	using IPxConstBufCach = IBuffer<BufferType::PxConstant, CachingPolicy::Caching>;
	using IPxConstBufNCach = IBuffer<BufferType::PxConstant, CachingPolicy::NonCaching>;
	using IVertexBuffer = IBuffer<BufferType::Vertex, CachingPolicy::Caching>;
}