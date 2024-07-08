#pragma once
#include <sstream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <vector>
#include <format>
#include <Core/src/utl/StatefulMeta/CTV.h>
#include "Serializer.h"
#include <Core/src/utl/StringHasher.h>

#include "Core/src/app/App.h"

#define ZT_DEFINE_SERIALIZER(x) struct Serializer : public tryn::ser::Serializer<x, #x>

namespace tryn::ser
{
	// Init serializer compile time map
	static constexpr auto SerializerListID = ZT_STRING_HASH("Serializer");
	ZT_INIT_CTV(SerializerListID);

	template <auto Tag = []{}>
	using SerializerList = typename utl::CTV::get_list<SerializerListID>;

	template <unsigned int UUID, unsigned N = 0>
	constexpr auto SerializerByUUID()
	{
		if constexpr (N < std::tuple_size_v<SerializerList<>>)
		{
			using NthSerializer = typename std::tuple_element_t<N, SerializerList<>>::Type;
			
			if constexpr (UUID == NthSerializer::UUID)
			{
				return NthSerializer{};
			}
			else
			{
				return SerializerByUUID<UUID, N + 1>();
			}
		}
	}

	template <unsigned int UUID>
	using ResolveSerializer = decltype(SerializerByUUID<UUID>());


	// Serializer base class
	template <typename T, utl::CTV::StaticString Name = "?">
	struct Serializer
	{
		static void Write(const class StreamWriter& streamWriter, const T& data, const bool binary = true, const std::string& name = "")
		{
			T::Write(streamWriter, data, binary, name);
		}
		static T Read(const class StreamReader& streamReader, const bool binary = false)
		{
			return T::Read(streamReader, binary);
		}
	private:
		using Name_t = decltype(utl::CTV::TextType<Name>);
		static constexpr Name_t nameFunc;
	public:
		using Type = T;
		static constexpr auto UUID = ZT_STRING_HASH(nameFunc());
	private:
		using Register_t = utl::CTV::UUIDMap_t<typename T::Serializer, Name, UUID, SerializerListID>;
	};

	template <typename T>
	struct TypeSerializer;

	template <typename T>
	concept HasSerializer = requires (const ser::StreamWriter& sw, const class StreamReader& sr, const T& data, const bool binary, const std::string& name)
	{
		std::derived_from<typename T::Serializer, Serializer<T>>;
		{ T::Serializer::Write(sw, data, binary, name) } -> std::same_as<void>;
		{ T::Serializer::Read(sr, binary) } -> std::same_as<T>;
	};

	template <typename T>
	concept HasRefReader = requires (const StreamReader& sr, T& data, const bool binary)
	{
		{T::Serializer::Read(data, sr, binary)} -> std::same_as<void>;
	};

	template <typename T>
	concept HasTypeRefReader = requires (const StreamReader& sr, T& data, const bool binary)
	{
		{TypeSerializer<T>::Read(data, sr, binary)} -> std::same_as<void>;
	};

	template <typename T>
	concept HasTypeSerializer = requires (const class StreamWriter& sw, const class StreamReader& sr, const T& data, const bool binary, const std::string& name)
	{
		{ TypeSerializer<T>::Write(sw, data, binary, name) } -> std::same_as<void>;
		{ TypeSerializer<T>::Read(sr, binary) } -> std::same_as<T>;
	};
}
