#include "TrynPCH.h"
#include "Gfx.h"
#include <Core/src/ioc/Container.h>
#include "ImguiManager.h"
#include "dx11/Dx11Graphics.h"

namespace tryn::gfx
{
	void Boot()
	{
		ioc::Get().Register<IGraphics>([](IGraphics::IocParams args) {
			return std::make_shared<dx11::Graphics>(
				args.hWnd,
				args.width.value_or(1280),
				args.height.value_or(720)
			);
			});

		static ImguiManager imguiManager;
	}
}