#pragma once
#include "Bindable.h"
#include <concepts>
#include <cstring>

namespace tryn::gfx
{
	class IConstantBufferResource;
	class IInstanceBuffer;
	class ConstantBuffer;

	template <typename BufferClass>
	concept CompatibleBufferType = requires (BufferClass buf) {
		{ buf.GetConstantBuffer() } -> std::same_as<ConstantBuffer&>;
	};

	template <CompatibleBufferType BufferClass>
	class JITUpdateBuffer;

	class IJITUpdateBuffer : public IBindable
	{
	public:
		virtual void Set(const void* pData, size_t size) = 0;
		bool Dirty() const;

		template <CompatibleBufferType BufferClass>
		static std::shared_ptr<IJITUpdateBuffer> MakeShared(const std::shared_ptr<BufferClass>& pBuff, const void* pData = nullptr, size_t numBytes = 0)
		{
			return std::make_shared<JITUpdateBuffer<BufferClass>>(pBuff, pData, numBytes);
		}
	protected:
		bool dirty = 0;
		const void* pData = nullptr;
		size_t numBytes = 0;
	};

	template <CompatibleBufferType BufferClass>
	class JITUpdateBuffer : public IJITUpdateBuffer
	{
	public:
		JITUpdateBuffer(const std::shared_ptr<BufferClass>& pBuffer, const void* pData = nullptr, size_t numBytes = 0)
			:
			pBuffer(pBuffer) {
			this->pData = pData;
			this->numBytes = numBytes;
		}
		void Set(const void* pData, size_t size) override
		{
			this->pData = pData;
			this->numBytes = size;
			dirty = true;
		}
		void Bind() override
		{
			if (pData != nullptr)
			{
				std::memcpy(pBuffer->GetConstantBuffer().Data(), pData, numBytes);
				dirty = false;
			}
			pBuffer->Bind();
		}
		void Bind(const IContext& ctx) override
		{
			if (pData != nullptr)
			{
				std::memcpy(pBuffer->GetConstantBuffer().Data(), pData, numBytes);
				dirty = false;
			}
			pBuffer->Bind(ctx);
		}
		BufferClass* Get() const
		{
			return pBuffer.get();
		}
	private:
		std::shared_ptr<BufferClass> pBuffer;
	};
}