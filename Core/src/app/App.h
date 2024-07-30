#pragma once
#include <Core/src/app/Globals.h>

namespace tryn::gfx
{
	class IGraphics;
}

namespace tryn::win
{
	class IWindow;
}

namespace tryn::app
{
	class App
	{
	public:
		App() = default;
		App(std::shared_ptr<win::IWindow> wnd, std::shared_ptr<gfx::IGraphics> gfx);
		virtual ~App() {};

		void Go();
		virtual void PreFrame();
		virtual void DoFrame();
		virtual void PostFrame();

		const gfx::IGraphics& Gfx() const;
		gfx::IGraphics& Gfx();

	protected:
		std::shared_ptr<win::IWindow> wnd;
		std::shared_ptr<gfx::IGraphics> gfx;
		long double dt = 0;
	};


	App* CreateApp(int argc, char** argv);
}