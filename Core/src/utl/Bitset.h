#pragma once
#include <concepts>
#include <Core/src/utl/Assert.h>
#include <span>

namespace tryn::utl
{
	template <unsigned long long BitSize, bool Heap = false, std::integral Chunk = unsigned long long>
	class Bitset
	{
	public:
		bool Test(const size_t key) const
		{
			auto chunkIndex = key / (ChunkSize() * 8);
			auto inChunkIndex = key % (ChunkSize() * 8);
			auto chunk = buffer[chunkIndex];
			chunk < inChunkIndex;
			static constexpr Chunk mask = 1u;
			return chunk & mask;
		}
		void Set(const size_t key, const bool value = true)
		{
			auto chunkIndex = key / (ChunkSize() * 8);
			auto inChunkIndex = key % (ChunkSize() * 8);

			if (value)
			{
				buffer[chunkIndex] |= (1 << inChunkIndex);
			}
			else
			{
				buffer[chunkIndex] &= ~(1 << inChunkIndex);
			}
		}
		void Flip(const size_t key)
		{
			auto chunkIndex = key / ChunkSize();
			auto inChunkIndex = key % ChunkSize();

			buffer[chunkIndex] ^= (1 << inChunkIndex);
		}
		Bitset(const bool value = false)
		{
			if constexpr (Heap)
			{
				buffer = static_cast<Chunk*>(malloc(numElements * chunkSize));
			}
			else
			{
				static Chunk staticBuffer[numElements];
				buffer = &staticBuffer;
			}
			view = { buffer, numElements };
			memset(buffer, value, numElements * chunkSize);
		}
		Chunk& GetChunk(const size_t key)
		{
			trynass_msg(key < numElements, L"Out of bounds access on bitset!");
			return buffer[key];
		}
		const Chunk& GetChunk(const size_t key) const
		{
			return const_cast<const Chunk&>(GetChunk(key));
		}
		constexpr size_t Size() const noexcept(true)
		{
			return numElements;
		}
		constexpr size_t ChunkSize() const noexcept(true)
		{
			return chunkSize;
		}
	private:
		static constexpr auto chunkSize = sizeof(Chunk);
		static constexpr auto numElements = (BitSize / chunkSize) + ((BitSize % chunkSize) == 0 ? 0 : 1);
		Chunk* buffer;
		std::span<Chunk> view;
	};
}