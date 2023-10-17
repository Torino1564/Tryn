#include "IWindow.h"
#include <Core/third/imgui/imgui.h>

namespace tryn::win
{
	void IWindow::OnKeyPress(uint8_t keyCode)
	{
		keyboard.OnKeyPress(std::move(keyCode));
	}
	void IWindow::OnKeyRelease(uint8_t keyCode)
	{
		keyboard.OnKeyRelease(std::move(keyCode));
	}
	void IWindow::OnChar(char character)
	{
		keyboard.OnChar(std::move(character));
	}
	void IWindow::ClearKeyboardState()
	{
		keyboard.Clear();
	}
	void IWindow::OnMouseMove(int x, int y) noexcept
	{
		mouse.OnMouseMove(x, y);
	}
	void IWindow::OnMouseLeave() noexcept
	{
		mouse.OnMouseLeave();
	}
	void IWindow::OnMouseEnter() noexcept
	{
		mouse.OnMouseEnter();
	}
	void IWindow::OnLeftPressed(int x, int y) noexcept
	{
		mouse.OnLeftPressed(x, y);
	}
	void IWindow::OnLeftReleased(int x, int y) noexcept
	{
		mouse.OnLeftReleased(x, y);
	}
	void IWindow::OnRightPressed(int x, int y) noexcept
	{
		mouse.OnRightPressed(x, y);
	}

	void IWindow::OnRightReleased(int x, int y) noexcept
	{
		mouse.OnRightReleased(x, y);
	}

	void IWindow::OnWheelPressed(int x, int y) noexcept
	{
		mouse.OnWheelPressed(x, y);
	}

	void IWindow::OnWheelReleased(int x, int y) noexcept
	{
		mouse.OnWheelReleased(x, y);
	}

	void IWindow::OnWheelUp(int x, int y) noexcept
	{
		mouse.OnWheelUp(x, y);
	}

	void IWindow::OnWheelDown(int x, int y) noexcept
	{
		mouse.OnWheelDown(x, y);
	}

	void IWindow::TrimBuffer() noexcept
	{
		mouse.TrimBuffer();
	}

	void IWindow::OnWheelDelta(int x, int y, int delta) noexcept
	{
		mouse.OnWheelDelta(x, y, delta);
	}
	void IWindow::OnRawDelta(int dx, int dy) noexcept
	{
		mouse.OnRawDelta(dx, dy);
	}
	void IWindow::EnableImGuiMouse()
	{
		ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
	}
	void IWindow::DisableImGuiMouse()
	{
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
	}
	bool IWindow::IsCursorEnabled() const
	{
		return isCursorEnabled;
	}
	void IWindow::EnableCursor()
	{
		isCursorEnabled = true;
		ShowCursor();
		EnableImGuiMouse();
		FreeCursor();
	}
	void IWindow::DisableCursor()
	{
		isCursorEnabled = false;
		HideCursor();
		DisableImGuiMouse();
		ConfineCursor();
	}
}

