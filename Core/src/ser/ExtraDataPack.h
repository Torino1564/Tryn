#pragma once
#include <vector>

#include "Core/src/utl/Assert.h"

namespace tryn::ser
{
	class ElementDataView
	{
	public:
		template <typename C>
		explicit ElementDataView(const C& data, const std::string_view name)
			: pElement(&data), name(name)
		{
			funcPtr = [](const void* pData, const void** ppFill){
				auto pCasted = static_cast<const C*>(pData);
				auto ppFillCasted = reinterpret_cast<const C**>(ppFill);
				*ppFillCasted = pCasted;
			};
		}

		void Get(const void** ppFill) const
		{
			funcPtr(pElement, ppFill);
		}

		void operator()(const void** ppFill) const
		{
			Get(ppFill);
		}

		const void* pElement;
		void (*funcPtr)(const void*, const void**) = nullptr;
		std::string name;
	};

	class ExtraDataPack
	{
	public:
		void AddElement(const ElementDataView& element)
		{
			elements.push_back(element);
		}

		const ElementDataView& Get(const std::string_view name) const
		{
			const auto it = std::ranges::find_if(elements, [&](const ElementDataView& view)
			{
				return !name.compare(name);
			});

			trynass(it != elements.end()).msg(L"Did not find the element with the specified name").ex();

			return *it;
		}

		std::vector<ElementDataView> elements;
	};


}
