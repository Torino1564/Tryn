//#pragma once
//#include <unordered_map>
//#include "StringHasher.h"
//#include <Core/src/utl/Exception.h>
//
//namespace tryn::utl
//{
//	template <typename Interface>
//	class TypeRegister
//	{
//	public:
//		using IndexType = uint16_t;
//
//		template <typename T>
//		static IndexType Register()
//		{
//			const auto index = reg.pRegisterables.size();
//
//			reg.pRegisterables.push_back(std::make_unique<T>());
//			reg.indexToUuid.insert({ T::GetUUID(), index });
//			reg.uuidToIndex.insert({ index, T::GetUUID() });
//
//			return index;
//		}
//		static Interface& GetByIndex(const IndexType index)
//		{
//			return reg.pRegisterables[index];
//		}
//		static Interface& GetByUUID(const UUID_t uuid)
//		{
//			const auto index = reg.uuidToIndex.find(uuid);
//			if (index == reg.uuidToIndex.end())
//			{
//
//			}
//		}
//	private:
//		static TypeRegister& Get()
//		{
//			static TypeRegister singleton;
//			return singleton;
//		}
//		static auto& reg = Get();
//		std::unordered_map<UUID_t, IndexType> uuidToIndex;
//		std::unordered_map<IndexType, UUID_t> indexToUuid;
//		std::vector<std::unique_ptr<Interface>> pRegisterables;
//	};
//}
