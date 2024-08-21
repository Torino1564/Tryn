#include <TrynEditor/src/Engine/Engine.h>

#include <TrynEditor/src/App/EditorApp.h>
#include <Core/src/app/EntryPoint.h>

using namespace tryn;

app::App* app::CreateApp(int argc, char** argv)
{
	auto window = ioc::Get().Resolve<win::IWindow>(win::IWindow::IocParams{ .size = spa::DimensionsI{.width = (1024), .height = (768) } });
	window->SetTitle(L"TrynGame");

	ioc::Get().Register<log::ISeverityLevelPolicy>([]
		{
			return std::make_shared<log::SeverityLevelPolicy>(
#ifdef _DEBUG
				log::Level::Warn
#else
				log::Level::Info
#endif
			);
		});

	auto gfx = ioc::Get().Resolve<gfx::IGraphics>(gfx::IGraphics::IocParams{ window->GetClientDimensions().width, window->GetClientDimensions().height, window->GetHandle() });

	auto pApp = new edit::TrynEditorApp(window, gfx);

	return pApp;
}