#pragma once
#include <TrynEditor/src/Engine/Engine.h>

namespace edit
{
	class TrynEditorApp final : public app::App
	{
	public:

		TrynEditorApp(const std::shared_ptr<win::IWindow>&, const std::shared_ptr<gfx::IGraphics>&);
	};
}