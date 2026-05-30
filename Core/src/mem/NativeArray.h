#pragma once
#include "Allocator.h"

namespace tryn::mem
{
	template <typename T>
	class NativeArray
	{
	public:
		template<typename Al>
		NativeArray(std::uint32_t numElements, Al& alloc)
			:
			length(numElements), buffer((T*)alloc.Allocate(numElements * sizeof(T)))
		{

		}
		
		T& operator[](std::uint32_t index)
		{
			return buffer[index];
		}

		T* begin()
		{
			return buffer;
		}

		T* end()
		{
			return buffer + length;
		}

		const T* data() const
		{
			return buffer;
		}

		T* data()
		{
			return buffer;
		}

		std::size_t size() const
		{
			return length;
		}
	private:
		T* buffer;
		const std::size_t length;
		static constexpr std::size_t stride = sizeof(T);
	};
}

