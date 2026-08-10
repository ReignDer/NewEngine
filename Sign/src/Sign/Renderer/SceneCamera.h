#pragma once

#include "Camera.h"
#include "Sign/Math/MathUtils.h"
namespace Sign {
	class SceneCamera : public Camera
	{
	public:
		SceneCamera() = default;
		SceneCamera(uint32_t width, uint32_t height);
		virtual ~SceneCamera() = default;

		void SetPerspective(float perspectiveFOV, float perspectiveNear, float perspectiveFar);
		void SetViewPortSize(uint32_t width, uint32_t height);


		float GetPerspectiveFOV() const { return m_PerspectiveFOV; }
		void SetPerspectiveFOV(float fov) { m_PerspectiveFOV = fov; RecalculateProjection(); }
		float GetPerspectiveFarClip() const { return m_PerspectiveFarClip; }
		void SetPerspectiveFarClip(float farClip) { m_PerspectiveFarClip = farClip; RecalculateProjection(); }
		float GetPerspectiveNearClip() const { return m_PerspectiveNearClip; }
		void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNearClip = nearClip; RecalculateProjection(); }

		Vector3D GetForwardDirection() const;
		Vector3D GetRightDirection() const;
		Vector3D GetUpDirection() const;
		Vector3D GetPosition() const;
		Quaternion GetOrientation() const;

		const Mat4& GetViewMatrix() const { return m_ViewMatrix; }
		Mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }
	private:
		void RecalculateProjection();
		void RecalculateView();
		private:
		Mat4 m_ViewMatrix;
		Vector3D  m_Position = { 0.0f,0.0f,0.0f };
		Vector3D  m_FocalPoint = { 0.0f,0.0f,0.0f };
		Vector2D m_InitialMousePosition = { 0.0f,0.0f };

		float m_PerspectiveFOV = MathUtils::ConvertToRadians(45.0f);
		float m_PerspectiveFarClip = 1000.0f;
		float m_PerspectiveNearClip = 0.01f;

		float m_AspectRatio = 0.0f;

		float m_Yaw = 0.0f;
		float m_Pitch = 0.0f;
	};
}

