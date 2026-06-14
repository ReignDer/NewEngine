#include "Window.h"

namespace Sign {
	Window::Window(const WindowSpecifications& specification) : m_WindowsSpecification(specification)
	{
	}
	Window::~Window()
	{
		Destroy();
	}
	void Window::Create()
	{
		m_InstanceHandle = GetModuleHandle(nullptr);

		WNDCLASSEX wc;
		wc.cbClsExtra = NULL;
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.cbWndExtra = NULL;
		wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		wc.hInstance = m_InstanceHandle;
		wc.lpszClassName = "SignEngineWindowClass";
		wc.lpszMenuName = "";
		wc.style = NULL;
		wc.lpfnWndProc = &WndProc;

		if (!::RegisterClassEx(&wc))
			assert(false && "Failed to create Win32 window class!");

		DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		if (m_WindowsSpecification.isResizable)
			windowStyle |= WS_THICKFRAME | WS_MAXIMIZEBOX;

		RECT wr = { 0,0,(LONG)m_WindowsSpecification.Width, (LONG)m_WindowsSpecification.Height };
		AdjustWindowRect(&wr, windowStyle, FALSE);


		m_WindowsHandle = ::CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, 
			"SignEngineWindowClass", 
			m_WindowsSpecification.Title.c_str(),
			windowStyle, 
			CW_USEDEFAULT, 
			CW_USEDEFAULT,
			wr.right - wr.left,
			wr.bottom - wr.top,
			nullptr, nullptr, m_InstanceHandle, this);

		if (!m_WindowsHandle)
			assert(false && "Failed to create Window Handle!");

		::ShowWindow(m_WindowsHandle, SW_SHOW);
		::UpdateWindow(m_WindowsHandle);
	}
	void Window::Destroy()
	{
		if (m_WindowsHandle) {
			DestroyWindow(m_WindowsHandle);
			m_WindowsHandle = nullptr;
		}

		if (m_InstanceHandle) {
			UnregisterClass("SignEngineWindowClass", m_InstanceHandle);
			m_InstanceHandle = nullptr;
		}
			
	}
	void Window::Update()
	{
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//window->onUpdate();

	}
	DirectX::XMFLOAT2 Window::GetFrameBufferSize() const
	{
		return DirectX::XMFLOAT2();
	}
	DirectX::XMFLOAT2 Window::GetMousePos() const
	{
		return DirectX::XMFLOAT2();
	}
	bool Window::ShouldClose()
	{
		return m_ShouldClose;
	}

	LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{
		case WM_CREATE:
			
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			break;
		default:
			return ::DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return NULL;
	}
}