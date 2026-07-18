#include "LayerStack.h"

Sign::LayerStack::~LayerStack()
{

}

void Sign::LayerStack::PushLayer(Layer* layer)
{
	m_LayerStack.emplace(m_LayerStack.begin() + m_LayerInsertIndex, layer);
	m_LayerInsertIndex++;
}

void Sign::LayerStack::PopLayer(Layer* layer)
{
	auto currentLayerIt = std::find_if(m_LayerStack.begin(), m_LayerStack.begin() + m_LayerInsertIndex, [layer](const std::unique_ptr<Layer>& ptr) {return ptr.get() == layer; });
	if (currentLayerIt != m_LayerStack.begin() + m_LayerInsertIndex)
	{
		layer->OnDettach();
		m_LayerStack.erase(currentLayerIt);
		m_LayerInsertIndex--;
	}
}

void Sign::LayerStack::PushOverlay(Layer* layer)
{
	m_LayerStack.emplace_back(layer);
}

void Sign::LayerStack::PopOverlay(Layer* layer)
{
	auto currentLayerIt = std::find_if(m_LayerStack.begin() + m_LayerInsertIndex, m_LayerStack.end(), [layer](const std::unique_ptr<Layer>& ptr) {return ptr.get() == layer; });
	if (currentLayerIt != m_LayerStack.end())
	{
		layer->OnDettach();
		m_LayerStack.erase(currentLayerIt);
	}
}

void Sign::LayerStack::Clear()
{
	for (const auto& layer : m_LayerStack) {
		layer->OnDettach();
	}
}
