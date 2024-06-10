#include <TrynCppUnitTest.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/win/Window.h>
#include <Core/src/app/Initialization.h>
#include <Core/src/gfx/Model/Model.h>

#include <Core/src/gfx/Render/Techniques/Flat.h>

using namespace tryn;

namespace Gfx
{
	TEST_CLASS(PointLightTests)
	{
	public:
		TEST_METHOD_INITIALIZE(Init)
		{
			app::BootCore();
			pWnd = std::make_unique<win::Window>(std::make_shared<win::WindowClass>(), L"Test window", spa::DimensionsI(300, 300));
			pGfx = std::make_unique<gfx::dx11::Graphics>(pWnd->GetHandle(), pWnd->GetClientDimensions().width, pWnd->GetClientDimensions().height);
		}
		TEST_METHOD(ModelTest)
		{
			auto model = gfx::Model::Make<gfx::Flat>(*pGfx, "resources/models/sphere.obj");
		}
		std::unique_ptr<gfx::dx11::Graphics> pGfx;
		std::unique_ptr<win::Window> pWnd;
	};
}