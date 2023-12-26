#pragma once
#include <Core/src/utl/Timer.h>
#include <imgui.h>

#define PROFILE_SCOPE(name) tryn::utl::Timer ScopedTimer##__LINE__(name, tryn::gfx::ImGuiProfilerCallback::Get())

namespace tryn::gfx
{
	class ImGuiProfilerCallback
	{
	public:
		void operator()(const char* name, long double duration)
		{
			ImGui::Begin("Profile Timers");
			ImGui::Text("%f %s", duration, name);
			ImGui::End();
		}
		static ImGuiProfilerCallback& Get()
		{
			static ImGuiProfilerCallback singleton;
			return singleton;
		}
	};
}