#pragma once
#include <memory>
#include <string>
#include "Window.h"
#include "Layer.h"


namespace Sign {
	struct ApplicationSpecifications {
		std::string name = "Application";
		WindowSpecifications WindowSpec;
	};
	class Application
	{
	public:
		Application(const ApplicationSpecifications& specifications = ApplicationSpecifications());
		~Application();

		void PushLayer(Layer* layer);

		void Run();
		void Stop();

		Window& GetWindow() { return *m_Window; }


		static Application& Get();
	private:
		ApplicationSpecifications m_Specifications;
		std::shared_ptr<Window> m_Window;
		bool m_Running = false;
	};
}

