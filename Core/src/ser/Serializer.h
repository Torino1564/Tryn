#pragma once
#include <string>

namespace tryn::ser
{
	class StreamIO;
	class StreamWriter;
	class StreamReader;

	template <typename T>
	concept HasGenericSerializer = requires (StreamIO& io, T* data, const bool binary, const std::string& name)
	{
		{ Serialize(io, data, binary, name) } -> std::same_as<void>;
	};

	template <typename T>
	concept HasTwoWaySerializer = requires (StreamWriter& sw, StreamReader& sr, T * data, const bool binary, const std::string & name)
	{
		{ Serialize(sw, data, binary, name) } -> std::same_as<void>;
		{ Serialize(sr, data, binary, name) } -> std::same_as<void>;
	};

	template <typename T>
	concept HasFunctionSerializer = HasGenericSerializer<T> || HasTwoWaySerializer<T>;

	template <typename T>
	concept Serializable = std::is_trivially_copyable_v<T> || HasFunctionSerializer<T>;
}
