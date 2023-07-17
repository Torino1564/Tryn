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
				return true;
			switch (msg) {
			case WM_DESTROY:
				hWnd_ = nullptr;
				PostQuitMessage(0);
				return 0;
			case WM_CLOSE:
				closing_ = true;
				return 0;
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