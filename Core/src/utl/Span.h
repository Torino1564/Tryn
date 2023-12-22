#pragma once
#include <stdint.h>

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
}