#pragma once
#include <Core/src/ser/StreamIO.h>
#include <vector>

namespace tryn::ser
{
	template <Serializable T>
	struct TypeSerializer<std::unique_ptr<T>>
	{
		static void Write(const StreamWriter& streamWriter, const std::unique_ptr<T>& pData, const bool binary = true, const std::string& name = "")
		{
			streamWriter.GetStringStream() << "UP:";
			streamWriter.Serialize(*pData, binary, name);
		}
		template <typename Data = void>
		static std::unique_ptr<T> Read(const StreamReader& streamReader, const bool binary = true, const Data* pExtraData = nullptr)
		{
			streamReader.ExtractExpression("UP:");
			return std::make_unique<T>(streamReader.ReadSerialized<T>(binary, pExtraData));
		}

		template <typename Data = void>
		static void Read(std::unique_ptr<T>& data, const StreamReader& streamReader, const bool binary = true, const Data* pExtraData = nullptr)
		{
			streamReader.ExtractExpression("UP:");
			data.release();
			data = std::unique_ptr<T>(new T(std::move(streamReader.ReadSerialized<T>(binary, pExtraData))));
		}
	};

	template <Serializable T>
	struct TypeSerializer<std::vector<T>>
	{
		static void Write(const StreamWriter& streamWriter, const std::vector<T>& data, const bool binary = true, const std::string& name = "")
		{
			streamWriter.GetStringStream() << std::format("VEC:{:0>16}", data.size());

			int i = 0;
			for (auto& element : data)
			{
				streamWriter.Serialize(element, binary, std::to_string(i++) + ":");
			}
		}

		template <typename Data = void>
		static std::vector<T> Read(const StreamReader& streamReader, const bool binary = true, const Data* pExtraData = nullptr)
		{
			static std::vector<char> charBuffer;
			charBuffer.resize(sizeof(uint16_t) * 8, (char)0);
			streamReader.ExtractExpression("VEC:", charBuffer);

			auto numElements = std::stoi(charBuffer.data(), nullptr, 10);

			std::vector<T> newVector;
			newVector.reserve(numElements);

			for (int i = 0; i < numElements; i++)
			{
				newVector.emplace_back(streamReader.ReadSerialized<T>(binary, pExtraData));
			}

			return newVector;
		}

		template <typename Data = void>
		static void Read(std::vector<T>& data, const StreamReader& streamReader, const bool binary = true, const Data* pExtraData = nullptr)
		{
			static std::vector<char> charBuffer;
			charBuffer.resize(sizeof(uint16_t) * 8, (char)0);
			streamReader.ExtractExpression("VEC:", charBuffer);

			auto numElements = std::stoi(charBuffer.data(), nullptr, 10);

			data.reserve(numElements);

			for (int i = 0; i < numElements; i++)
			{
				data.emplace_back(streamReader.ReadSerialized<T>(binary, pExtraData));
			}
		}
	};

	template <>
	struct TypeSerializer<std::string>
	{
		static void Write(const StreamWriter& streamWriter, const std::string& data, const bool binary = true, const std::string& name = "");

		template <typename Data = void>
		static std::string Read(const StreamReader& streamReader, const bool binary = true, const Data* pExtraData = nullptr)
		{
			std::size_t numChars = 0;

			char sizeStr[(sizeof(std::size_t) * 2) + 1] = {0};
			streamReader.GetStringStream().read(sizeStr, sizeof(std::size_t) * 2);

			numChars = std::strtoul(sizeStr, nullptr, 16);

			std::string newString;
			newString.resize(numChars);

			streamReader.GetStringStream().read(newString.data(), numChars);

			return newString;
		}
	};
}
