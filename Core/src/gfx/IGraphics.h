#pragma once
#include <Core/src/win/TrynWin.h>
#include <optional>

namespace tryn::gfx
{
	class IGraphics
	{
	public:
		struct IocParams
		{
			std::optional<int> width;
			std::optional<int> height;
			HWND hWnd;
		};

		virtual ~IGraphics() {}
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void ClearBuffer(float r, float g, float b) = 0;
		virtual void DrawTriangle() = 0;
	};
}