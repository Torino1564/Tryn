#include <TrynCppUnitTest.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/win/Window.h>
#include <Core/src/gfx/Vertex.h>
#include <Core/src/gfx/dx11/Bindables/DX11Buffer.h>
#include <Core/src/app/EntryPoint.h>
#include <Core/src/gfx/Bindables/ConstantBuffer.h>

using namespace tryn;

tryn::app::App* tryn::app::CreateApp(int argc, char** argv)
{
	return nullptr;
}


namespace Gfx
{
	TEST_CLASS(D3D11BufferTests)
	{
	public:
		TEST_METHOD_INITIALIZE(Init)
		{
			app::BootCore();
			pWnd = std::make_unique<win::Window>(std::make_shared<win::WindowClass>(),L"Test window",spa::DimensionsI(300,300));
			pGfx = std::make_unique<gfx::dx11::Graphics>(pWnd->GetHandle(),pWnd->GetClientDimensions().width, pWnd->GetClientDimensions().height);
		}
		TEST_METHOD(VertexBufferTest)
		{
			gfx::VertexLayout vlayout;
			vlayout.AppendElement(gfx::VertexLayout::Position3D);
			vlayout.AppendElement(gfx::VertexLayout::Normal);
			vlayout.AppendElement(gfx::VertexLayout::UV);
			std::shared_ptr<gfx::VertexBuffer> pCPUVB = std::make_shared<gfx::VertexBuffer>(std::move(vlayout),10);
			pCPUVB->SetClean();
			gfx::dx11::DX11Buffer<gfx::BufferType::Vertex> vertexBuffer(*pGfx, pCPUVB);
			vertexBuffer.Bind();
			vertexBuffer.Bind(pGfx->GetContextInterface());
		}
		TEST_METHOD(PixelConstantBufferTest)
		{
			gfx::ConstantBufferLayout cbl;
			cbl.Append(gfx::ConstantBufferLayout::Node(gfx::ConstantBufferLayout::Type::Float3, "Color"));
			cbl.Append(gfx::ConstantBufferLayout::Node(gfx::ConstantBufferLayout::Type::Bool, "Switch"));
			cbl.Append(gfx::ConstantBufferLayout::Node(gfx::ConstantBufferLayout::Type::Float, "Slider"));
			cbl.Solidify();
			gfx::dx11::DX11Buffer<gfx::BufferType::PxConstant> pxConstantBuffer(*pGfx, std::move(cbl), 1);
			pxConstantBuffer.Bind();
			pxConstantBuffer.Bind(pGfx->GetContextInterface());
		}
		TEST_METHOD(VertexConstantBufferTest)
		{
			gfx::ConstantBufferLayout cbl;
			cbl.Append(gfx::ConstantBufferLayout::Node(gfx::ConstantBufferLayout::Type::Float3, "Color"));
			cbl.Append(gfx::ConstantBufferLayout::Node(gfx::ConstantBufferLayout::Type::Bool, "Switch"));
			cbl.Append(gfx::ConstantBufferLayout::Node(gfx::ConstantBufferLayout::Type::Float, "Slider"));
			cbl.Solidify();
			gfx::dx11::DX11Buffer<gfx::BufferType::VtxConstant> pxConstantBuffer(*pGfx, std::move(cbl), 1);
			pxConstantBuffer.Bind();
			pxConstantBuffer.Bind(pGfx->GetContextInterface());
		}
		std::unique_ptr<gfx::dx11::Graphics> pGfx;
		std::unique_ptr<win::Window> pWnd;
	};
}