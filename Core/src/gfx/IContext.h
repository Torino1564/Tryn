#pragma once
#include <Core/src/gfx/GraphicAPI.h>
#include <memory>
#include <Core/src/gfx/IBufferFwd.h>

namespace tryn::gfx
{
	class IGraphics;
	class IContext
	{
		friend class IGraphics;
		friend class Graphics;
	public:
		virtual ~IContext() = default;
		constexpr virtual GraphicAPI GetApi() const = 0;
		virtual void Submit(IGraphics&) = 0;
		virtual void DrawIndexed(int count) = 0;
		gfx::IVtxConstantBuffer& GetTransfromBuffer()
		{
			return *pTCB;
		}
		void SetDeferred(bool setting)
		{
			deferred = setting;
		}
		bool IsDeferred(void)
		{
			return deferred;
		}
	protected:
		bool deferred = false;
		std::unique_ptr<gfx::IVtxConstantBuffer> pTCB;
	};
}