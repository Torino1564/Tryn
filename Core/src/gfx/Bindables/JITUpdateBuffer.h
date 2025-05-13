#pragma once
#include "Bindable.h"
#include <Core/src/gfx/IBufferFwd.h>

namespace tryn::gfx
{
	template <IBuffer<>>
	class JITUpdateBuffer : public IBindable
	{
	public:
		JITUpdateBuffer(IVtxConstantBuffer* pBuffer, void* pData, size_t numBytes);
		void Bind() override;
		void Bind(const IContext& ctx) override;
	private:
		void* pData;
		size_t numBytes;
		IVtxConstantBuffer* pBuffer;
	};
}