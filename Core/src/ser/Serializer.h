#pragma once
#include <sstream>
#include <fstream>
#include <iomanip>
#include <memory>

namespace tryn::ser
{
	// Base serializer class. Every Serializer implementation derives from this
	class SerializerBase
	{
		// Put here base class serializer stuff
	};

	// Actual serializer base class
	template <typename T>
	class Serializer : SerializerBase
	{
	public:
		static void Write(class StreamWriter& streamWriter, const T& data)
		{
			T::Write(streamWriter, data);
		}
	};

	template <typename T>
	struct TypeSerializer;

	template <typename T>
	concept HasSerializer = requires { std::derived_from<typename T::Serializer, Serializer<T>>; };

	template<class First, std::size_t = sizeof(First)>
	using first_t = First;

	template<class T>
	struct is_complete_type : std::false_type {};

	template<class T>
	struct is_complete_type<first_t<T>> : std::true_type {};

	template <typename T>
	concept HasTypeSerialized = is_complete_type<TypeSerializer<T>>::value;

	class StreamWriter
	{
	public:
		explicit StreamWriter(std::ostringstream& oss)
			: oss(oss) {}

		template <typename T>
		requires HasSerializer<T> || std::is_trivially_copyable_v<T> || HasTypeSerialized<T>
		void Serialize(const T& data, const bool binary = true, const std::string_view name = "")
		{
			if constexpr (std::is_trivially_copyable_v<T>)
			{
				if (binary)
				{
					const auto pData = (char*)(&data);
					oss.write(pData, sizeof(T));
				}
				return;
			}
			else if constexpr (HasTypeSerialized<T>)
			{
				return TypeSerializer<T>::Write(*this, data);
			}
			else
			{
				return T::Serializer::Write(*this, data);
			}
		}

		auto& GetStringStream() const
		{
			return oss;
		}
	private:
		std::ostringstream& oss;
	};

	class Dummy
	{
		
	};

	template <typename T>
	concept UniquePtr = requires
	{
		std::is_same_v<std::unique_ptr<typename T::element_type>, T>;
	};

	template <>
	struct TypeSerializer<int>
	{
		static void Write(const StreamWriter& streamWriter, const int& data)
		{
			streamWriter.GetStringStream().write((char*)data, sizeof(int));
		}
	};
}
