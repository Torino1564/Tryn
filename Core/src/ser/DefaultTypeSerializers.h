#pragma once
#include <Core/src/ser/StreamIO.h>
#include <vector>
#include <unordered_map>
#include "ExtraDataPack.h"

namespace tryn::ser
{
	template <Serializable T>
	void Serialize(StreamWriter& streamWriter, std::unique_ptr<T>* pData, const bool binary = true, const std::string& name = "")
	{
		streamWriter.Field(pData->get(), binary, name);
	}

	template <Serializable T>
	void Serialize(StreamReader& streamReader, std::unique_ptr<T>* pData, const bool binary = true, const std::string& name = "")
	{
		auto pT = new T();
		streamReader.Field(pT, binary, name);
		pData->reset(pT);
	}

	template <Serializable T>
	static void Serialize(StreamWriter& streamWriter, std::vector<T>* data, const bool binary = true, const std::string& name = "")
	{
		streamWriter.GetStringStream() << std::format("VEC:{:0>16}", data->size());

		int i = 0;
		for (auto& element : *data)
		{
			streamWriter.Field(&element, binary, std::to_string(i++) + ":");
		}
	}

	template <Serializable T>
	static void Serialize(StreamReader& streamReader, std::vector<T>* pData, const bool binary = true, const std::string& name = "")
	{
		static std::vector<char> charBuffer;
		charBuffer.resize(sizeof(uint16_t) * 8, (char)0);
		streamReader.ExtractExpression("VEC:", charBuffer);

		auto numElements = std::stoi(charBuffer.data(), nullptr, 10);
		new(pData) std::vector<T>();
		pData->reserve(numElements);

		for (int i = 0; i < numElements; i++)
		{
			pData->emplace_back();
			streamReader.Field(&pData->back(), binary);
		}
	}

	void Serialize(StreamWriter& streamWriter, std::string* pData, const bool binary = true, const std::string& name = "");

	void Serialize(StreamReader& sr, std::string* pData, const bool binary = true, const std::string& name = "");

	template <Serializable T, Serializable K>
	void Serialize(StreamWriter& streamWriter, std::unordered_map<T, K>* data, const bool binary = true, const std::string& name = "")
	{
		streamWriter.GetStringStream() << std::format("UMAP:{:0>16}", data->size());

		unsigned int i = 0;
		for (auto& [key, value] : *data)
		{
			streamWriter.Field(&key, binary, std::to_string(i) + ":");
			streamWriter.Field(&value, binary, std::to_string(i) + ":");
			i++;
		}
	}

	template <Serializable T, Serializable K>
	void Serialize(StreamReader& streamReader, std::unordered_map<T, K>* pData, const bool binary = true, const std::string& name = "")
	{
		static std::vector<char> charBuffer;
		charBuffer.resize(sizeof(uint16_t) * 8, (char)0);
		streamReader.ExtractExpression("UMAP:", charBuffer);

		auto numElements = std::stoi(charBuffer.data(), nullptr, 10);

		pData->reserve(numElements);

		for (int i = 0; i < numElements; i++)
		{
			T first;
			streamReader.Field(&first, binary);
			K second;
			streamReader.Field(&second, binary);
			const auto it = pData->insert({std::move(first), std::move(second)});
		}
	}
}
