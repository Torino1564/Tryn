#pragma once
#include <Core/src/utl/Exception.h>
#include <vector>

ZT_EX_DEF(CPUBufferException);

namespace Microsoft::glTF
{
	struct MeshPrimitive;
}
struct aiMesh;

namespace tryn::gfx
{
	class WinGLTFLoaderContext;

	class CPUBuffer
	{
	public:
		virtual ~CPUBuffer() = default;
		constexpr bool Dirty() const noexcept;
		constexpr void SetClean() noexcept;
		constexpr void SetDirty() noexcept;
		constexpr virtual void* Data() const noexcept = 0;
		constexpr virtual std::size_t ByteSize() const noexcept = 0;
		virtual std::size_t Size() const noexcept = 0;
		virtual std::size_t Stride() const noexcept = 0;
		virtual void Resize(std::size_t newSize);

	protected:
		bool dirty = true;
	};

	class FlatBuffer : public CPUBuffer
	{
	public:
		FlatBuffer(int byteSize = 0);
		constexpr void* Data() const noexcept override;
		std::size_t Stride() const noexcept override;
		constexpr std::size_t ByteSize() const noexcept override;
		std::size_t Size() const noexcept override;
		void Resize(std::size_t newSize) override;

	private:
		std::vector<std::byte> buffer;
	};

	class IndexBuffer : public CPUBuffer
	{
	public:
		IndexBuffer(int indexCount = 0);
		IndexBuffer(const std::vector<uint32_t>& data);
		IndexBuffer(const aiMesh& mesh);
		IndexBuffer(const Microsoft::glTF::MeshPrimitive& primitive, const gfx::WinGLTFLoaderContext& context);
		constexpr void* Data() const noexcept override;
		std::size_t Stride() const noexcept override;
		constexpr std::size_t ByteSize() const noexcept override;
		std::size_t Size() const noexcept override;
		void Resize(std::size_t newSize) override;

	private:
		static constexpr int stride = sizeof(uint32_t);
		std::vector<uint32_t> buffer;
	};

	constexpr bool CPUBuffer::Dirty() const noexcept
	{
		return dirty;
	}

	constexpr void CPUBuffer::SetClean() noexcept
	{
		dirty = false;
	}

	constexpr void CPUBuffer::SetDirty() noexcept
	{
		dirty = true;
	}

	constexpr void* FlatBuffer::Data() const noexcept
	{
		return (void*)(buffer.data());
	}

	constexpr std::size_t FlatBuffer::ByteSize() const noexcept
	{
		return buffer.size();
	}

	constexpr void* IndexBuffer::Data() const noexcept
	{
		return (void*)(buffer.data());
	}

	constexpr std::size_t IndexBuffer::ByteSize() const noexcept
	{
		return buffer.size() * stride;
	}
}
