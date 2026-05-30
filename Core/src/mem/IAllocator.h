#pragma once
#include "BasicPoolAllocator.h"

namespace tryn::mem
{
	template <typename T , typename Allocator = BasicPoolAllocator<>>
	class IAllocator
	{
		using Pointer = T*;

		Pointer allocate(std::size_t byteNumber)
		{
			return static_cast<Pointer>(allocator->Request(byteNumber));
		}
		void deallocate(Pointer ptr, std::size_t byteNumber)
		{

		}
	private:
		Allocator* allocator;
	};
}