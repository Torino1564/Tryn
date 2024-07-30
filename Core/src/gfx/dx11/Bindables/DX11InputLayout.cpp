#include "DX11InputLayout.h"
#include <Core/src/gfx/dx11/Bindables/DX11VertexShader.h>
#include <Core/src/gfx/dx11/Bindables/DX11Buffer.h>
#include <Core/src/gfx/dx11/GraphicsError.h>

namespace tryn::gfx::dx11
{
	DX11InputLayout::DX11InputLayout(const Graphics& gfx, IVertexBuffer& vb, IVertexShader& vs)
		:
		gfx(gfx)
	{
		trynass_msg(vb.GetAPI() == GraphicAPI::DX11, L"A DX11InputLayout was constructed with a non DX11 Vertex Buffer");
		trynass_msg(vs.GetAPI() == GraphicAPI::DX11, L"A DX11InputLayout was constructed with a non DX11 Vertex Shader");

		auto& dx11vb = static_cast<DX11VertexBuffer&>(vb);
		auto& dx11vs = static_cast<DX11VertexShader&>(vs);

		type = GraphicAPI::DX11;

		buffer.push_back(*reinterpret_cast<D3D11_INPUT_ELEMENT_DESC*>(vb.GetLayoutFromVB().data()));

		gfx.GetDevice().CreateInputLayout(buffer.data(), (UINT)vb.GetLayout().GetElementCount(), dx11vs.GetBlob()->GetBufferPointer(), dx11vs.GetBlob()->GetBufferSize(), &pLayout) >> chk;
	}
	
	DX11InputLayout::DX11InputLayout(const Graphics& gfx, VertexLayout& layout, IVertexShader& vs)
		:
		gfx(gfx)
	{
		trynass_msg(vs.GetAPI() == GraphicAPI::DX11, L"A DX11InputLayout was constructed with a non DX11 Vertex Shader");

		auto& dx11vs = static_cast<DX11VertexShader&>(vs);

		type = GraphicAPI::DX11;

		buffer = Graphics::GetSlottedLayout(layout, 0);

		gfx.GetDevice().CreateInputLayout(buffer.data(), (UINT)layout.GetElementCount(), dx11vs.GetBlob()->GetBufferPointer(), dx11vs.GetBlob()->GetBufferSize(), &pLayout) >> chk;
	}
	void DX11InputLayout::Bind()
	{
		gfx.GetContext().IASetInputLayout(pLayout.Get());
	}
	void DX11InputLayout::Bind(const IContext& context)
	{
		gfx.AssertContextCoherence(context);
		auto& dx11context = static_cast<const DX11Context*>(&context)->GetContext();

		dx11context.IASetInputLayout(pLayout.Get());
	}
}