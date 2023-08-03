#pragma once
#include "TrynWin.h"
#include <string>

namespace tryn::win
{
	class IWindowClass
	{
	public:
		virtual ~IWindowClass() = default;
		virtual ATOM GetAtom() const = 0;
		virtual HINSTANCE GetInstance() const = 0;
	protected:
		// allows descendents of IWindowClass access to the IWindow virtual message handler
		static LRESULT ForwardMessage_(class IWindow* pWnd, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	};

	class WindowClass : public IWindowClass
	{
	public:
		WindowClass(const std::wstring& className = L"$Tryn-Default-Window-Class");
		ATOM GetAtom() const override;
		HINSTANCE GetInstance() const override;
		~WindowClass() override;
	private:
		// functions
		static LRESULT CALLBACK HandleMessageSetup_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		static LRESULT CALLBACK HandleMessageThunk_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		// data
		HINSTANCE hInstance_;
		ATOM atom_;
	};
}