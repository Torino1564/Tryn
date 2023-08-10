#include "IWindow.h"

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
}

