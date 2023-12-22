#pragma once
#include <vector>
#include <Core/src/utl/Assert.h>
#include <new>

namespace tryn::mem
{
	template <typename ChunkSize = unsigned long long>
	class ArenaAllocator
	{
	public:
		ArenaAllocator(int numChunks = 16'000)
		{
			buffer.resize(numChunks, (ChunkSize)0);
		}
		void* Allocate(std::uint32_t numBytes)
		{
			const auto numChunks = (numBytes / chunkSize) + 1;
			
			if (numChunks + usedChunks >= buffer.size())
			{
				overflow = true;
				return malloc(numBytes);
			}

			auto& ref = buffer[usedChunks];
			usedChunks += numChunks;

			return reinterpret_cast<void*>(&ref);
		}
		template <typename T, typename... Args>
		T* MakeNew()
		{
			auto ptr = Allocate(sizeof(T));
			return new(ptr) T(Args...);
		}
		void Wipe()
		{
			if (overflow)
			{
				buffer.resize(buffer.size() * 1.3f);
				overflow = false;
			}
			std::memset(buffer.data(), 0, buffer.size());
			usedChunks = 0;
		}
		void Resize(std::size_t newSize)
		{
			buffer.resize(newSize);
		}
	private:
		bool overflow = false;
		std::vector<ChunkSize> buffer;
		std::uint32_t usedChunks = 0;
		static constexpr std::size_t chunkSize = sizeof(ChunkSize);
	};
}