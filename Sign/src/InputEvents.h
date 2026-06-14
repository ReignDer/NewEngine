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

		inline bool IsRepeated const{ return m_IsRepeated; };
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

	class MouseButtonEvent : public Event
	{
	public:
		inline int GetMouseButton() const { return m_Button; }
	protected:
		MouseButtonEvent(int button) : m_Button(button) {}

		int m_Button;

	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			return std::format("MouseButtonPressedEvent: {})", m_Button);
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			return std::format("MouseButtonReleasedEvent: {})", m_Button);
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)

	};

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(double mouseX, double mouseY) : m_MouseX(mouseX), m_MouseY(mouseY) {}

		inline double GetMouseX() const { return m_MouseX; }
		inline double GetMouseY() const { return m_MouseY; }

		std::string ToString() const override
		{
			return std::format("MouseMovedEvent: {} {})", m_MouseX, m_MouseY);
		}

		EVENT_CLASS_TYPE(MouseMoved)
	private:
		double m_MouseX, m_MouseY;

	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(double xOffset, double yOffset) : m_XOffest(xOffset), m_YOffest(yOffset) {}

		inline double GetXOffset() const { return m_XOffest; }
		inline double GetYOffset() const { return m_YOffest; }

		std::string ToString() const override
		{
			return std::format("MouseScrolledEvent: {} {})", m_XOffest, m_YOffest);
		}

		EVENT_CLASS_TYPE(MouseScrolled)

	private:
		double m_XOffest, m_YOffest;

	};

}

