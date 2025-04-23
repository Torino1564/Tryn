#pragma once
#include <any>

namespace tryn::gfx
{
	class Attribute
	{
	public:
		template <typename T>
		static Attribute Make(const std::string& name, T&& value)
		{
			Attribute newVal;

			newVal.name = name;
			newVal.value = std::make_any<T>(std::forward<T&&>(value));

			return newVal;
		}
		template <typename T>
		T Get()
		{
			return std::any_cast<T>(value);
		}
	private:
		std::string name;
		std::any value;
	};
}
