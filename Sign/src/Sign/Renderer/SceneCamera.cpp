#include "signpch.h"
#include "SceneCamera.h"

namespace Sign {
	SceneCamera::SceneCamera(uint32_t width, uint32_t height)
	{
		RecalculateProjection();
	}
	void SceneCamera::SetPerspective(float perspectiveFOV, float perspectiveNear, float perspectiveFar)
	{
		m_PerspectiveFOV = perspectiveFOV;
		m_PerspectiveNearClip = perspectiveNear;
		m_PerspectiveFarClip = perspectiveFar;
		RecalculateProjection();
	}

	void SceneCamera::SetViewPortSize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0)
			return;
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}
	Vector3D SceneCamera::GetForwardDirection() const
	{
		auto forwardDir = GetOrientation().rotate(Vector3D(0.0f, 0.0f, 1.0f));

		return forwardDir;
	}
	Vector3D SceneCamera::GetRightDirection() const
	{
		auto rightDir = GetOrientation().rotate(Vector3D(1.0f, 0.0f, 0.0f));

		return rightDir;
	}
	Vector3D SceneCamera::GetUpDirection() const
	{
		auto upDir = GetOrientation().rotate(Vector3D(0.0f, 1.0f, 0.0f));

		return upDir;
	}
	Vector3D SceneCamera::GetPosition() const
	{
		return Vector3D();
	}
	Quaternion SceneCamera::GetOrientation() const
	{
		return Quaternion::QuaternionRotationPitchYawRoll(m_Pitch, m_Yaw, 0.0f);
	}
	void SceneCamera::RecalculateProjection()
	{
		if (m_AspectRatio == 0.0f)
			return;
		m_ProjectionMatrix = Mat4::perspectiveFovLH(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNearClip, m_PerspectiveFarClip);
	}
	void SceneCamera::RecalculateView()
	{
		Vector3D eyePos = m_Position;

		auto forwardDir = GetForwardDirection();
		auto upDir = GetUpDirection();
		Vector3D target = eyePos + forwardDir;

		m_ViewMatrix = Mat4::lookAtLH(eyePos, target, upDir);
	}
}