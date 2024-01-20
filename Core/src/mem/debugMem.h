#pragma once
#include <bitset>
#include <Core/src/win/TrynWin.h>
#include <memoryapi.h>
#include <cstdlib>
#include <Core/src/utl/Bitset.h>

namespace tryn::mem
{
	template <int ChunkByteSize = 4096u, size_t BufferSize = 10'000'000'000>
	class DebugMemoryManager
	{
	public:
		static DebugMemoryManager& Get()
		{
			static DebugMemoryManager singleton;
			return singleton;
		}
		void* allocate(size_t size)
		{
			auto chunksNeeded = (size / chunkSize) + ((size % chunkSize) == 0 ? 0 : 1);
			size_t contiguousChunks = 0;

			for (int i = 0; i < booker.Size(); i++)
			{
				auto chunk = booker.GetChunk(i);
				decltype(chunk) mask = 1u;

				for (int j = 0; j < sizeof(decltype(chunk)) * 8; j++)
				{
					if (((mask << j) & chunk) == 0u)
					{
						contiguousChunks++;
						if (contiguousChunks == chunksNeeded)
						{
							const auto index = (i * booker.ChunkSize() * 8) + j - (contiguousChunks - 1);
							return SetAndReturn(index, chunksNeeded);
						}
					}
					else
					{
						contiguousChunks = 0;
					}
				}
			}
			
			return nullptr;
		}
		void deallocate(void* ptr)
		{
			const auto chunkOffset = (static_cast<uint8_t*>(ptr) - static_cast<uint8_t*>(startPtr)) / chunkSize;
			const auto numChunks = sizeBufferView[chunkOffset];

			for (int i = 0; i < numChunks; i++)
			{
				booker.Set(chunkOffset + i, false);
			}

			auto rv = VirtualProtect(ptr, numChunks * chunkSize, na, &rw);
		}
	private:
		void* SetAndReturn(size_t position, size_t amount)
		{
			for (int i = 0; i < amount; i++)
			{
				booker.Set(position + i);
			}

			sizeBufferView[position] = amount;

			auto rv = VirtualProtect(static_cast<uint8_t*>(startPtr) + (position * chunkSize), chunkSize * amount, rw, &na);

			return static_cast<uint8_t*>(startPtr) + (position * chunkSize);
		}
		DebugMemoryManager()
		{
			SYSTEM_INFO sysInfo = {};
			GetSystemInfo(&sysInfo);
			sysInfo.dwPageSize;

			startPtr = VirtualAlloc(nullptr, BufferSize, MEM_COMMIT | MEM_RESERVE, rw);
			auto rv = VirtualProtect(startPtr, BufferSize, na, &rw);

			sizeBuffer = (size_t*)malloc(sizeof(size_t) * numChunks);
			sizeBufferView = std::span<size_t>(sizeBuffer, numChunks);

			memset(sizeBuffer, 0, numChunks * sizeof(size_t));

			booker = utl::Bitset<numChunks, true>(false);
			chunkPointer = 0;
		}
		void* startPtr = nullptr;
		size_t chunkPointer = 0;
		DWORD rw = PAGE_READWRITE;
		DWORD na = PAGE_READWRITE | PAGE_GUARD;
		static inline constexpr size_t chunkSize = ChunkByteSize;
		static inline constexpr auto numChunks = (BufferSize / chunkSize) + ((BufferSize % chunkSize) == 0 ? 0 : 1);
		utl::Bitset<numChunks, true> booker;
		size_t* sizeBuffer = nullptr;
		std::span<size_t> sizeBufferView;
	};
}

void* operator new(decltype(sizeof(0)) size) noexcept(false)
{
	return tryn::mem::DebugMemoryManager<>::Get().allocate(size);
}
void operator delete(void* ptr)
{
	if (ptr != nullptr)
	{
		tryn::mem::DebugMemoryManager<>::Get().deallocate(ptr);
	}
}
