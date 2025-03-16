#pragma once
#include <TrynEditor/src/Engine/Engine.h>
#include <Core/src/utl/ITypeRegister.h>
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
		void RegisterApplet()
		{
			appletRegister.RegisterType<T>();
		}

		void CreateApplet(utl::UUID_t uuid);

	private:

		// Applets
		utl::ITypeRegister<ed::Applet> appletRegister;
		std::vector<std::unique_ptr<ed::Applet>> pApplets;
		uint16_t appletIdCounter = 0;
	};
}
