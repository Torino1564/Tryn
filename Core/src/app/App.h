#pragma once

namespace tryn::gfx
{
	class IGraphics;
}

namespace tryn::win
{
	class IWindow;
}

namespace tryn::ecs
{
	class ECS;
}

namespace tryn::app
{
	class App
	{
	public:
		App(const std::shared_ptr<win::IWindow>& wnd, const std::shared_ptr<gfx::IGraphics>& gfx);
		virtual ~App();

		void Go();
		virtual void PreFrame();
		virtual void DoFrame();
		virtual void PostFrame();

		const gfx::IGraphics& Gfx() const;
		gfx::IGraphics& Gfx();
		ecs::ECS& ECS();
		const ecs::ECS& ECS() const;

	protected:
		std::unique_ptr<ecs::ECS> pEcs;
		std::shared_ptr<win::IWindow> wnd;
		std::shared_ptr<gfx::IGraphics> gfx;
		long double dt = 0;
	};


	App* CreateApp(int argc, char** argv);
}