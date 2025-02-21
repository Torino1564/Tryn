#pragma once
#include <Core/src/utl/Exception.h>
#include "Serializer.h"
#include <span>
#include <Core/src/ser/ExtraDataPack.h>

class std::ostringstream;
class std::istringstream;

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
			if constexpr (HasFunctionSerializer<T>)
			{
				return SerializeWrite(*this, data, binary, name);
			}
			if constexpr (std::is_trivially_copyable_v<T>)
			{
				if (binary)
				{
					const auto pData = (char*)(&data);
					WriteBinary(pData, sizeof(T));
				}
			}
		}

		auto& GetStringStream() const
		{
			return oss;
		}
	private:
		std::ostringstream& oss;
		void WriteBinary(const char* pData, unsigned size) const;
	};

	class StreamReader
	{
	public:
		explicit StreamReader(std::istringstream& iss) : iss(iss){}

		template <typename T>
		requires Serializable<T> && !HasFunctionSerializer<T>
		T ReadSerialized(const bool binary = true, const ExtraDataPack* pExtraData = nullptr) const
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

		template <typename T>
		requires Serializable<T>
		void ReadSerialized(T& data, const bool binary = true, const ExtraDataPack* pExtraData = nullptr) const
		{
			if constexpr (HasSerializer<T>)
			{
				return T::Serializer::Read(data, *this, binary, pExtraData);
			}
			else if constexpr (HasTypeSerializer<T>)
			{
				return TypeSerializer<T>::Read(data, *this, binary, pExtraData);
			}
			else if constexpr (HasFunctionSerializer<T>)
			{
				return SerializeRead(*this, data, binary, pExtraData);
			}
			else
			{
				if (binary)
				{
					iss.read((char*)&data, sizeof(T));
				}
				else
				{
					return;
				}
			}
		}

		void ExtractExpression(const std::string& expression, std::span<char> pExtraChars = {}) const;

		std::istringstream& GetStringStream() const;

	private:

		void ReadBinary(char* pData, unsigned size) const;
		std::istringstream& iss;
	};
}

#include "DefaultTypeSerializers.h"
