#include "JITUpdateBuffer.h"
#include "IBuffer.h"

tryn::gfx::JITUpdateBuffer::JITUpdateBuffer(IVtxConstantBuffer* pBuffer, void* pData, size_t numBytes)
	:
	pBuffer(pBuffer), pData(pData), numBytes(numBytes)
{
}

void tryn::gfx::JITUpdateBuffer::Bind()
{
	memcpy(pBuffer->GetCPUBuffer().Data(), pData, numBytes);
	pBuffer->Bind();
}

void tryn::gfx::JITUpdateBuffer::Bind(const IContext& ctx)
{
	memcpy(pBuffer->GetCPUBuffer().Data(), pData, numBytes);
	pBuffer->Bind(ctx);
}
