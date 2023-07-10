#include "DX11PolyVBuffer.h"
#include <Core/src/gfx/dx11/Bindables/DX11InputLayout.h>

namespace tryn::gfx::dx11
{
	DX11PolyVBuffer::DX11PolyVBuffer(Graphics& gfx)
		:
		gfx(gfx)
	{}

	void DX11PolyVBuffer::Append(std::shared_ptr<IVertexBuffer> pVB)
	{
		slots.push_back(pVB);
	}


	void DX11PolyVBuffer::Bind()
	{
		int slot = 0;
		for (auto& vBuffer : slots)
		{
			vBuffer->BindSlotted(slot, (int)Size());
			slot++;
		}
	}

}