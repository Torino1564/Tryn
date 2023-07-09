#include "TestApp.h"
#include <Core/src/gfx/Bindables/BindableBase.h>

TestApp::TestApp(std::shared_ptr<win::IWindow> wnd_, std::shared_ptr<gfx::IGraphics> gfx_)
{
	wnd = wnd_;
	gfx = gfx_;

	ent::Entity box;
	auto& model = ent::Cube::GetInstance();

	model.MakeBindables(Gfx());

	// Index Buffer
	auto indexBuffer = model.GetBindables().back().get();
	
	// VertexBuffer
}

void TestApp::DoFrame()
{

}
