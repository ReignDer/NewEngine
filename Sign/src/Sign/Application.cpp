#include "Application.h"

namespace Sign {
	static Application* s_Application = nullptr;
	Application::Application(const ApplicationSpecifications& specifications)
	{
		s_Application = this;
#if defined(SIGN_DEBUG)
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			std::cout << "Hello";

			Microsoft::WRL::ComPtr<ID3D12Debug1> debugController1;
			if (SUCCEEDED(debugController.As(&debugController1)))
			{
				debugController1->SetEnableGPUBasedValidation(true);
			}
		}
#endif
		m_Specifications = specifications;
		if (m_Specifications.WindowSpec.Title.empty())
			m_Specifications.WindowSpec.Title = m_Specifications.name;

		m_Specifications.WindowSpec.EventCallback = [this](Event& event) {return RaiseEvent(event); };

		
		m_Window = std::make_unique<Window>(m_Specifications.WindowSpec);
		m_Window->Create();

		Renderer::Init(m_Window->GetContext());
		Renderer::Resizebuffers(m_Window->GetFrameBufferSize().x, m_Window->GetFrameBufferSize().y);
		Renderer::BeginInitializationCommand();
		m_ImGuiLayer = new ImGuiLayer;
		PushOverlay(m_ImGuiLayer);

	}
	Application::~Application()
	{
		m_LayerStack.Clear();
		Renderer::ShutDown();
		m_Window.reset();
		std::println("App Destroyed");
		s_Application = nullptr;
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::Run()
	{
		Renderer::EndInitializationCommand();

		m_Running = true;
		while (m_Running) {
			auto currentTime = Time::GetTime();
			Timestep deltaTime = currentTime - m_LastFrameTime;

			m_LastFrameTime = currentTime;

			m_Window->PollEvents();


			for (const auto& layer : m_LayerStack) {
				layer->OnUpdate(deltaTime);
			}


			//If Multi-threaded, implement Rendering in OnUpdate to solve asynchronous problems like race conditions
			for (const auto& layer : m_LayerStack) {
				layer->OnRender();
			}

			m_ImGuiLayer->Begin();

			for (const auto& layer : m_LayerStack) {
				layer->OnImGuiRender();
			}

			m_ImGuiLayer->End();

			m_Window->Update();

		}
	}
	void Application::Stop()
	{
		m_Running = false;
	}
	void Application::RaiseEvent(Event& event)
	{
		std::println("{}", event.ToString());
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowClosedEvent>([this](WindowClosedEvent& event) {return OnWindowClosedEvent(event); });
		dispatcher.Dispatch<WindowResizedEvent>([this](WindowResizedEvent& event) {return OnWindowResizedEvent(event); });

		for (const auto& layer : std::views::reverse(m_LayerStack)) {
			layer->OnEvent(event);
			if (event.m_Handled)
				break;
		}
	}

	bool Application::OnWindowClosedEvent(WindowClosedEvent& e)
	{
		m_Running = false;
		return m_Running;
	}

	bool Application::OnWindowResizedEvent(WindowResizedEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0) {
			return false;
		}
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		return false;
	}

	Application& Application::Get()
	{
		assert(s_Application);
		return *s_Application;
	}
}