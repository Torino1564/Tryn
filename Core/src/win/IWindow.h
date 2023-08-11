#pragma once
#include "TrynWin.h"
#include <future>
#include <string>
#include <optional>
#include <Core/src/spa/Dimensions.h>
#include <Core/src/spa/Vec2.h>
#include "Keyboard.h"

namespace tryn::win
{
	class IWindow
	{
		// allow WindowClasses access to the message handling function
		friend class IWindowClass;
	public:
		// types
		struct IocParams
		{
			std::optional<std::shared_ptr<IWindowClass>> pClass;
			std::optional<std::wstring> name;
			std::optional<spa::DimensionsI> size;
			std::optional<spa::Vec2I> position;
		};
		// functions
		virtual ~IWindow() = default;
		virtual HWND GetHandle() const = 0;
		virtual bool IsClosing() const = 0;
		virtual std::future<void> SetTitle(std::wstring title) = 0;
		virtual spa::DimensionsI GetClientDimensions() const = 0;
	public:
		Keyboard keyboard;
	protected:
		virtual LRESULT HandleMessage_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept = 0;

		// Keyboard Window Interface
		void OnKeyPress(uint8_t keyCode);
		void OnKeyRelease(uint8_t keyCode);
		void OnChar(char character);
		void ClearKeyboardState();
	};
}