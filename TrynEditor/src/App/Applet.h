#pragma once

namespace tryn::ed
{
	class Applet
	{
	public:
		virtual ~Applet() = default;
		virtual void DoFrame() = 0;
	};
}