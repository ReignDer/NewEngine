#pragma once
#include <vector>
#include <ranges>
#include "Layer.h"
namespace Sign {
	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack();
		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		void PushOverlay(Layer* layer);
		void PopOverlay(Layer* layer);

		void Clear();

		auto begin() { return m_LayerStack.begin(); }
		auto end() { return m_LayerStack.end(); }
		auto rbegin() { return m_LayerStack.rbegin(); }
		auto rend() { return m_LayerStack.rend(); }

		auto begin() const { return m_LayerStack.begin(); }
		auto end()	const { return m_LayerStack.end(); }
		auto rbegin() const { return m_LayerStack.rbegin(); }
		auto rend() const { return m_LayerStack.rend(); }
	private:
		std::vector<std::unique_ptr<Layer>> m_LayerStack;
		int m_LayerInsertIndex = 0;
	};
}
