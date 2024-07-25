#pragma once
#include <sstream>
#include <Core/src/utl/Exception.h>
#include "Serializer.h"

namespace tryn::ser
{
	ZT_EX_DEF(StreamIOException);
	
	class StreamWriter
	{
	public:
		explicit StreamWriter(std::ostringstream& oss)
			: oss(oss) {}

		template <typename T>
		requires Serializable<T>
		void Serialize(const T& data,const bool binary = true, const std::string& name = "") const
		{
			if constexpr (HasSerializer<T>)
			{
				return T::Serializer::Write(*this, data, binary, name);
			}
			if constexpr (HasTypeSerializer<T>)
			{
				return TypeSerializer<T>::Write(*this, data, binary, name);
			}
			if constexpr (std::is_trivially_copyable_v<T>)
			{
				if (binary)
				{
					const auto pData = (char*)(&data);
					oss.write(pData, sizeof(T));
				}
			}
		}

		auto& GetStringStream() const
		{
			return oss;
		}
	private:
		std::ostringstream& oss;
	};

	class StreamReader
	{
	public:
		explicit StreamReader(std::istringstream& iss) : iss(iss){}

		template <typename T, typename Data = void>
		requires Serializable<T>
		T ReadSerialized(const bool binary = true, const Data* pExtraData = nullptr) const
		{
			if constexpr (HasSerializer<T>)
			{
				return T::Serializer::Read(*this, binary, pExtraData);
			}
			else if constexpr (HasTypeSerializer<T>)
			{
				return TypeSerializer<T>::Read(*this, binary, pExtraData);
			}
			else {
				if (binary)
				{
					T newElement = {};
					iss.read((char*)&newElement, sizeof(T));
					return newElement;
				}
				else
				{
					return T{};
				}
			}
		}

		template <typename T, typename Data = void>
		requires (HasSerializer<T> && HasRefReader<T>) || std::is_trivially_copyable_v<T> || (HasTypeSerializer<T> && HasTypeRefReader<T>) 
		void ReadSerialized(T& data, const bool binary = true, const Data* pExtraData = nullptr) const
		{
			if constexpr (HasSerializer<T>)
			{
				return T::Serializer::Read(data, *this, binary, pExtraData);
			}
			else if constexpr (HasTypeSerializer<T>)
			{
				return TypeSerializer<T>::Read(data, *this, binary, pExtraData);
			}
			else
			{
				if (binary)
				{
					unsigned int position = iss.tellg();
					std::string_view view {iss.str().data() + iss.str().size() - position, position};
					iss.read((char*)&data, sizeof(T));
				}
				else
				{
					
				}
			}
		}

		void ExtractExpression(const std::string& expression, std::span<char> pExtraChars = {}) const
		{
			static std::string charBuffer;

			if (expression.size() > charBuffer.size())
			{
				charBuffer.resize(expression.size());
			}

			std::memset(charBuffer.data(), '0', charBuffer.size());

			iss.read(charBuffer.data(), expression.size());

			if (expression != charBuffer)
				throw StreamIOException{"Failed to parse the requested expression from file"};

			if (pExtraChars.data() != nullptr)
			{
				iss.read(pExtraChars.data(), pExtraChars.size());
			}
		}

		auto& GetStringStream() const
		{
			return iss;
		}
	private:
		std::istringstream& iss;
	};
}

#include "DefaultTypeSerializers.h"
