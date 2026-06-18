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
		ScreenToClient(Application::Get().GetWindow().GetHandle(), &p);

		/*RECT rect;
		GetClientRect(Application::Get().GetWindow().GetHandle(), &rect);

		if (p.x > rect.right || p.x < rect.left
			|| p.y < rect.top || p.y > rect.bottom) {
			return { 0.0f,0.0f };
		}*/
		return { (float)p.x,(float)p.y };
	}

	float Input::GetMouseX() {

		return GetMousePos().x;
	}

	float Input::GetMouseY() {

		return GetMousePos().y;
	}
}