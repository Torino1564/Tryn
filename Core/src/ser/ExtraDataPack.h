#pragma once
#include <vector>

#include "Core/src/utl/Assert.h"

namespace tryn::ser
{
	class ElementDataView
	{
	public:
		template <typename C>
		explicit ElementDataView(C& data, const std::string_view name)
			: pElement(&data), name(name)
		{
			funcPtr = [](void* pData, void** ppFill){
				auto pCasted = reinterpret_cast<C>(pData);
				auto ppFillCasted = reinterpret_cast<C*>(ppFill);
				*ppFillCasted = pCasted;
			};
		}

		void Get(void** ppFill) const
		{
			funcPtr(pElement, ppFill);
		}

		void operator()(void** ppFill) const
		{
			Get(ppFill);
		}

		void* pElement;
		void (*funcPtr)(void*, void**) = nullptr;
		std::string name;
	};

	class ConstElementDataView
	{
	public:
		template <typename C>
		explicit ConstElementDataView(C& data, const std::string_view name)
			: pElement(&data), name(name)
		{
			funcPtr = [](const void* pData, const void** ppFill) {
				auto pCasted = reinterpret_cast<const C>(pData);
				auto ppFillCasted = reinterpret_cast<const C*>(ppFill);
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
		void AddElement(const ConstElementDataView& element)
		{
			elements.push_back(element);
		}

		const void Get(const std::string_view name, ) const
		{
			const auto it = std::ranges::find_if(elements, [&](const ElementDataView& view)
			{
				return name == view.name;
			});

			trynass(it != elements.end()).msg(L"Did not find the element with the specified name").ex();

			return *it;
		}

		std::vector<ElementDataView> elements;
		std::vector<ConstElementDataView> constElements;
	};


}
