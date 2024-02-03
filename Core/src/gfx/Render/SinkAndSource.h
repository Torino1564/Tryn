#pragma once
#include <tuple>
#include <memory>
#include <string>
#include <array>
#include <concepts>
#include <vector>

namespace tryn::gfx
{
	enum class Policy {
		ReadOnly,
		ReadWrite,
		WriteOnly
	};

	template<typename T>
	struct In
	{
		In(std::string name) : name(std::move(name)) {}
	private:
		Policy policy = Policy::ReadWrite;
		std::string name;
		using SysType = T;
	};

	class ISink
	{

	};

	template <typename... Dependencies>
	class Sink : public ISink
	{
	public:
		template <typename... Names>
		Sink(Names... names)
			requires (sizeof(Names...) == sizeof(Dependencies...))
		{

		}
	private:

		std::tuple<std::shared_ptr<Dependencies>...> dependencies;
		std::vector<std::string> names;
	};

	class ISource
	{

	};
}