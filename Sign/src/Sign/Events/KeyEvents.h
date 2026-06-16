#pragma once

#include "Event.h"
#include <format>
namespace Sign {
	class KeyEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return m_KeyCode; }

	protected:
		KeyEvent(int keyCode) : m_KeyCode(keyCode) {}

		int m_KeyCode;
	};


	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keyCode, bool isRepeated) : KeyEvent(keyCode), m_IsRepeated(isRepeated) {}

		inline bool IsRepeated() const { return m_IsRepeated; };
		std::string ToString() const override
		{
			return std::format("KeyPressedEvent: {} (repeat={})", m_KeyCode, m_IsRepeated);
		}

		EVENT_CLASS_TYPE(KeyPressed)
		
	private:
		bool m_IsRepeated;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keyCode) : KeyEvent(keyCode) {}

		std::string ToString() const override
		{
			return std::format("KeyPressedEvent: {})", m_KeyCode);
		}

		EVENT_CLASS_TYPE(KeyReleased)

	};

	

}

