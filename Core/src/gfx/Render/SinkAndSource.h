#pragma once
#include <tuple>
#include <memory>
#include <string>
#include <array>

namespace tryn::gfx
{
	class ISink
	{

	};

	template <typename... Dependencies>
	class Sink : public ISink
	{
	public:
		Sink(std::array<std::string, sizeof(Dependencies)...> parameters);
	private:
		std::tuple<std::shared_ptr<Dependencies>...> dependencies;
	};

	class ISource
	{

	};
}