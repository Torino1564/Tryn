#include "DX11InputLayout.h"
#include <Core/src/gfx/dx11/Bindables/DX11VertexBuffer.h>
#include <Core/src/gfx/dx11/Bindables/DX11VertexShader.h>
#include <Core/src/gfx/dx11/Bindables/DX11PolyVBuffer.h>
#include <Core/src/gfx/dx11/GraphicsError.h>

namespace tryn::gfx::dx11
{
	DX11InputLayout::DX11InputLayout(Graphics& gfx, IVertexBuffer& vb, IVertexShader& vs)
		:
		gfx(gfx)
	{
		trynass_msg(vb.GetAPI() == GraphicAPI::DX11, L"A DX11InputLayout was constructed with a non DX11 Vertex Buffer");
		trynass_msg(vs.GetAPI() == GraphicAPI::DX11, L"A DX11InputLayout was constructed with a non DX11 Vertex Shader");

		auto& dx11vb = static_cast<DX11VertexBuffer&>(vb);
		auto& dx11vs = static_cast<DX11VertexShader&>(vs);

		type = GraphicAPI::DX11;

		buffer.push_back(*reinterpret_cast<D3D11_INPUT_ELEMENT_DESC*>(vb.GetLayoutFromVB().data()));

		gfx.GetDevice().CreateInputLayout(buffer.data(), (UINT)vb.ConstGet().GetLayout().GetElementCount(), dx11vs.GetBlob()->GetBufferPointer(), dx11vs.GetBlob()->GetBufferSize(), &pLayout) >> chk;
	}
	DX11InputLayout::DX11InputLayout(Graphics& gfx, IPolyVBuffer& pvb, IVertexShader& vs)
		:
		gfx(gfx)
	{
		trynass_msg(pvb.GetAPI() == GraphicAPI::DX11, L"A DX11InputLayout was constructed with a non DX11 Poly Vertex Buffer");
		trynass_msg(vs.GetAPI() == GraphicAPI::DX11, L"A DX11InputLayout was constructed with a non DX11 Vertex Shader");

		auto& dx11vs = static_cast<DX11VertexShader&>(vs);

		type = GraphicAPI::DX11;

		size_t totalElCount = 0;
		int slot = 0;
		for (auto& buf : pvb.slots)
		{
			auto layout = buf->GetSlottedLayoutFromVB(slot++);
			for (auto& element : layout)
			{
				buffer.push_back(std::any_cast<D3D11_INPUT_ELEMENT_DESC>(element));
			}
			totalElCount += buf->Get().GetLayout().GetElementCount();
		}
		gfx.GetDevice().CreateInputLayout(buffer.data(), (UINT)totalElCount, dx11vs.GetBlob()->GetBufferPointer(), dx11vs.GetBlob()->GetBufferSize(), &pLayout) >> chk;
	}
	//DX11InputLayout::DX11InputLayout(Graphics& gfx, StaticMesh& mesh, IVertexShader& vs)
	//	:
	//	gfx(gfx)
	//{
	//	trynass_msg(mesh.HasBindables(), L"A DX11InputLayout was constructed with a mesh with no bindings");
	//	trynass_msg(vs.GetAPI() == GraphicAPI::DX11, L"A DX11InputLayout was constructed with a non DX11 Vertex Shader");
	//	type = GraphicAPI::DX11;

	//	auto& dx11vs = static_cast<DX11VertexShader&>(vs);

	//	size_t totalElCount = 0;
	//	int slot = 0;
	//	for (auto& buf : mesh.GetPolyVBufer()->slots)
	//	{
	//		buffer.push_back(*reinterpret_cast<D3D11_INPUT_ELEMENT_DESC*>(buf->GetSlottedLayoutFromVB(slot++).data()));
	//		totalElCount += buf->Get().GetLayout().GetElementCount();
	//	}
	//	gfx.GetDevice()->CreateInputLayout(buffer.data(), (UINT)totalElCount, dx11vs.GetBlob()->GetBufferPointer(), dx11vs.GetBlob()->GetBufferSize(), &pLayout) >> chk;
	//}
	DX11InputLayout::DX11InputLayout(Graphics& gfx, VertexLayout& layout, IVertexShader& vs)
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
}