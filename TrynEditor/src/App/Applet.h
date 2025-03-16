#pragma once
#include <string>

namespace tryn::ed
{
	class Applet
	{
	public:
		virtual ~Applet() = default;
		virtual void Kernel() = 0;
		virtual void DoFrame();

		std::string name;
		uint16_t instanceID = 0;
	};
}
