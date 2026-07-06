#pragma once
#include <memory>
#include <string>
#include <ranges>
#include <print>

#include <directx/d3d12.h>
#include <wrl.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>
#include <directx/d3d12sdklayers.h>

#include "Window.h"
#include "LayerStack.h"
#include "Events/Event.h"
#include "Events/WindowEvents.h"
#include "Renderer/Renderer.h"
#include "ImGui/ImGuiLayer.h"
#include "Sign/Time.h"
#include "Timestep.h"



namespace Sign {
	
	struct ApplicationSpecifications {
		std::string name = "Application";
		float targetFPS = 0.0f;
		WindowSpecifications WindowSpec;
	};
	class Application
	{
	public:
		Application(const ApplicationSpecifications& specifications = ApplicationSpecifications());
		~Application();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		void Run();
		void Stop();

		void RaiseEvent(Event& event);
		Window& GetWindow() { return *m_Window; }

		bool OnWindowClosedEvent(WindowClosedEvent& e);
		bool OnWindowResizedEvent(WindowResizedEvent& e);


		static Application& Get();
	private:
		ApplicationSpecifications m_Specifications;
		std::shared_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;
		

		float m_LastFrameTime = 0.0f;
		bool m_Running = false;
	};
}

