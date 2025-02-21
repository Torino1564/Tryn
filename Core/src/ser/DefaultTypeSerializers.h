#pragma once
#include <Core/src/ser/StreamIO.h>
#include <vector>
#include <unordered_map>
#include "ExtraDataPack.h"

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
		static std::unique_ptr<T> Read(const StreamReader& streamReader, const bool binary = true, const ExtraDataPack* pExtraData = nullptr)
		{
			std::unique_ptr<T> uptr;
			
			Read(uptr, streamReader, binary, pExtraData);

			return uptr;
		}

		static void Read(std::unique_ptr<T>& data, const StreamReader& streamReader, const bool binary = true, const ExtraDataPack* pExtraData = nullptr)
		{
			streamReader.ExtractExpression("UP:");
			data.release();
			if constexpr (HasFunctionSerializer<T>)
			{
				data = std::make_unique<T>();
				streamReader.ReadSerialized(*data, binary, pExtraData);
			}
			else
			{
				data = std::unique_ptr<T>(new T(std::move(streamReader.ReadSerialized<T>(binary, pExtraData))));
			}
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

		static std::vector<T> Read(const StreamReader& streamReader, const bool binary = true, const ExtraDataPack* pExtraData = nullptr)
		{
			std::vector<T> newVector;

			Read(newVector, streamReader, binary, pExtraData);

			return newVector;
		}

		static void Read(std::vector<T>& data, const StreamReader& streamReader, const bool binary = true, const ExtraDataPack* pExtraData = nullptr)
		{
			static std::vector<char> charBuffer;
			charBuffer.resize(sizeof(uint16_t) * 8, (char)0);
			streamReader.ExtractExpression("VEC:", charBuffer);

			auto numElements = std::stoi(charBuffer.data(), nullptr, 10);

			data.reserve(numElements);

			for (int i = 0; i < numElements; i++)
			{
				if constexpr (HasFunctionSerializer<T>)
				{
					data.emplace_back();
					streamReader.ReadSerialized(data.back(), binary, pExtraData);
				}
				else
				{
					data.emplace_back(streamReader.ReadSerialized<T>(binary, pExtraData));
				}
			}
		}
	};

	template <>
	struct TypeSerializer<std::string>
	{
		static void Write(const StreamWriter& streamWriter, const std::string& data, const bool binary = true, const std::string& name = "");

		static std::string Read(const StreamReader& streamReader, const bool binary = true, const ExtraDataPack* pExtraData = nullptr);

		static void Read(std::string& data, const StreamReader& sr, const bool binary = true, const ExtraDataPack* pExtraData = nullptr);
	};

	template <Serializable T, Serializable K>
	struct TypeSerializer<std::unordered_map<T, K>>
	{
		static void Write(const StreamWriter& streamWriter, const std::unordered_map<T, K>& data, const bool binary = true, const std::string& name = "")
		{
			streamWriter.GetStringStream() << std::format("UMAP:{:0>16}", data.size());

			unsigned int i = 0;
			for (const auto& [key, value] : data)
			{
				streamWriter.Serialize(key, binary, std::to_string(i) + ":");
				streamWriter.Serialize(value, binary, std::to_string(i) + ":");
				i++;
			}
		}

		static std::unordered_map<T, K> Read(const StreamReader& streamReader, const bool binary = true, const ExtraDataPack* pExtraData = nullptr)
		{

			std::unordered_map<T, K> newUMap;
			
			Read(newUMap, streamReader, binary, pExtraData);

			return newUMap;
		}

		static void Read(std::unordered_map<T, K>& data, const StreamReader& streamReader, const bool binary = true, const ExtraDataPack* pExtraData = nullptr)
		{
			static std::vector<char> charBuffer;
			charBuffer.resize(sizeof(uint16_t) * 8, (char)0);
			streamReader.ExtractExpression("UMAP:", charBuffer);

			auto numElements = std::stoi(charBuffer.data(), nullptr, 10);

			data.reserve(numElements);

			for (int i = 0; i < numElements; i++)
			{
				if constexpr (HasFunctionSerializer<T> && HasFunctionSerializer<K>)
				{
					const auto it = data.insert({T{}, K{}});
					streamReader.ReadSerialized(it.first, binary, pExtraData);
					streamReader.ReadSerialized(it.second, binary, pExtraData);
				}
				else if constexpr (HasFunctionSerializer<T>)
				{
					const auto it = data.insert({ T{}, streamReader.ReadSerialized<K>(binary, pExtraData) });
					streamReader.ReadSerialized(it.first, binary, pExtraData);
				}
				else if constexpr (HasFunctionSerializer<K>)
				{
					const auto it = data.insert({ streamReader.ReadSerialized<T>(binary, pExtraData), K{}});
					streamReader.ReadSerialized(it.second, binary, pExtraData);
				}
				else
				{
					const auto it = data.insert({ streamReader.ReadSerialized<T>(binary, pExtraData), streamReader.ReadSerialized<K>(binary, pExtraData) });
				}
			}
		}
	};
}
