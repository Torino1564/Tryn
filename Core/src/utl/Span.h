#pragma once
#include <stdint.h>
#include <vector>
#include <Core/src/utl/Assert.h>

namespace tryn::utl
{
	template <typename T>
	class Span
	{
	public:
		Span() = default;
		Span( T* ptr, uint32_t size )
			:
			buffer(ptr), size(size)
		{}
		T& operator[](const uint32_t index)
		{
			return buffer[index];
		}

		T* begin() const
		{
			return buffer[0];
		}

		const T& operator[](const uint32_t index) const
		{
			return buffer[index];
		}

		void operator=(const Span<T>& rhs)
		{
			buffer = rhs.buffer;
			size = rhs.size;
		}

		std::size_t Size() const
		{
			return size;
		}
	private:
		T* buffer = nullptr;
		size_t size = 0;
	};

	template <typename T>
	class MultiSpan
	{
	public:
		auto Size() const noexcept
		{
			return numElements;
		}
		void PushBack(std::span<T> newElement)
		{
			buffer.push_back(newElement);
			numElements += newElement.size();
		}
		void Clear()
		{
			buffer.clear();
			numElements = 0;
		}
		T& operator[](std::uint32_t index)
		{
			std::size_t accumulatedSize = 0;
			for (auto& element : buffer)
			{
				auto newIndex = index - accumulatedSize;
				if ( newIndex < element.size())
				{
					return element[newIndex];
				}
				else
				{
					accumulatedSize += element.size();
				}
			}
			trynchk_fail.msg(L"Out of bounds access in a MultiSpan").ex();
		}
	private:
		std::vector<std::span<T>> buffer;
		std::size_t numElements = 0;
	};
}