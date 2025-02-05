#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <concepts>
#include <array>
#include "Core/src/utl/Assert.h"

namespace tryn::scr
{
	using MaxScriptVariableSize = std::array<std::byte, 24>;

	template <typename T>
	concept FitsInScriptVariable = requires
	{
		sizeof(T) <= sizeof(MaxScriptVariableSize);
	};

	class ScriptVariable
	{
	public:

		template <FitsInScriptVariable T>
		static ScriptVariable Make()
		{
			return Make<T>(std::move(T()));
		}

		template <FitsInScriptVariable T>
		static ScriptVariable Make(T&& rhs)
		{
			ScriptVariable retval;
			retval.rawData = {};
			*static_cast<T*>(retval.GetData()) = std::forward<T>(rhs);
			retval.pDeleter = [](void* pData_)
				{
					const auto pData = static_cast<T*>(pData_);
					pData->~T();
				};
			return retval;
		}
		~ScriptVariable();
		void* GetData();

	private:
		MaxScriptVariableSize rawData = {};
		void(*pDeleter)(void*) = nullptr;
	};

	static constexpr auto test = sizeof(ScriptVariable);

	class ScriptVariableBag
	{
	public:
		template <FitsInScriptVariable T>
		T& Get(const std::string_view str)
		{
			const auto it = map.find(str.data());
			trynass(it != map.end()).msg(L"The requested variable does not exist!");
			return *static_cast<T*>(bag[it->second].GetData());
		}
		template <FitsInScriptVariable T>
		void Insert(const std::string_view name, T&& rhs)
		{
			const auto it = map.find(name.data());
			trynass(it == map.end()).msg(L"The requested variable already exist!");
			map.insert({ name.data(), currentPosition });

			if (currentPosition >= bag.size())
				bag.resize(currentPosition + 1);
			
			bag[currentPosition++] = ScriptVariable::Make<T>(std::forward<T>(rhs));
		}
		template <FitsInScriptVariable T>
		requires std::is_default_constructible_v<T>
		void Insert(const std::string_view name)
		{
			Insert(name, std::move(T()));
		}
	private:
		uint16_t currentPosition = 0;
		std::vector<ScriptVariable> bag;
		std::unordered_map<std::string, uint16_t> map;
	};
}
