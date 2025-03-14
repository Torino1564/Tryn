#pragma once
#include <TrynEditor/src/Engine/Engine.h>

#include "Applet.h"

namespace tryn::ed
{
	static constexpr auto MAX_NAME_SIZE = 128;

	class TrynEditorApp final : public app::App
	{
	public:
		TrynEditorApp(const std::shared_ptr<win::IWindow>&, const std::shared_ptr<gfx::IGraphics>&);
		void DoFrame() override;

		template <typename T>
			requires std::derived_from<T, Applet>
		void RegisterApplet()
		{
			pApplets.push_back(std::move(std::make_unique<T>()));
		}

	private:
		std::vector<std::unique_ptr<ed::Applet>> pApplets;
	};
}
