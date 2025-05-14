#include "TrynPCH.h"
#include "DX11SOAVertexBuffer.h"
#include <Core/src/gfx/dx11/Dx11Graphics.h>

#include "Core/src/gfx/Bindables/IBufferBase.h"
#include "Core/src/gfx/dx11/DX11BufferFwd.h"
#include "Core/src/gfx/dx11/Bindables/DX11VertexShader.h"
#include "Core/src/gfx/dx11/Bindables/DX11Buffer.h"

namespace tryn::gfx::dx11
{
	DX11SOAVertexBuffer::DX11SOAVertexBuffer(const Graphics& gfx, const std::shared_ptr<IVertexShader>& pVS)
		: gfx(gfx)
	{
		trynchk(pVS->GetAPI() == GraphicAPI::DX11).msg(L"Missmatch between elements using different graphic APIs");
		this->pVS = std::static_pointer_cast<DX11VertexShader>(pVS);
	}

	void DX11SOAVertexBuffer::Bind()
	{
		if (dirty)
		{
			Update();
		}
		Bind_Impl(gfx.GetContextInterface());
	}

	void DX11SOAVertexBuffer::Bind(const IContext& context)
	{
		if (dirty)
		{
			Update();
		}
		Bind_Impl(context);
	}

	void DX11SOAVertexBuffer::AssertApiMatch(const std::shared_ptr<IVertexBuffer>& pVB)
	{
		trynchk(pVB->GetAPI() == GraphicAPI::DX11).msg(L"Missmatch between elements using different graphic APIs");
	}

	void DX11SOAVertexBuffer::Bind_Impl(const IContext& context)
	{
		
	}

	void DX11SOAVertexBuffer::Update()
	{
		strides.clear();
		strides.resize(pBuffers.size());
		offsets.clear();
		offsets.resize(pBuffers.size());
		buffArray.clear();
		buffArray.resize(pBuffers.size());

		for (auto& [name, data] : pBuffers)
		{
			auto& [buffer, slot] = data;
			auto casted = std::static_pointer_cast<DX11VertexBuffer>(buffer);

			
		}
	}
}
