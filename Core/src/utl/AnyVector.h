#pragma once
#include <vector>
#include <tuple>
#include <span>

namespace tryn::utl
{
	class AnyVector
	{
	public:
		template <typename T>
		void PushBack(T&& type)
		{
			const auto byteSizeOfT = sizeof(T);
			accumulatedOffset += byteSizeOfT;
			buffer.insert(buffer.end(), byteSizeOfT, 0u);
			offsetVector.push_back(accumulatedOffset, byteSizeOfT);
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
		using ElementSize = std::uint16_t;
		using Offset = std::uint32_t;
		std::vector<std::pair<Offset, ElementSize>> offsetVector;
		std::uint32_t accumulatedOffset = 0;
		std::uint32_t elCount = 0;
	};
}