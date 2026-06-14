#pragma once
#include <string>
#include <functional>
#include <Windows.h>

#include <d3d12.h>
#include <DirectXMath.h>

#include "Event.h"

namespace Sign {
	struct WindowSpecifications {
		std::string Title;
		uint32_t Width = 1280;
		uint32_t Height = 720;
		bool isResizable = true;
		bool VSync = true;

		using EventCallbackFn = std::function<void(Event&)>;
	};
	class Window
	{
	public:
		Window(const WindowSpecifications& specification = WindowSpecifications());

		~Window();

		void Create();
		void Destroy();

		void Update();

		DirectX::XMFLOAT2 GetFrameBufferSize() const;
		DirectX::XMFLOAT2 GetMousePos() const;

		inline unsigned int GetWidth() const { return m_WindowsSpecification.Width; }
		inline unsigned int GetHeight() const { return m_WindowsSpecification.Height; }

		bool ShouldClose();
		HWND GetHandle() const { return m_WindowsHandle; }
	private:
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	private:
		WindowSpecifications m_WindowsSpecification;

		HWND m_WindowsHandle = nullptr;
		HINSTANCE m_InstanceHandle = nullptr;

		bool m_ShouldClose;
	};
}

