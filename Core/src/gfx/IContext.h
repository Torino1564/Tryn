#pragma once
#include <Core/src/gfx/GraphicAPI.h>
#include <memory>
#include "Bindables/IBufferBase.h"

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
		virtual void Submit(const IGraphics&) = 0;
		virtual void DrawIndexed(int count) const = 0;
		virtual void DrawIndexedInstanced(int indexCount, int instanceCount, int startIndexLocation, int baseVertexLocation, int startInstanceLocation) const = 0;
		virtual void UpdateContextDimensions(const IGraphics& gfx) = 0;
		virtual void Flush() = 0;
		virtual void ClearState() = 0;
		gfx::IVtxConstantBuffer& GetTransfromBuffer() const;
		void SetDeferred(bool setting);
		bool IsDeferred(void);
	protected:
		bool deferred = false;
		std::unique_ptr<gfx::IVtxConstantBuffer> pTCB;
	};
}