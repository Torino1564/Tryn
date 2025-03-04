#pragma once
#include "TrynWin.h"
#include <future>
#include <string>
#include <optional>
#include <Core/src/spa/Dimensions.h>
#include <Core/src/spa/Vec2.h>
#include "Keyboard.h"
#include "Mouse.h"
#include "WindowHandle.h"

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
		virtual WindowHandle GetHandle() const = 0;
		virtual bool IsClosing() const = 0;
		virtual std::future<void> SetTitle(std::wstring title) = 0;
		virtual spa::DimensionsI GetClientDimensions() const = 0;
		bool IsCursorEnabled() const;
		void EnableCursor();
		void DisableCursor();
		virtual std::future<void> Resize(spa::DimensionsI newDimensions) = 0;
		virtual std::future<void> SetResizableFlag(bool value) = 0;
		virtual void NewFrame() = 0;
		bool HasSizeChanged() const;
		void AcknowledgeSizeChange();
	public:
		Keyboard keyboard;
		Mouse mouse;
	protected:
		bool isCursorEnabled = true;
		spa::DimensionsI clientDimensions;
		std::vector<char> rawBufer;
	protected:
		virtual LRESULT HandleMessage_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept = 0;

		// Keyboard Window Interface
		void OnKeyPress(uint8_t keyCode);
		void OnKeyRelease(uint8_t keyCode);
		void OnChar(char character);
		void ClearKeyboardState();

		// Mouse Window Interface
		void OnMouseMove(int x, int y) noexcept;
		void OnMouseLeave() noexcept;
		void OnMouseEnter() noexcept;
		void OnLeftPressed(int x, int y) noexcept;
		void OnLeftReleased(int x, int y) noexcept;
		void OnRightPressed(int x, int y) noexcept;
		void OnRightReleased(int x, int y) noexcept;
		void OnWheelPressed(int x, int y) noexcept;
		void OnWheelReleased(int x, int y) noexcept;
		void OnWheelUp(int x, int y) noexcept;
		void OnWheelDown(int x, int y) noexcept;
		void TrimBuffer() noexcept;
		void OnWheelDelta(int x, int y, int delta) noexcept;
		void OnRawDelta(int dx, int dy) noexcept;
		virtual void HideCursor() = 0;
		virtual void ShowCursor() = 0;
		void EnableImGuiMouse();
		void DisableImGuiMouse();
		virtual void ConfineCursor() = 0;
		virtual void FreeCursor() = 0;

		bool sizeChanged = false;
	};
}