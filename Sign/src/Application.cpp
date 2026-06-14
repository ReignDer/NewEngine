#include "Application.h"

namespace Sign {
	static Application* s_Application = nullptr;
	Application::Application(const ApplicationSpecifications& specifications)
	{
		s_Application = this;

		if (m_Specifications.WindowSpec.Title.empty())
			m_Specifications.WindowSpec.Title = m_Specifications.name;

		m_Window = std::make_unique<Window>(m_Specifications.WindowSpec);
		m_Window->Create();
	}
	Application::~Application()
	{
		m_Window->Destroy();
		s_Application = nullptr;
	}
	void Application::Run()
	{
		m_Running = true;
		while (m_Running) {
			m_Window->Update();
		}
	}
	void Application::Stop()
	{
		m_Running = false;
	}
	Application& Application::Get()
	{
		assert(s_Application);
		return *s_Application;
	}
}