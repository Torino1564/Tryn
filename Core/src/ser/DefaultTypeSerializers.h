#pragma once
#include <Core/src/ser/StreamIO.h>

namespace tryn::ser
{
		template <typename T>
	struct TypeSerializer<std::unique_ptr<T>>
	{
		static void Write(const StreamWriter& streamWriter, const std::unique_ptr<T>& pData, const bool binary = true, const std::string& name = "")
		{
			streamWriter.GetStringStream() << "UP:";
			streamWriter.Serialize(*pData, binary, name);
		}

		static std::unique_ptr<T> Read(const StreamReader& streamReader, const bool binary = true)
		{
			streamReader.ExtractExpression("UP:");
			return std::make_unique<T>(streamReader.ReadSerialized<T>(binary));
		}

		static void Read(std::unique_ptr<T>& data, const StreamReader& streamReader, const bool binary = true)
		{
			streamReader.ExtractExpression("UP:");
			data.release();
			data = std::make_unique<T>(streamReader.ReadSerialized<T>(binary));
		}
	};

	template <typename T>
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

		static std::vector<T> Read(const StreamReader& streamReader, const bool binary = true)
		{
			static std::vector<char> charBuffer;
			charBuffer.resize(sizeof(uint16_t) * 8, (char)0);
			streamReader.ExtractExpression("VEC:", charBuffer);

			auto numElements = std::stoi(charBuffer.data(), nullptr, 10);

			std::vector<T> newVector;
			newVector.reserve(numElements);

			for (int i = 0; i < numElements; i++)
			{
				newVector.emplace_back(streamReader.ReadSerialized<T>(binary));
			}

			return newVector;
		}

		static void Read(std::vector<T>& data, const StreamReader& streamReader, const bool binary = true)
		{
			static std::vector<char> charBuffer;
			charBuffer.resize(sizeof(uint16_t) * 8, (char)0);
			streamReader.ExtractExpression("VEC:", charBuffer);

			auto numElements = std::stoi(charBuffer.data(), nullptr, 10);

			data.reserve(numElements);

			for (int i = 0; i < numElements; i++)
			{
				data.emplace_back(streamReader.ReadSerialized<T>(binary));
			}
		}
	};

	template <>
	struct TypeSerializer<std::string>
	{
		static void Write(const StreamWriter& streamWriter, const std::string& data, const bool binary = true, const std::string& name = "")
		{
			auto stringOut = std::format("STR:{:0>16}", data.size());
			streamWriter.GetStringStream().write(stringOut.data(), stringOut.size());
			streamWriter.GetStringStream().write(data.data(), data.size());
		}

		static std::string Read(const StreamReader& streamReader, const bool binary = true)
		{
			static std::vector<char> charBuffer {sizeof(uint16_t) * 8};
			streamReader.ExtractExpression("STR:", charBuffer);

			auto numChars = std::stoi(charBuffer.data(), nullptr, 10);

			std::span<char> stringView {(char*)nullptr, (std::size_t)numChars};
			streamReader.ExtractExpression("", stringView);

			return std::string{stringView.data(), stringView.size()};
		}
	};
}
