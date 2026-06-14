#pragma once
#include <string>
#include <format>

#include "Event.h"

namespace Sign {
	class WindowClosedEvent : public Event
	{
	public:
		WindowClosedEvent() {}

		EVENT_CLASS_TYPE(WindowClosed)
	};

	class WindowResizedEvent : public Event
	{
	public:
		WindowResizedEvent(uint32_t width, uint32_t height) : m_Width(width), m_Height(height) {}

		inline uint32_t GetWidth() const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height; }

		std::string ToString() const override {
			std::format("WindowResizedEvent: {} {}", m_Width, m_Height);
		}

	private:
		uint32_t m_Width, m_Height;
	};
}

