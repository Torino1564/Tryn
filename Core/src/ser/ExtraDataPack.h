#pragma once
#include <vector>

#include "Core/src/utl/Assert.h"
#include <variant>
#include <Core/src/log/Log.h>

namespace tryn::ser
{
	class ElementDataView
	{
	public:
		template <typename C>
		explicit ElementDataView(C& data, const std::string_view name)
		{
			if constexpr(std::is_const_v<C>)
			{
				dataStruct.emplace<ConstData>(nullptr, nullptr);
				isConst = true;
				auto& info = std::get<ConstData>(dataStruct);
				info.funcPtr = [](const void* pData, const void*& pFillRef) {
					auto pCasted = reinterpret_cast<const C*>(pData);
					auto& pFillCastedRef = reinterpret_cast<const C*&>(pFillRef);
					pFillCastedRef = pCasted;
					};
				info.pElement = &data;
			}
			else
			{
				dataStruct.emplace<Data>(nullptr, nullptr);
				isConst = false;
				auto& info = std::get<Data>(dataStruct);
				info.funcPtr = [](void* pData, void*& pFillRef) {
					auto pCasted = reinterpret_cast<C*>(pData);
					auto& pFillCastedRef = reinterpret_cast<C*&>(pFillRef);
					pFillCastedRef = pCasted;
					};
				info.pElement = &data;
			}
			this->name = name;
		}

		void Get(void*& pFillRef) const
		{
			if (isConst)
			{
				pFillRef = nullptr;
				trylog.warn(L"Invalid pointer qualifier. Should pass a const pointer to retrieve the variable.");
				return;
			}
			else
			{
				const auto& [pElement, funcPtr] = std::get<Data>(dataStruct);
				funcPtr(pElement, pFillRef);
			}
		}

		void Get(const void*& pFillRef) const
		{
			auto& data = std::get<ConstData>(dataStruct);
			data.funcPtr(data.pElement, pFillRef);
		}

		struct Data
		{
			void* pElement;
			void (*funcPtr)(void*, void*&) = nullptr;
		};

		struct ConstData
		{
			const void* pElement;
			void (*funcPtr)(const void*, const void*&) = nullptr;
		};

		std::variant<Data, ConstData> dataStruct;

		bool isConst = false;
		std::string name;
	};

	class ExtraDataPack
	{
	public:
		void AddElement(const ElementDataView& element)
		{
			elements.push_back(element);
		}

		ElementDataView Get(const std::string_view name) const
		{
			const auto it = std::ranges::find_if(elements, [&](const ElementDataView& view)
			{
				return name == view.name;
			});

			trynass(it != elements.end()).msg(L"Did not find the element with the specified name").ex();

			return *it;
		}

		std::vector<ElementDataView> elements;
	};


}
