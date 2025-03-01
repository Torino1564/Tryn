#pragma once

#define ZT_DEFINE_SERIALIZER(x) struct Serializer : public tryn::ser::Serializer<x>

#define ZT_SERIALIZER_HELPER(x) struct Serializer : public tryn::ser::Serializer<x> {\
	static void Write(const tryn::ser::StreamWriter& streamWriter, const x& data, const bool binary = true, const std::string& name = "")\
	{\
		\
	}\
	static x Read(const tryn::ser::StreamReader& streamReader, const bool binary = true, class ExtraDataPack* pExtraData = nullptr)\
	{\
		\
	}\
	static void Read(x& data, const tryn::ser::StreamReader& streamReader, const bool binary = true, class ExtraDataPack* pExtraData = nullptr)\
	{\
		\
	}\
};

namespace tryn::ser
{
	// Serializer base class
	template <typename T>
	struct Serializer
	{
		static void Write(const class StreamWriter& streamWriter, const T& data, const bool binary = true, const std::string& name = "")
		{
			T::Write(streamWriter, data, binary, name);
		}
		static T Read(const class StreamReader& streamReader, const bool binary = false, class ExtraDataPack* pExtraData = nullptr)
		{
			return T::Read(streamReader, binary, pExtraData);
		}

		using Type = T;
	};

	template <typename T>
	struct TypeSerializer;

	template <typename T>
	concept HasSerializer = requires (const ser::StreamWriter& sw, const class StreamReader& sr, const T& data, const bool binary, const std::string& name, class ExtraDataPack* pExtraData)
	{
		std::derived_from<typename T::Serializer, Serializer<T>>;
		{ T::Serializer::Write(sw, data, binary, name) } -> std::same_as<void>;
		{ T::Serializer::Read(sr, binary, pExtraData) } -> std::same_as<T>;
	};

	template <typename T>
	concept HasRefReader = requires (const StreamReader& sr, T& data, const bool binary, class ExtraDataPack* pExtraData)
	{
		{T::Serializer::Read(data, sr, binary, pExtraData)} -> std::same_as<void>;
	};

	template <typename T>
	concept HasTypeRefReader = requires (const StreamReader& sr, T& data, const bool binary, class ExtraDataPack* pExtraData)
	{
		{TypeSerializer<T>::Read(data, sr, binary, pExtraData)} -> std::same_as<void>;
	};

	template <typename T>
	concept HasTypeSerializer = requires (const class StreamWriter& sw, const class StreamReader& sr, const T& data, const bool binary, const std::string& name, class ExtraDataPack* pExtraData)
	{
		{ TypeSerializer<T>::Write(sw, data, binary, name) } -> std::same_as<void>;
		{ TypeSerializer<T>::Read(sr, binary, pExtraData) } -> std::same_as<T>;
	};

	template <typename T>
	concept HasFunctionSerializer = requires (const class StreamWriter& sw, const class StreamReader& sr, const T& data, T& data_, const bool binary, const std::string& name, class ExtraDataPack* pExtraData)
	{
		{ SerializeWrite(sw, data, binary, name) } -> std::same_as<void>;
		{ SerializeRead(sr, data_, binary, pExtraData) } -> std::same_as<void>;
	};

	template <typename T>
	concept Serializable = HasSerializer<T> || HasTypeSerializer<T> || std::is_trivially_copyable_v<T> || HasFunctionSerializer<T>;
}
