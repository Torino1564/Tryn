#pragma once
#include "StringHasher.h"
#include <unordered_map>

namespace tryn::utl
{
	class TypeData;
	class TypeRegister
	{
	public:
		void Append(TypeData& newData);

		static TypeRegister& Get()
		{
			static TypeRegister singleton;
			return singleton;
		}
		const TypeData& Resolve(const std::string& name);
		const TypeData& Resolve(UUID_t uuid);
	private:
		TypeRegister() = default;
		std::unordered_map<UUID_t, TypeData> uuidToType;
		std::unordered_map<std::string, TypeData> nameToType;
	};

	class TypeData
	{
	public:
		template <typename T>
		static TypeData MakeTypeData()
		{
			auto data = TypeData(ZT_TYPE_UUID(T), ZT_TYPE_OF(T), sizeof(T));
			TypeRegister::Get().Append(data);
			return data;
		}
		TypeData(const UUID_t id, std::string_view name, const size_t byteSize);
		TypeData(const TypeData& rhs);
		TypeData() = default;

		UUID_t id = {};
		std::string name = {};
		size_t byteSize = {};
	};

	template <typename T>
	class Instanciator
	{
	public:
		const static auto var = TypeData::MakeTypeData<T>();
	};

	template <typename T>
	UUID_t UuidOf()
	{
		auto data = Instanciator<T>::var;
		return data.id;
	}


}
