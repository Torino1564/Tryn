#include "Window.h"
#include "Utilities.h"
#include "Exception.h"
#include <format>
#include <Core/src/log/Log.h>
#include <Core/src/utl/String.h>
#include "imgui_impl_win32.h"

namespace tryn::win
{
	Window::Window(std::shared_ptr<IWindowClass> pWindowClass, std::wstring title,
		spa::DimensionsI clientAreaSize, std::optional<spa::Vec2I> position)
		:
		pWindowClass_{ std::move(pWindowClass) },
		kernelThread_{ &Window::MessageKernel_, this },
		clientDimensions(clientAreaSize)
	{
		auto future = tasks_.Push([=, this] {
			const DWORD styles = WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
			const DWORD exStyles = 0;
			const auto windowDims = ClientToWindowDimensions(clientAreaSize, styles);
			const auto hModule = GetModuleHandleW(nullptr);
			if (!hModule) {
				trylog.error().hr();
				throw WindowException{ "Failed to get module handle" };
			}
			hWnd_ = CreateWindowExW(
				exStyles,
				MAKEINTATOM(pWindowClass_->GetAtom()),
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
			});
		startSignal_.release();
		future.get();
	}
	HWND Window::GetHandle() const
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
			if (!SetWindowTextW(hWnd_, title.c_str())) {
				trylog.warn(L"Failed setting window title").hr();
			}
			});
	}
	Window::~Window()
	{
		Dispatch_([this] {
			ImGui_ImplWin32_Shutdown();
			if (!DestroyWindow(hWnd_)) {
				trylog.warn(L"Failed destroying window").hr();
			}
			});
		kernelThread_.join();
	}
	LRESULT Window::HandleMessage_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		try {
			extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
			if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
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
			/******************** KEYBOARD MESSAGES *****************/
			case WM_KEYDOWN:
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
						SetCapture(hWnd);
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
			case CustomTaskMessageId:
				tasks_.PopExecute();
				return 0;
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

		return DefWindowProcW(hWnd, msg, wParam, lParam);
	}
	void Window::NotifyTaskDispatch_() const
	{
		if (!PostMessageW(hWnd_, CustomTaskMessageId, 0, 0)) {
			trylog.error().hr();
			throw WindowException{ "Failed to post task notification message" };
		}
	}
	void Window::MessageKernel_() noexcept
	{
		startSignal_.acquire();
		tasks_.PopExecute();

		MSG msg{};
		while (GetMessageW(&msg, hWnd_, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
}