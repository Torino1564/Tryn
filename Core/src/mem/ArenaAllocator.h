#pragma once
#include <vector>
#include <Core/src/utl/Assert.h>
#include <span>
#include <Core/src/utl/String.h>
#include <Core/src/log/Log.h>

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
				buffer.resize(buffer.size() * 1.3f, 0);
				trylog.info(utl::ToWide(std::format("Growing buffer. Requested {} chunks and {} out of {} were in use.", numChunks, usedChunks, buffer.size())));
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
		std::vector<ChunkSize> buffer;
		std::uint32_t usedChunks = 0;
		static constexpr std::size_t chunkSize = sizeof(ChunkSize);
	};
}