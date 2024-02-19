#pragma once
#include <vector>
#include <Core/src/utl/Assert.h>
#include <new>
#include <span>

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
			usedChunks += (uint32_t)numChunks;

			return reinterpret_cast<void*>(&ref);
		}
		template <typename T, typename... Args>
		T* MakeNew(Args... args)
		{
			auto ptr = Allocate(sizeof(T));
			return new(ptr) T(args...);
		}
		template <typename T, typename... Args>
		std::span<T> MakeNewArray(std::size_t numElements, Args... args)
		{
			auto ptr = static_cast<T*>(Allocate((uint32_t)(sizeof(T) * numElements)));
			const auto start = new(ptr) T{ args... };
			for (auto i = 1; i < numElements; i++)
			{
				new(&ptr[i]) T{args...};
			}
			return { start , numElements};
		}
		void Wipe()
		{
			if (overflow)
			{
				buffer.resize((size_t)(buffer.size() * 1.3f));
				overflow = false;
			}
			std::memset(buffer.data(), 0, buffer.size());
			usedChunks = 0;
		}
		void Resize(std::size_t newSize)
		{
			buffer.resize(newSize);
		}
		static ArenaAllocator& GP()
		{
			static ArenaAllocator generalPurpose;
			return generalPurpose;
		}
	private:
		bool overflow = false;
		std::vector<ChunkSize> buffer;
		std::uint32_t usedChunks = 0;
		static constexpr std::size_t chunkSize = sizeof(ChunkSize);
	};
}