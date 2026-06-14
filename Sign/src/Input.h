#pragma once
#include "KeyCodes.h"
#include "MouseCodes.h"
#include <DirectXMath.h>
namespace Sign {
	class Input {

	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static DirectX::XMFLOAT2 GetMousePos();
		static float GetMouseX();
		static float GetMouseY();

	};
}