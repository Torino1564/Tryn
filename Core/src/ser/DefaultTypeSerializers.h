#pragma once
#include <Core/src/ser/StreamIO.h>
#include <vector>
#include <unordered_map>
#include "ExtraDataPack.h"

namespace tryn::ser
{
	template <Serializable T>
	void Serialize(const StreamWriter& streamWriter, std::unique_ptr<T>* pData, const bool binary = true, const std::string& name = "")
	{
		streamWriter.Field(*pData, binary, name);
	}

	template <Serializable T>
	void Serialize(const StreamReader& streamReader, std::unique_ptr<T>* pData, const bool binary = true, const std::string& name = "")
	{
		auto pT = new T();
		streamReader.Field(pT, binary, name);
		pData->reset(pT);
	}

	template <Serializable T>
	static void Serialize(const StreamWriter& streamWriter, std::vector<T>* data, const bool binary = true, const std::string& name = "")
	{
		streamWriter.GetStringStream() << std::format("VEC:{:0>16}", data.size());

		int i = 0;
		for (auto& element : data)
		{
			streamWriter.Field(element, binary, std::to_string(i++) + ":");
		}
	}

	template <Serializable T>
	static void Serialize(const StreamReader& streamReader, std::vector<T>* pData, const bool binary = true, const std::string& name = "")
	{
		static std::vector<char> charBuffer;
		charBuffer.resize(sizeof(uint16_t) * 8, (char)0);
		streamReader.ExtractExpression("VEC:", charBuffer);

		auto numElements = std::stoi(charBuffer.data(), nullptr, 10);
		new(pData) std::vector<T>();
		pData.reserve(numElements);

		for (int i = 0; i < numElements; i++)
		{
			pData->emplace_back();
			streamReader.Field(pData->back(), binary);
		}
	}

	void Serialize(const StreamWriter& streamWriter, std::string* pData, const bool binary = true, const std::string& name = "");

	void Serialize(const StreamReader& sr, std::string* pData, const bool binary = true, const std::string& name = "");

	template <Serializable T, Serializable K>
	void Serialize(const StreamWriter& streamWriter, std::unordered_map<T, K>* data, const bool binary = true, const std::string& name = "")
	{
		streamWriter.GetStringStream() << std::format("UMAP:{:0>16}", data.size());

		unsigned int i = 0;
		for (const auto& [key, value] : *data)
		{
			streamWriter.Field(key, binary, std::to_string(i) + ":");
			streamWriter.Field(value, binary, std::to_string(i) + ":");
			i++;
		}
	}

	template <Serializable T, Serializable K>
	void Serialize(const StreamReader& streamReader, std::unordered_map<T, K>* pData, const bool binary = true, const std::string& name = "")
	{
		static std::vector<char> charBuffer;
		charBuffer.resize(sizeof(uint16_t) * 8, (char)0);
		streamReader.ExtractExpression("UMAP:", charBuffer);

		auto numElements = std::stoi(charBuffer.data(), nullptr, 10);

		pData->reserve(numElements);

		for (int i = 0; i < numElements; i++)
		{
			const auto it = pData->insert({ streamReader.Field<T>(binary), streamReader.Field<K>(binary) });
		}
	}
}
