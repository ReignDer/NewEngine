#include "Window.h"
#include "Input.h"

#include "Application.h"
namespace Sign {
	bool Input::IsKeyPressed(const KeyCode keyCode) {
		if (GetForegroundWindow() != Application::Get().GetWindow().GetHandle()) return false;
		return (GetAsyncKeyState(static_cast<int>(keyCode)) & 0x8000) != 0;
	}

	bool Input::IsMouseButtonPressed(const MouseCode button) {
		if (GetForegroundWindow() != Application::Get().GetWindow().GetHandle()) return false;
		return (GetAsyncKeyState(static_cast<int>(button)) & 0x8000) != 0;
	}

	DirectX::XMFLOAT2 Input::GetMousePos() {
		if (GetForegroundWindow() != Application::Get().GetWindow().GetHandle()) return { 0.0f,0.0f };

		POINT p;
		GetCursorPos(&p);
		return { (float)p.x,(float)p.y };
	}

	float Input::GetMouseX() {

		return GetMousePos().x;
	}

	float Input::GetMouseY() {

		return GetMousePos().y;
	}
}