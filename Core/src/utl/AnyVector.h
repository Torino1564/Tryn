#pragma once
#include <vector>
#include <tuple>
#include <span>
#include <Core/src/utl/Assert.h>
namespace tryn::utl
{
	// WARNING: This container does not call the destructors of its contents. ONLY USE IF YOU KNOW WHAT YOU ARE DOING
	class AnyVector
	{
	public:
		template <typename T>
		void PushBack(T&& type)
		{
			const auto byteSizeOfT = sizeof(T);
			const auto vectorEnd = buffer.size();
			buffer.insert(buffer.end(), (const size_t) byteSizeOfT,(std::byte)0u);
			auto pData = buffer.data();
			new(pData + vectorEnd) T(std::move(type));
			
			offsetVector.push_back({ accumulatedOffset, byteSizeOfT });
			accumulatedOffset += byteSizeOfT;
			elCount++;
		}
		std::span<std::byte> At(std::size_t i)
		{
			auto [offset, size] = offsetVector[i];
			return { &buffer[offset], size };
		}
		void* operator[](std::size_t i)
		{
			return &buffer[offsetVector[i].first];
		}
		void Clear()
		{
			accumulatedOffset = 0;
			elCount = 0;
			buffer.clear();
			offsetVector.clear();
		}
		std::size_t Size() const
		{
			return elCount;
		}
	private:
		std::vector<std::byte> buffer;
		using ElementSize = std::size_t;
		using Offset = std::uint32_t;
		std::vector<std::pair<Offset, ElementSize>> offsetVector;
		std::uint32_t accumulatedOffset = 0;
		std::uint32_t elCount = 0;
	};
}