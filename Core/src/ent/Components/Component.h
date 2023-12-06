#pragma once
#include <string>
#include "ComponentCodex.h"
#include <optional>
#include <Core/third/imgui/imgui.h>

namespace tryn::ent
{
	class Component
	{
	public:
		static constexpr ComponentType GetCUID()
		{
			return ComponentType::Invalid;
		}
		virtual ~Component() = default;
		virtual void OnCreate() {}
		virtual void OnUpdate(double dt = 0) {}
		virtual void OnDestroy() {}
		virtual void Controls()
		{
			ImGui::TreeNode(name->c_str());
		}
		std::string_view GetName();
	private:
		std::optional<std::string> name;
		int entityUID;
	};
}