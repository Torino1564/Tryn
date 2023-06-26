#pragma once
#include <Core/src/win/TrynWin.h>
#include <optional>
#include <Core/src/utl/Assert.h>

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
		virtual void DrawIndexed(int count) = 0;

		template<typename T>
		T* QueryInterface()
		{
			auto ptr = dynamic_cast<T*>(this);
			trynass_msg(ptr,L"Attempt to cast query IGraphics interface to an invalid type");
			return ptr;
		}
	};
}