#pragma once
#include <any>

namespace tryn::gfx
{
	class Attribute
	{
	public:
		template <typename T>
		static Attribute Make(const T& value)
		{
			Attribute newVal;

			newVal.value = std::make_any<T>(value);

			return newVal;
		}
		template <typename T>
		T Get() const
		{
			return std::any_cast<T>(value);
		}
	private:
		std::any value;
	};
}
