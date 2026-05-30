
#include <Windows.h>
#include "Window.h"
#include "Utilities.h"
#include "Exception.h"
#include <format>
#include <imgui.h>
#include <Core/src/log/Log.h>
#include <Core/src/utl/String.h>
#include <imgui_impl_win32.h>
#include "Core/third/backward.hpp"
#include "WindowClass.h"
#include <tuple>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace tryn::win
{
	Window::Window(std::shared_ptr<IWindowClass> pWindowClass, std::wstring title,
		spa::DimensionsI clientAreaSize, std::optional<spa::Vec2I> position)
		:
		pWindowClass_{ std::move(pWindowClass) },
		kernelThread_{ &Window::MessageKernel_, this }
	{
		clientDimensions = clientAreaSize;
		auto future = tasks_.Push([=, this] {
			const DWORD styles = WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
			const DWORD exStyles = 0;
			const auto windowDims = ClientToWindowDimensions(clientAreaSize, styles);
			const auto hModule = GetModuleHandleW(nullptr);
			if (!hModule) {
				trylog.error().hr();
				throw WindowException{ "Failed to get module handle" };
			}
			hWnd_ = (WindowHandle)CreateWindowExW(
				exStyles,
				(LPCWSTR)(MAKEINTATOM(pWindowClass_->GetAtom())),
				title.c_str(),
				styles,
				position.transform([](auto v) {return v.x; }).value_or(CW_USEDEFAULT),
				position.transform([](auto v) {return v.y; }).value_or(CW_USEDEFAULT),
				windowDims.width, windowDims.height,
				nullptr, nullptr, hModule,
				this
			);
			if (!hWnd_) {
				trylog.error(L"Failed creating window").hr();
				throw WindowException{ "Failed creating window" };
			}
			ImGui_ImplWin32_Init(hWnd_);
			RAWINPUTDEVICE rid;
			rid.usUsage = 0x02;
			rid.usUsagePage = 0x01;
			rid.dwFlags = 0;
			rid.hwndTarget = nullptr;

			RegisterRawInputDevices(&rid, 1, sizeof(rid));
			});
		startSignal_.release();
		future.get();
	}
	WindowHandle Window::GetHandle() const
	{
		return hWnd_;
	}
	bool Window::IsClosing() const
	{
		return closing_;
	}
	spa::DimensionsI Window::GetClientDimensions() const
	{
		return clientDimensions;
	}
	std::future<void> Window::SetTitle(std::wstring title)
	{
		return Dispatch_([=, this] {
			if (!SetWindowTextW((HWND)hWnd_, title.c_str())) {
				trylog.warn(L"Failed setting window title").hr();
			}
			});
	}
	Window::~Window()
	{
		Dispatch_([this] {
			ImGui_ImplWin32_Shutdown();
			if (!DestroyWindow((HWND)hWnd_)) {
				trylog.warn(L"Failed destroying window").hr();
			}
			});
		kernelThread_.join();
	}
	uintptr_t  Window::HandleMessage_(WindowHandle hWnd, unsigned int msg, uintptr_t  wParam, uintptr_t  lParam) noexcept
	{
		try {
			if (ImGui_ImplWin32_WndProcHandler((HWND)hWnd, msg, wParam, lParam))
			{
				return true;
			}

			const auto& imio = ImGui::GetIO();

			switch (msg) {
			case WM_DESTROY:
				hWnd_ = nullptr;
				PostQuitMessage(0);
				return 0;
			case WM_CLOSE:
				closing_ = true;
				return 0;
			case WM_KILLFOCUS:
				ClearKeyboardState();
				break;
			case WM_ACTIVATE:
			{
				if (!isCursorEnabled)
				{
					if (wParam & WA_ACTIVE || wParam & WA_CLICKACTIVE)
					{
						ConfineCursor();
						HideCursor();
					}
					else
					{
						FreeCursor();
						ShowCursor();
					}
				}
			}
				break;
			/******************** KEYBOARD MESSAGES *****************/
			case WM_KEYDOWN:
				[[fallthrough]];
			case WM_SYSKEYDOWN:
				if (!(lParam & 0x40000000) || keyboard.IsAutoRepeatEnabled())
				{
					OnKeyPress(static_cast<uint8_t>(wParam));
				}
				break;
			case WM_KEYUP:
			case WM_SYSKEYUP:
				OnKeyRelease(static_cast<uint8_t>(wParam));
				break;
			case WM_CHAR:
				OnChar(static_cast<char>(wParam));
				break;
			/******************** END KEYBOARD MESSAGES *****************/
			/********************** RAW MOUSE MESSAGES *******************/
			case WM_INPUT:
			{
				if (!mouse.IsRawEnabled())
				{
					break;
				}
				UINT size = 0;
				if (GetRawInputData(
					reinterpret_cast<HRAWINPUT>(lParam),
					RID_INPUT,
					nullptr,
					&size,
					sizeof(RAWINPUTHEADER)) == -1)
				{
					trylog.warn(L"Failed to receive raw mouse input data");
					break;
				}
				rawBufer.resize(size);

				if (GetRawInputData(
					reinterpret_cast<HRAWINPUT>(lParam),
					RID_INPUT,
					rawBufer.data(),
					&size,
					sizeof(RAWINPUTHEADER)) != size)
				{
					trylog.warn(L"Failed to receive raw mouse input data");
					break;
				}

				auto& rawInput = reinterpret_cast<const RAWINPUT&>(*rawBufer.data());
				if (rawInput.header.dwType == RIM_TYPEMOUSE &&
					(rawInput.data.mouse.lLastX != 0 || rawInput.data.mouse.lLastY != 0))
				{
					OnRawDelta(rawInput.data.mouse.lLastX, rawInput.data.mouse.lLastY);
				}
			}
				break;
			/******************** END RAW MOUSE MESSAGES *****************/
			/************************ MOUSE MESSAGES *********************/
			case WM_MOUSEMOVE:
			if (imio.WantCaptureMouse)
			{
				break;
			}
			{
				const POINTS pt = MAKEPOINTS(lParam);
				// in client region -> log move, and log enter + capture mouse (if not previously in window)
				if (pt.x >= 0 && pt.x < clientDimensions.width && pt.y >= 0 && pt.y < clientDimensions.height)
				{
					OnMouseMove(pt.x, pt.y);
					if (!mouse.IsInWindow())
					{
						SetCapture((HWND)hWnd);
						OnMouseEnter();
					}
				}
				// not in client -> log move / maintain capture if button down
				else
				{
					if (wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON))
					{
						OnMouseMove(pt.x, pt.y);
					}
					// button up -> release capture / log event for leaving
					else
					{
						ReleaseCapture();
						OnMouseLeave();
					}
				}
				break;
			}
			case WM_LBUTTONDOWN:
				if (imio.WantCaptureMouse)
				{
					break;
				}
				{
					const POINTS pt = MAKEPOINTS(lParam);
					OnLeftPressed(pt.x, pt.y);
					break;
				}
			case WM_RBUTTONDOWN:
				if (imio.WantCaptureMouse)
				{
					break;
				}
				{
					const POINTS pt = MAKEPOINTS(lParam);
					OnRightPressed(pt.x, pt.y);
					break;
				}
			case WM_MBUTTONDOWN:
				if (imio.WantCaptureMouse)
				{
					break;
				}
				{
					const POINTS pt = MAKEPOINTS(lParam);
					OnWheelPressed(pt.x, pt.y);
					break;
				}
			case WM_LBUTTONUP:
				if (imio.WantCaptureMouse)
				{
					break;
				}
				{
					const POINTS pt = MAKEPOINTS(lParam);
					OnLeftReleased(pt.x, pt.y);
					break;
				}
			case WM_RBUTTONUP:
				if (imio.WantCaptureMouse)
				{
					break;
				}
				{
					const POINTS pt = MAKEPOINTS(lParam);
					OnRightReleased(pt.x, pt.y);
					break;
				}
			case WM_MBUTTONUP:
				if (imio.WantCaptureMouse)
				{
					break;
				}
				{
					const POINTS pt = MAKEPOINTS(lParam);
					OnWheelReleased(pt.x, pt.y);
					break;
				}
			case WM_MOUSEWHEEL:
				if (imio.WantCaptureMouse)
				{
					break;
				}
				{
					const POINTS pt = MAKEPOINTS(lParam);
					const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
					OnWheelDelta(pt.x, pt.y, delta);
					break;
				}

			/********************** END MOUSE MESSAGES *******************/
			/********************* CUSTOM TASK MESSAGES ******************/
			case CustomTaskMessageId:
				tasks_.PopExecute();
				break;
			/********************* CUSTOM TASK MESSAGES ******************/
			/*********************** RESIZING MESSAGES ********************/
			case WM_SIZE:
				{
					sizeChanged = true;
					break;
				}
			/*********************** RESIZING MESSAGES ********************/

			}
		}
		catch (const std::exception& e) {
			trylog.error(std::format(
				L"Uncaught exception in Windows message handler: {}",
				utl::ToWide(e.what())
			));
		}
		catch (...) {
			trylog.error(L"Uncaught annonymous exception in Windows message handler");
		}

		return DefWindowProcW((HWND)hWnd, msg, wParam, lParam);
	}

	std::future<void> Window::Resize(const spa::DimensionsI newDimensions)
	{
		return Dispatch_([=, this] {
				Resize_(newDimensions);
			});
	}

	std::future<void> Window::SetResizableFlag(const bool value)
	{
		WINDOWINFO windowInfo = {};
		GetWindowInfo((HWND)hWnd_, &windowInfo);
		const auto newStyle = WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		return Dispatch_([=, this] {
			if (!SetWindowLongPtr((HWND)hWnd_, GWL_STYLE, value ? (windowInfo.dwStyle | newStyle) : (windowInfo.dwStyle & ~newStyle))) {
				trylog.warn(L"Failed changing the resize flag!").hr();
			}
			});
	}

	void Window::NotifyTaskDispatch_() const
	{
		if (!PostMessageW((HWND)hWnd_, CustomTaskMessageId, 0, 0)) {
			trylog.error().hr();
			throw WindowException{ "Failed to post task notification message" };
		}
	}
	void Window::NewFrame()
	{
		ImGui_ImplWin32_NewFrame();
	}
	void Window::HideCursor()
	{
		Dispatch_([=,this]
			{
				while (::ShowCursor(FALSE) >= 0);
			});
	}
	void Window::ShowCursor()
	{
		Dispatch_([=, this]
			{
				while (::ShowCursor(TRUE) < 0);
			});
	}
	void Window::ConfineCursor()
	{
		RECT rect;
		GetClientRect((HWND)hWnd_, &rect);
		MapWindowPoints((HWND)hWnd_, nullptr, reinterpret_cast<POINT*>(&rect), 2);
		ClipCursor(&rect);
	}
	void Window::FreeCursor()
	{
		ClipCursor(nullptr);
	}

	void Window::Resize_(const spa::DimensionsI newDimensions)
	{
		sizeChanged = true;
		WINDOWINFO windowInfo = {};
		GetWindowInfo((HWND)hWnd_, &windowInfo);
		clientDimensions = ClientToWindowDimensions(newDimensions, windowInfo.dwStyle);
		if (!SetWindowPos((HWND)hWnd_, nullptr, 0, 0, newDimensions.width, newDimensions.height, SWP_NOMOVE | SWP_FRAMECHANGED)) {
			trylog.warn(L"Failed resizing the window!").hr();
		}
	}

	void Window::MessageKernel_() noexcept
	{
		startSignal_.acquire();
		tasks_.PopExecute();

		MSG msg{};
		while (GetMessageW(&msg, (HWND)hWnd_, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

#include <shlobj_core.h>
	std::pair<bool, std::filesystem::path> SelectDirectory()
	{
		const auto currentWD = std::filesystem::current_path();
		BROWSEINFOA browseInfo = {};

		const auto rv = SHBrowseForFolderA(&browseInfo);
		SetCurrentDirectoryA(currentWD.string().c_str());
		if (rv == nullptr)
		{
			return {false, {}};
		}
		char buffer[MAX_PATH] = {};
		if (SHGetPathFromIDListA(rv, buffer))
		{
			return {true, {buffer}};
		}
		else
		{
			return {false, {}};
		}
	}

#include <commdlg.h>
	std::pair<bool, std::filesystem::path> SelectFile(std::vector<std::string> extensions)
	{
		const auto currentWD = std::filesystem::current_path();
		char filename[ MAX_PATH ];

		std::string extensionString;
		for (const auto ext : extensions)
		{
			extensionString += '*' + ext + '\0'; 
		}

		OPENFILENAMEA ofn;
		ZeroMemory( &filename, sizeof( filename ) );
		ZeroMemory( &ofn,      sizeof( ofn ) );
		ofn.lStructSize  = sizeof( ofn );
		ofn.hwndOwner    = nullptr;  // If you have a window to center over, put its HANDLE here
		ofn.lpstrFilter  = extensionString.c_str();
		ofn.lpstrFile    = filename;
		ofn.nMaxFile     = MAX_PATH;
		ofn.lpstrTitle   = "Select a File, yo!";
		ofn.Flags        = 0x02000000 | 0x00001000;

		const auto res = GetOpenFileNameA(&ofn);
		SetCurrentDirectoryA(currentWD.string().c_str());
	    if (res)
	    {
		    return {true, {filename}};
	    }
		return {false, {}};
	}
}
