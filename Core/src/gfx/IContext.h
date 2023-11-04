#pragma once
#include <Core/src/gfx/GraphicAPI.h>

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
		virtual void Submit() = 0;
		virtual void DrawIndexed(int count) = 0;
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
	};
}