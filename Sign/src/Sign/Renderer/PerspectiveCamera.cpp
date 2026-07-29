#include "PerspectiveCamera.h"

namespace Sign {
	PerspectiveCamera::PerspectiveCamera(uint32_t width, uint32_t height)
	{
		SetViewPortSize(width, height);
	}

	void PerspectiveCamera::OnUpdate(Timestep dt)
	{

		if (Input::IsMouseButtonPressed(Mouse::LeftButton)) {

			const Vector2D& mouse = { Input::GetMouseX(), Input::GetMouseY() };

			if (!m_IsDragging) {
				m_InitialMousePosition = mouse;
				m_IsDragging = true;
			}
			else {
				float deltaMouseX = mouse.x - m_InitialMousePosition.x;
				float deltaMouseY = mouse.y - m_InitialMousePosition.y;
				m_InitialMousePosition = mouse;
				m_Yaw += deltaMouseX * RotationSpeed() * dt;
				m_Pitch += deltaMouseY * RotationSpeed() * dt;

				m_Pitch = std::clamp(m_Pitch, MathUtils::ConvertToRadians(-89.5), MathUtils::ConvertToRadians(89.5));
			}
		}
		else {
			m_IsDragging = false;
			m_InitialMousePosition = { Input::GetMouseX(), Input::GetMouseY() };
		}

		Vector3D pos = m_Position;

		if (Input::IsKeyPressed(Key::W)) {
			auto forwardDir = GetForwardDirection();
			pos += forwardDir * MoveSpeed() * dt;
		}
		else if (Input::IsKeyPressed(Key::A)) {
			auto rightDir = GetRightDirection();
			pos += rightDir * -MoveSpeed() * dt;

			std::println("{} {} {}", pos.x, pos.y, pos.z);
		}
		else if (Input::IsKeyPressed(Key::S)) {
			auto forwardDir = GetForwardDirection();
			pos += forwardDir * -MoveSpeed() * dt;
		}
		else if (Input::IsKeyPressed(Key::D)) {
			auto rightDir = GetRightDirection();
			pos += rightDir * MoveSpeed() * dt;
			
			std::println("{} {} {}", pos.x, pos.y, pos.z);
		}
		m_Position = pos;
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
	Vector3D PerspectiveCamera::GetForwardDirection() const
	{
		auto forwardDir = GetOrientation().rotate(Vector3D(0.0f, 0.0f, 1.0f));

		return forwardDir;
	}
	Vector3D PerspectiveCamera::GetRightDirection() const
	{
		auto rightDir = GetOrientation().rotate(Vector3D(1.0f, 0.0f, 0.0f));

		return rightDir;
	}
	Vector3D PerspectiveCamera::GetUpDirection() const
	{
		auto upDir = GetOrientation().rotate(Vector3D(0.0f,1.0f,0.0f));

		return upDir;
	}
	Vector3D PerspectiveCamera::GetPosition() const
	{
		return Vector3D();
	}
	Quaternion PerspectiveCamera::GetOrientation() const
	{
		return Quaternion::QuaternionRotationPitchYawRoll(m_Pitch, m_Yaw, 0.0f);
	}
	void PerspectiveCamera::RecalculateProjection()
	{
		if (m_AspectRatio == 0.0f)
			return;
		m_ProjectionMatrix = Mat4::perspectiveFovLH(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNearClip, m_PerspectiveFarClip);
	}
	void PerspectiveCamera::RecalculateView()
	{
		Vector3D eyePos = m_Position;

		auto forwardDir = GetForwardDirection();
		auto upDir = GetUpDirection();
		Vector3D target = eyePos + forwardDir;

		m_ViewMatrix = Mat4::lookAtLH(eyePos, target, upDir);
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