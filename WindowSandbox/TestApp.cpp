#include "TestApp.h"
#include <Core/src/gfx/Bindables/BindableBase.h>

TestApp::TestApp(std::shared_ptr<win::IWindow> wnd_, std::shared_ptr<gfx::IGraphics> gfx_)
{
	wnd = wnd_;
	gfx = gfx_;

	std::unique_ptr<gfx::IBindable> vertexBuffer;
	auto pVertexBuffer = vertexBuffer.get();

	Gfx().CreateVertexBuffer(  , &pVertexBuffer );
}

void TestApp::DoFrame()
{

}
 