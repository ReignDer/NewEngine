#include "PerspectiveCamera.h"

namespace Sign {
	PerspectiveCamera::PerspectiveCamera(uint32_t width, uint32_t height)
	{
		SetViewPortSize(width, height);
	}

	void PerspectiveCamera::OnUpdate(float ts)
	{

		

		if (Input::IsMouseButtonPressed(Mouse::LeftButton)) {
			const DirectX::XMFLOAT2& mouse = { Input::GetMouseX(), Input::GetMouseY() };
			float deltaMouseX = mouse.x - m_InitialMousePosition.x;
			float deltaMouseY = mouse.y - m_InitialMousePosition.y;
			m_InitialMousePosition = mouse;
			m_Yaw += deltaMouseX * RotationSpeed() * ts;
			m_Pitch += deltaMouseY * RotationSpeed() * ts;

			m_Pitch = std::clamp(m_Pitch, DirectX::XMConvertToRadians(-89.5), DirectX::XMConvertToRadians(89.5));
		}
		else {
			m_InitialMousePosition = { Input::GetMouseX(), Input::GetMouseY() };
		}

		DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&m_Position);

		if (Input::IsKeyPressed(Key::W)) {
			auto forwardDir = GetForwardDirection();
			DirectX::XMVECTOR forwardVec = DirectX::XMLoadFloat3(&forwardDir);
			pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(forwardVec, MoveSpeed() * ts));
		}
		else if (Input::IsKeyPressed(Key::A)) {
			auto rightDir = GetRightDirection();
			DirectX::XMVECTOR rightVec = DirectX::XMLoadFloat3(&rightDir);
			pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(rightVec, -MoveSpeed() * ts));
			DirectX::XMFLOAT3 debug;
			DirectX::XMStoreFloat3(&debug, pos);
			std::println("{} {} {}", debug.x, debug.y, debug.z);
		}
		else if (Input::IsKeyPressed(Key::S)) {
			auto forwardDir = GetForwardDirection();
			DirectX::XMVECTOR forwardVec = DirectX::XMLoadFloat3(&forwardDir);
			pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(forwardVec, -MoveSpeed() * ts));
		}
		else if (Input::IsKeyPressed(Key::D)) {
			auto rightDir = GetRightDirection();
			DirectX::XMVECTOR rightVec = DirectX::XMLoadFloat3(&rightDir);
			pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(rightVec, MoveSpeed() * ts));
			DirectX::XMFLOAT3 debug;
			DirectX::XMStoreFloat3(&debug, pos);
			std::println("{} {} {}", debug.x, debug.y, debug.z);
		}
		DirectX::XMStoreFloat3(&m_Position, pos);
		//std::println("CameraPos: {} {}", m_Position.x, m_Position.y);
		RecalculateView();
	}

	void PerspectiveCamera::OnEvent(Event& e)
	{
	}

	void PerspectiveCamera::SetPerspective(float perspectiveFOV, float perspectiveNear, float perspectiveFar)
	{
		m_PerspectiveFOV = perspectiveFOV;
		m_PerspectiveNearClip = perspectiveNear;
		m_PerspectiveFarClip = perspectiveFar;
		RecalculateProjection();
	}

	void PerspectiveCamera::SetViewPortSize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0)
			return;
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}
	DirectX::XMFLOAT3 PerspectiveCamera::GetForwardDirection() const
	{
		auto upDir = DirectX::XMVector3Rotate(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), GetOrientation());

		DirectX::XMFLOAT3 result;
		DirectX::XMStoreFloat3(&result, upDir);
		return result;
	}
	DirectX::XMFLOAT3 PerspectiveCamera::GetRightDirection() const
	{
		auto upDir = DirectX::XMVector3Rotate(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), GetOrientation());

		DirectX::XMFLOAT3 result;
		DirectX::XMStoreFloat3(&result, upDir);
		return result;
	}
	DirectX::XMFLOAT3 PerspectiveCamera::GetUpDirection() const
	{
		auto upDir = DirectX::XMVector3Rotate(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), GetOrientation());

		DirectX::XMFLOAT3 result;
		DirectX::XMStoreFloat3(&result, upDir);
		return result;
	}
	DirectX::XMFLOAT3 PerspectiveCamera::GetPosition() const
	{
		return DirectX::XMFLOAT3();
	}
	DirectX::XMVECTOR PerspectiveCamera::GetOrientation() const
	{
		return DirectX::XMQuaternionRotationRollPitchYaw(m_Pitch, m_Yaw, 0.0f);
	}
	void PerspectiveCamera::RecalculateProjection()
	{
		if (m_AspectRatio == 0.0f)
			return;
		m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNearClip, m_PerspectiveFarClip);
	}
	void PerspectiveCamera::RecalculateView()
	{
		DirectX::XMVECTOR eyePos = DirectX::XMLoadFloat3(&m_Position);

		auto forwardDir = GetForwardDirection();
		auto upDir = GetUpDirection();
		DirectX::XMVECTOR forwardVec = DirectX::XMLoadFloat3(&forwardDir);
		DirectX::XMVECTOR target = DirectX::XMVectorAdd(eyePos, forwardVec);

		DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&upDir);
		m_ViewMatrix = DirectX::XMMatrixLookAtLH(eyePos, target, upVec);
	}

	float PerspectiveCamera::MoveSpeed()
	{
		return 1.0f;
	}
	float PerspectiveCamera::RotationSpeed()
	{
		return 0.8f;
	}
}