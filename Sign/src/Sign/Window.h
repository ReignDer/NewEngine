#pragma once
#include <string>
#include <functional>
#include <Windows.h>

#include <directx/d3d12.h>
#include <DirectXMath.h>

#include "Events/Event.h"
#include "Events/WindowEvents.h"
#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"
#include "D3D12Context.h"

namespace Sign {
	struct WindowSpecifications {
		std::string Title;
		uint32_t Width = 1280;
		uint32_t Height = 720;
		bool isResizable = true;
		bool VSync = true;

		using EventCallbackFn = std::function<void(Event&)>;
		EventCallbackFn EventCallback;
	};
	class Window
	{
	public:
		Window(const WindowSpecifications& specification = WindowSpecifications());

		~Window();

		void Create();
		void Destroy();

		void Update();
		void PollEvents();

		DirectX::XMFLOAT2 GetFrameBufferSize() const;
		DirectX::XMFLOAT2 GetMousePos() const;

		inline unsigned int GetWidth() const { return m_WindowsSpecification.Width; }
		inline unsigned int GetHeight() const { return m_WindowsSpecification.Height; }

		bool ShouldClose();
		HWND GetHandle() const { return m_WindowsHandle; }
		D3D12Context* GetContext() { return m_Context.get(); }
	private:
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	private:
		WindowSpecifications m_WindowsSpecification;

		std::unique_ptr<D3D12Context> m_Context;

		HWND m_WindowsHandle = nullptr;
		HINSTANCE m_InstanceHandle = nullptr;

		bool m_ShouldClose;
	};
}

