#include "Window.h"
#include "backends/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
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

		m_Context = std::make_unique<D3D12Context>(m_WindowsHandle,m_WindowsSpecification.Width,m_WindowsSpecification.Height);
		m_Context->Init();

		::SetWindowLongPtr(m_WindowsHandle, GWLP_USERDATA, (LONG_PTR)this);
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
		std::println("Window Destroyed");
	}
	void Window::Update()
	{
		m_Context->SwapBuffers();

	}
	void Window::PollEvents()
	{
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	
	Vector2D Window::GetFrameBufferSize() const
	{
		RECT clientRect;
		if (GetClientRect(m_WindowsHandle, &clientRect)) {
			float width = static_cast<float>(clientRect.right - clientRect.left);
			float height = static_cast<float>(clientRect.bottom - clientRect.top);

			return Vector2D(width, height);
		}
		return Vector2D(0.0f,0.0f);
	}
	Vector2D Window::GetMousePos() const
	{
		return Vector2D();
	}
	bool Window::ShouldClose()
	{
		return m_ShouldClose;
	}



	LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		Window* window = (Window*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
			return true;
		switch (msg)
		{
		case WM_CLOSE: {
			window->m_ShouldClose = true;
			WindowClosedEvent e;
			window->m_WindowsSpecification.EventCallback(e);
			return 0;
		}
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;

		case WM_KEYDOWN:
		{
			bool isRepeat = (lparam & 0x40000000);
			KeyPressedEvent e((int)wparam, isRepeat);
			
			window->m_WindowsSpecification.EventCallback(e);
			return 0;
		}
		case WM_KEYUP:
		{
			KeyReleasedEvent e((int)wparam);
			window->m_WindowsSpecification.EventCallback(e);
			return 0;
		}

		case WM_SIZE:
		{
			uint32_t width = LOWORD(lparam);
			uint32_t height = HIWORD(lparam);

			if (window && window->m_Context) {
				window->m_WindowsSpecification.Width = width;
				window->m_WindowsSpecification.Height = height;
				window->m_PendingResize = true;
			}

			return 0;
		}
		case WM_EXITSIZEMOVE:  // fires ONCE when user releases the window edge
		{
			if (window && window->m_PendingResize) {
				WindowResizedEvent e(window->m_WindowsSpecification.Width, window->m_WindowsSpecification.Height);
				window->m_WindowsSpecification.EventCallback(e);
				window->m_PendingResize = false;
			}
			return 0;
		}
		case WM_MBUTTONDOWN:
		{
			MouseButtonPressedEvent e((int)wparam);
			window->m_WindowsSpecification.EventCallback(e);
			return 0;
		}
		default:
			return ::DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return NULL;
	}
}