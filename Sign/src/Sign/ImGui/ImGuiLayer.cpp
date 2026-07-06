#include "signpch.h"
#include "Sign/Application.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx12.h"
#include "ImGuiLayer.h"


namespace Sign {
	ImGuiLayer::ImGuiLayer()
	{
	}
	ImGuiLayer::~ImGuiLayer()
	{
       
	}
	void ImGuiLayer::OnAttach()
	{
        ImGui_ImplWin32_EnableDpiAwareness();
        float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // Setup scaling
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
        style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
        io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
        io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        auto handle = Application::Get().GetWindow().GetHandle();
        ImGui_ImplWin32_Init(handle);
        auto context = Renderer::GetContext();
        auto device = context->GetDevice();
        auto srvHeap = context->Get_CBV_SRV_UAV_DescriptorHeap().Get();

        //Using Legacy for now
/*        ImGui_ImplDX12_Init(device.Get(),
            D3D12Utils::g_NumFrames,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            srvHeap,
            srvHeap->GetCPUDescriptorHandleForHeapStart(),
            srvHeap->GetGPUDescriptorHandleForHeapStart()
        );*/
        
        ImGui_ImplDX12_InitInfo init_info = {};
        init_info.Device = device.Get();
        init_info.CommandQueue = context->GetCommandQueue()->GetCommandQueue().Get();
        init_info.NumFramesInFlight = D3D12Utils::g_NumFrames;
        init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;
        // Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
        // (current version of the backend will only allocate one descriptor, future versions will need to allocate more)
        init_info.SrvDescriptorHeap = srvHeap;
        init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) { return Renderer::GetContext()->Get_CBV_SRV_UAV_Allocator().Alloc(out_cpu_handle, out_gpu_handle); };
        init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) {
            auto context = Renderer::GetContext();
            uint64_t currentFenceValue = context->GetCurrentFrameFenceValue();
            return Renderer::GetContext()->Get_CBV_SRV_UAV_Allocator().Free(cpu_handle, gpu_handle, currentFenceValue); };
        ImGui_ImplDX12_Init(&init_info);

        // Before 1.91.6: our signature was using a single descriptor. From 1.92, specifying SrvDescriptorAllocFn/SrvDescriptorFreeFn will be required to benefit from new features.
        //ImGui_ImplDX12_Init(g_pd3dDevice, APP_NUM_FRAMES_IN_FLIGHT, DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap, g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(), g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

        // Load Fonts
        // - If fonts are not explicitly loaded, Dear ImGui will select an embedded font: either AddFontDefaultVector() or AddFontDefaultBitmap().
        //   This selection is based on (style.FontSizeBase * style.FontScaleMain * style.FontScaleDpi) reaching a small threshold.
        // - You can load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        // - If a file cannot be loaded, AddFont functions will return a nullptr. Please handle those errors in your code (e.g. use an assertion, display an error and quit).
        // - Read 'docs/FONTS.md' for more instructions and details.
        // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use FreeType for higher quality font rendering.
        // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
        //style.FontSizeBase = 20.0f;
        //io.Fonts->AddFontDefaultVector();
        //io.Fonts->AddFontDefaultBitmap();
        //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
        //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
        //IM_ASSERT(font != nullptr);

	}
	void ImGuiLayer::OnDettach()
	{
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
	}
    void ImGuiLayer::OnImGuiRender()
    {
    }
/*	void ImGuiLayer::OnImGuiRender()
	{
        bool show = true;
        ImGui::ShowDemoWindow(&show);
	}*/
    void ImGuiLayer::OnEvent(Event& event)
    {
        if (m_BlockEvents) {
            ImGuiIO& io = ImGui::GetIO();
            switch (event.GetEventType()) {
            case EventType::MouseButtonPressed:
            case EventType::MouseButtonReleased:
            case EventType::MouseMoved:
            case EventType::MouseScroll:
                event.m_Handled |= io.WantCaptureMouse;
                break;
            case EventType::KeyPressed:
            case EventType::KeyReleased:
                event.m_Handled |= io.WantCaptureKeyboard;
                break;
            default:
                break;
            }
        }
    }
	void ImGuiLayer::Begin()
	{
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
	}
    void ImGuiLayer::End()
    {
        ImGui::Render();
        ImGuiIO& io = ImGui::GetIO();

        auto context = Renderer::GetContext();
        auto commandQueue = context->GetCommandQueue();
        auto uiCommandList = commandQueue->GetCommandList();

        ID3D12DescriptorHeap* heaps[] = { context->Get_CBV_SRV_UAV_DescriptorHeap().Get() };
        uiCommandList->SetDescriptorHeaps(_countof(heaps), heaps);

        auto backBuffer = context->GetBackBuffer(context->GetCurrentBackBuffer());
        auto rtv = context->GetCurrentTargetView();

        D3D12Utils::TransitionResource(
            uiCommandList,
            backBuffer,
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET);

        uiCommandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);


        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), uiCommandList.Get());

       
        {
            D3D12Utils::TransitionResource(
                uiCommandList,
                backBuffer,
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PRESENT);

            uint64_t fenceValue = commandQueue->ExecuteCommandList(uiCommandList);
            context->SetFrameFenceValues(fenceValue);


            // Update and Render additional Platform Windows
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }

        }
    }
}
