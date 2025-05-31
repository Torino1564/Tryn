#pragma once
#include "Bindable.h"
#include <Core/src/gfx/IBufferFwd.h>

namespace tryn::gfx
{
	class IBuffer;

	class JITUpdateBuffer : public IBindable
	{
	public:
		template <BufferType Type, CachingPolicy Policy>
		static JITUpdateBuffer Make(const std::shared_ptr<IBufferBase<Type, Policy>>& pBuffer, const void* pData = nullptr, size_t numBytes = 0)
			requires (Type == BufferType::VtxConstant || Type == BufferType::PxConstant || Type == BufferType::Instance)
		{
			return JITUpdateBuffer(pBuffer, pData, numBytes);
		}
		void Set(const void* pData, size_t size);
		void Bind() override;
		void Bind(const IContext& ctx) override;
		bool Dirty() const;
		IBuffer* Get() const;
	private:
		JITUpdateBuffer(const std::shared_ptr<IBuffer>& pBuffer, const void* pData, size_t numBytes);
		const void* pData;
		size_t numBytes;
		std::shared_ptr<IBuffer> pBuffer;
		bool dirty = true;
	};
}