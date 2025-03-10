#pragma once
#include "IWindow.h"
#include <string>
#include <thread>
#include <semaphore>
#include <atomic>
#include <optional>
#include <Core/src/spa/Dimensions.h>
#include <Core/src/spa/Vec2.h>
#include <Core/src/ccr/GenericTaskQueue.h>

namespace tryn::win
{
	class Window : public IWindow
	{
	public:
		Window(std::shared_ptr<class IWindowClass> pWindowClass, std::wstring title,
			spa::DimensionsI clientAreaSize, std::optional<spa::Vec2I> position = std::nullopt);
		WindowHandle GetHandle() const override;
		bool IsClosing() const override;
		spa::DimensionsI GetClientDimensions() const override;
		std::future<void> SetTitle(std::wstring title) override;
		~Window() override;
	protected:
		// constants
		static constexpr unsigned int CustomTaskMessageId = 0x0400 + 0;
		// functions
		virtual void MessageKernel_() noexcept;
		uintptr_t  HandleMessage_(WindowHandle hWnd, unsigned int msg, uintptr_t  wParam, uintptr_t  lParam) noexcept override;
		template<std::invocable F>
		auto Dispatch_(F&& f) const
		{
			auto future = tasks_.Push(std::forward<F>(f));
			NotifyTaskDispatch_();
			return future;
		}
		std::future<void> Resize(spa::DimensionsI newDimensions) override;
		std::future<void> SetResizableFlag(bool value) override;
		void NotifyTaskDispatch_() const;
		void NewFrame() override;
		void HideCursor() override;
		void ShowCursor() override;
		void ConfineCursor() override;
		void FreeCursor() override;
		// data
		std::shared_ptr<class IWindowClass> pWindowClass_;
		mutable ccr::GenericTaskQueue tasks_;
		std::binary_semaphore startSignal_{ 0 };
		std::thread kernelThread_;
		WindowHandle hWnd_ = nullptr;
		std::atomic<bool> closing_ = false;

	private:
		void Resize_( spa::DimensionsI newDimensions);
		unsigned int widthCache = 0;
		unsigned int heightCache = 0;

		bool wasMinimized = false;
		bool sizing = false;
	};
}