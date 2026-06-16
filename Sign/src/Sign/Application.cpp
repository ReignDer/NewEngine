#include "Application.h"

namespace Sign {
	static Application* s_Application = nullptr;
	Application::Application(const ApplicationSpecifications& specifications)
	{
		s_Application = this;

		if (m_Specifications.WindowSpec.Title.empty())
			m_Specifications.WindowSpec.Title = m_Specifications.name;

		m_Specifications.WindowSpec.EventCallback = [this](Event& event) {return RaiseEvent(event); };

		m_Window = std::make_unique<Window>(m_Specifications.WindowSpec);
		m_Window->Create();
		Renderer::Init(m_Window->GetContext());

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

	}
	Application::~Application()
	{
		std::println("App Destroyed");
		m_Window->Destroy();
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
		m_Running = true;
		while (m_Running) {

			m_Window->PollEvents();

			float ts = 0;
			for (const auto& layer : m_LayerStack) {
				layer->OnUpdate(ts);
			}

			for (const auto& layer : m_LayerStack) {
				layer->OnRender();
			}

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
		Renderer::OnWindowResize();
		return false;
	}

	Application& Application::Get()
	{
		assert(s_Application);
		return *s_Application;
	}
}