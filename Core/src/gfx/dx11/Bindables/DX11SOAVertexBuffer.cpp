#include "TrynPCH.h"
#include "DX11SOAVertexBuffer.h"
#include <Core/src/gfx/dx11/Dx11Graphics.h>

#include "DX11InputLayout.h"
#include "Core/src/gfx/Bindables/IBufferBase.h"
#include "Core/src/gfx/dx11/DX11BufferFwd.h"
#include "Core/src/gfx/dx11/Bindables/DX11VertexShader.h"
#include "Core/src/gfx/dx11/Bindables/DX11Buffer.h"

namespace tryn::gfx::dx11
{
	DX11SOAVertexBuffer::DX11SOAVertexBuffer(const Graphics& gfx)
		: gfx(gfx)
	{
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

	void DX11SOAVertexBuffer::SetVertexShader(const std::shared_ptr<IVertexShader>& pVS)
	{
		trynchk(pVS->GetAPI() == GraphicAPI::DX11).msg(L"Missmatch between elements using different graphic APIs");
		this->pVS = std::static_pointer_cast<DX11VertexShader>(pVS);
	}

	void DX11SOAVertexBuffer::AssertApiMatch(const std::shared_ptr<IVertexBuffer>& pVB)
	{
		trynchk(pVB->GetAPI() == GraphicAPI::DX11).msg(L"Missmatch between elements using different graphic APIs");
	}

	void DX11SOAVertexBuffer::Bind_Impl(const IContext& context) const
	{
		const auto& dx11context = static_cast<const DX11Context&>(context);
		pLayout->Bind(context);
		dx11context.GetContext().IASetVertexBuffers((UINT)0, (UINT)buffArray.size(), buffArray.data(), strides.data(), offsets.data());

	}

	void DX11SOAVertexBuffer::Update()
	{
		strides.clear();
		strides.resize(pBuffers.size());
		offsets.clear();
		offsets.resize(pBuffers.size());
		buffArray.clear();
		buffArray.resize(pBuffers.size());

		std::vector<D3D11_INPUT_ELEMENT_DESC> descBuffer;
		for (const auto& pair : pBuffers | std::views::values)
		{
			const auto& casted = std::static_pointer_cast<DX11VertexBuffer>(pair.first);

			// Input layout creation
			const auto& buffer = pair.first;
			const auto slot= pair.second;
			trylog.info(utl::ToWide(buffer->Test().data()));
			const auto nthDescBuffer = buffer->GetSlottedLayoutFromVB(slot);
			for (auto& any : nthDescBuffer)
			{
				const auto& elementDescriptor = std::any_cast<D3D11_INPUT_ELEMENT_DESC>(any);
				descBuffer.push_back(elementDescriptor);
			}

			strides[slot] = buffer->GetVertexBuffer().Stride();
			offsets[slot] = 0u;
			buffArray[slot] = casted->Data();
		}

		// Input layout creation
		pLayout = std::make_shared<DX11InputLayout>(gfx, descBuffer, *pVS);

		dirty = false;
	}
}
