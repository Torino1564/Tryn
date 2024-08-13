#pragma once
#include <Core/src/utl/Exception.h>

ZT_EX_DEF(CPUBufferException);

namespace tryn::gfx
{
	class CPUBuffer
	{
	public:
		virtual ~CPUBuffer() = default;
		constexpr bool Dirty() const noexcept
		{
			return dirty;
		}
		constexpr void SetClean() noexcept
		{
			dirty = false;
		}
		constexpr void SetDirty() noexcept
		{
			dirty = true;
		}
		constexpr virtual void* Data() const noexcept = 0;
		constexpr virtual std::size_t ByteSize() const noexcept = 0;
		virtual std::size_t Size() const noexcept = 0;
		virtual std::size_t Stride() const noexcept = 0;
		virtual void Resize(const std::size_t newSize)
		{
			throw CPUBufferException(L"This CPU Buffer cannot be resized!");
		}
	protected:
		bool dirty = true;
	};

	class FlatBuffer : public CPUBuffer
	{
	public:
		FlatBuffer(int byteSize = 0)
		{
			buffer.resize(byteSize, (std::byte)0);
		}
		constexpr void* Data() const noexcept override
		{
			return (void*)(buffer.data());
		}
		std::size_t Stride() const noexcept override
		{
			return 0;
		}
		constexpr std::size_t ByteSize() const noexcept override
		{
			return buffer.size();
		}
		std::size_t Size() const noexcept override
		{
			return buffer.size();
		}
		void Resize(const std::size_t newSize) override
		{
			buffer.resize(newSize);
		}

	private:
		std::vector<std::byte> buffer;
	};

	class IndexBuffer : public CPUBuffer
	{
	public:
		IndexBuffer(int indexCount = 0)
		{
			buffer.resize(indexCount);
			dirty = false;
		}
		IndexBuffer(const std::vector<uint32_t>& data)
		{
			buffer = data;
			dirty = false;
		}
		constexpr void* Data() const noexcept override
		{
			return (void*)(buffer.data());
		}
		std::size_t Stride() const noexcept override
		{
			return stride;
		}
		constexpr std::size_t ByteSize() const noexcept override
		{
			return buffer.size() * stride;
		}
		std::size_t Size() const noexcept override
		{
			return buffer.size();
		}
		void Resize(const std::size_t newSize) override
		{
			buffer.resize(newSize);
		}
	private:
		static constexpr int stride = sizeof(uint32_t);
		std::vector<uint32_t> buffer;
	};
}