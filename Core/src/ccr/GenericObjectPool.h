#pragma once
#include "GenericTaskQueue.h"
#include <vector>
#include <Core/third/boost/DynamicBitset.h>

namespace tryn::ccr
{
	template<class T>
	class GenericObjectPool
	{
		friend class GenericUniqueReference;
	public:
		GenericObjectPool(std::size_t size = 0)
		{
			
		}
	private:
		// Queue Stuff
		void Request_();
		void Release_();
		std::vector<T> entries;
		boost::dynamic_bitset book;

		//Thread Stuff
		void ThreadKernel_();
		ccr::GenericTaskQueue queue_;
		std::jthread thread_;
	};

	template<class T>
	class GenericUniqueReference
	{
	public:
		~GenericUniqueReference()
		{

		}
		T* operator->() const override
		{
			return pObject;
		}
	private:
		T* pObject;
		GenericObjectPool<T>* pPool;
	};
}