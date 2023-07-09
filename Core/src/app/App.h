#pragma once
#include <memory>
#include <Core/src/gfx/IGraphics.h>
#include <Core/src/win/IWindow.h>

namespace tryn::app
{
	class App
	{
	public:
		App() = default;
		App(std::shared_ptr<win::IWindow> wnd , std::shared_ptr<gfx::IGraphics> gfx);
		virtual ~App() {};

		void Go();
		virtual void PreFrame();
		virtual void DoFrame();
		virtual void PostFrame();

		gfx::IGraphics& Gfx();
	protected:
		std::shared_ptr<win::IWindow> wnd;
		std::shared_ptr<gfx::IGraphics> gfx;
	};
}