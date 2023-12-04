#pragma once
#include "Bindable.h"
#include <Core/src/gfx/CPUBuffer.h>
#include <Core/src/gfx/BindablePool.h>
#include <Core/src/gfx/IBufferFwd.h>
#include <Core/src/gfx/Bindables/ConstantBuffer.h>
#include <Core/src/gfx/Vertex.h>
#include <any>

ZT_EX_DEF(BufferMissmatchException);

namespace tryn::gfx
{
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

	template <>
	constexpr const char* GetTypeString<BufferType::Index>() { return "Index"; }

	template<int>
	struct empty_t {};

	template<BufferType Type, CachingPolicy Policy>
	class IBuffer : public IBindable
	{
	public:
		template<BufferType T = Type, CachingPolicy P = Policy>
		requires (T == BufferType::VtxConstant && P == CachingPolicy::Caching)
		static std::shared_ptr<IVtxConstantBuffer> Resolve(IGraphics& gfx, ConstantBufferLayout&& cbl, int slot = 0, std::string tag = "?")
		{
			return gfx::BindablePool::Resolve<IVtxConstantBuffer>(gfx, std::forward<ConstantBufferLayout>(cbl), slot, tag);
		}

		template<BufferType T = Type, CachingPolicy P = Policy>
		requires (T == BufferType::VtxConstant && P == CachingPolicy::NonCaching)
		static std::shared_ptr<IVtxConstantBufferNCach> Resolve(IGraphics& gfx, ConstantBufferLayout&& cbl, int slot = 0, std::string tag = "?")
		{
			return gfx::BindablePool::Resolve<IVtxConstantBufferNCach>(gfx, std::forward<ConstantBufferLayout>(cbl), slot, tag);
		}

		template<BufferType T = Type, CachingPolicy P = Policy>
		requires (T == BufferType::PxConstant && P == CachingPolicy::Caching)
		static std::shared_ptr<IPxConstantBuffer> Resolve(IGraphics& gfx, ConstantBufferLayout&& cbl, int slot = 0, std::string tag = "?")
		{
			return gfx::BindablePool::Resolve<IPxConstantBuffer>(gfx, std::forward<ConstantBufferLayout>(cbl), slot, tag);
		}

		template<BufferType T = Type, CachingPolicy P = Policy>
		requires (T == BufferType::PxConstant && P == CachingPolicy::NonCaching)
		static std::shared_ptr<IPxConstantBufferNCach> Resolve(IGraphics& gfx, ConstantBufferLayout&& cbl, int slot = 0, std::string tag = "?")
		{
			return gfx::BindablePool::Resolve<IPxConstantBufferNCach>(gfx, std::forward<ConstantBufferLayout>(cbl), slot, tag);
		}

		template<BufferType T = Type, CachingPolicy P = Policy>
		requires (T == BufferType::Vertex && P == CachingPolicy::Caching)
		static std::shared_ptr<IVertexBuffer> Resolve(IGraphics& gfx, std::shared_ptr<VertexBuffer> cpuBuffer, std::string tag = "?")
		{
			return gfx::BindablePool::Resolve<IVertexBuffer>(gfx, cpuBuffer, tag);
		}

		template<BufferType T = Type, CachingPolicy P = Policy>
			requires (T == BufferType::Index && P == CachingPolicy::Caching)
		static std::shared_ptr<IIndexBuffer> Resolve(IGraphics& gfx, std::shared_ptr<IndexBuffer> indices, std::string tag = "?")
		{
			return gfx::BindablePool::Resolve<IIndexBuffer>(gfx, indices, tag);
		}

		template <BufferType T = Type>
		requires (T == BufferType::Vertex)
		static std::string GenerateID(IGraphics& gfx, std::shared_ptr<VertexBuffer> cpuBuffer, std::string tag = "?")
		{
			if (tag == "?") return tag;
			decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
			std::stringstream ss;
			ss << typeStr << "#VertexBuffer#" << std::to_string(cpuBuffer->ByteSize()) << "#";

			for (auto& element : cpuBuffer->GetLayout().Elements)
			{
				ss << element.first.GetName() << element.second;
			};
			ss << "#" << tag;

			return ss.str();
		}

		template <BufferType T = Type>
		requires (T == BufferType::Index)
		static std::string GenerateID(IGraphics& gfx, std::shared_ptr<IndexBuffer> indices, std::string tag = "?")
		{
			if (tag == "?") return tag;
			decltype(auto) typeStr = gfx.GetAPIString();
			std::string UID(typeStr);
			UID += "#IndexBuffer#";
			UID += std::to_string(indices->Size());
			UID += '#';
			UID += tag;
			return UID;
		}

		template <BufferType T = Type>
		requires (T == BufferType::PxConstant || T == BufferType::VtxConstant)
		static std::string GenerateID(IGraphics& gfx, ConstantBufferLayout& cbl, int slot = 0, std::string tag = "?")
		{
			if (tag == "?") return tag;
			decltype(auto) typeStr = gfx.GetAPIString();
			std::stringstream ss;
			ss << typeStr << "#VtxConstantBuffer#" << slot << '#' << tag;
			return ss.str();
		}

		void Bind() override
		{
			throw BufferMissmatchException("Invalid call from interface");
		}
		void Bind(IContext& context) override
		{
			throw BufferMissmatchException("Invalid call from interface");
		}
	
		virtual std::vector<std::any> GetLayoutFromVB() const
		{
			throw BufferMissmatchException("Invalid call from interface");
		}

		virtual std::vector<std::any> GetSlottedLayoutFromVB(int slot) const
		{
			throw BufferMissmatchException("Invalid call from interface");
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
		std::string_view GetTag() const
		{
			return tag;
		}
		template <BufferType T = Type> 
		requires (T == BufferType::Vertex)
		VertexLayout& GetLayout()
		{
			return layout;
		}
		template <BufferType T = Type>
		requires (T == BufferType::Vertex)
		const VertexLayout& GetLayout() const
		{
			return layout;
		}

		template <BufferType T = Type>
		requires (T == BufferType::VtxConstant || T == BufferType::PxConstant)
		ElementView operator[](std::string id)
		{
			pCPUBuffer->SetDirty();
			auto& indexTo = layout.GetRoot().IndexByName(id);
			if (indexTo.GetType() == gfx::ConstantBufferLayout::Type::Empty)
			{
				return ElementView{ indexTo, nullptr };
			}
			else
			{
				return ElementView{ indexTo, (char*)(pCPUBuffer->Data()) + indexTo.GetOffset() };
			}
		}
	protected:
		std::string path;
		std::string tag;
		std::shared_ptr<CPUBuffer> pCPUBuffer;
		[[no_unique_address]] std::conditional<Type == BufferType::PxConstant || Type == BufferType::VtxConstant, int, empty_t<0>>::type slot;
		using Layout_Ty = std::conditional_t<
			Type == BufferType::PxConstant || Type == BufferType::VtxConstant || Type == BufferType::Vertex,
			std::conditional_t<Type == BufferType::Vertex, VertexLayout, ConstantBufferLayout>,
			empty_t<1>>;
		[[no_unique_address]] Layout_Ty layout;
	};
}