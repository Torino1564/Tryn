
#include "JITUpdateBuffer.h"
#include "IBufferBase.h"

tryn::gfx::JITUpdateBuffer::JITUpdateBuffer(const std::shared_ptr<IBuffer>& pBuffer, const void* pData, const size_t numBytes)
	:
	pData(pData), numBytes(numBytes), pBuffer(pBuffer)
{
}

void tryn::gfx::JITUpdateBuffer::Set(const void* pData, const size_t size)
{
	this->pData = pData;
	this->numBytes = size;
	dirty = true;
}

void tryn::gfx::JITUpdateBuffer::Bind()
{
	if (pData != nullptr)
	{
		std::memcpy(pBuffer->GetCPUBuffer().Data(), pData, numBytes);
		dirty = false;
	}
	pBuffer->Bind();
}

void tryn::gfx::JITUpdateBuffer::Bind(const IContext& ctx)
{
	if (pData != nullptr)
	{
		std::memcpy(pBuffer->GetCPUBuffer().Data(), pData, numBytes);
		dirty = false;
	}
	pBuffer->Bind(ctx);
}

bool tryn::gfx::JITUpdateBuffer::Dirty() const
{
	return dirty;
}

tryn::gfx::IBuffer* tryn::gfx::JITUpdateBuffer::Get() const
{
	return pBuffer.get();
}
