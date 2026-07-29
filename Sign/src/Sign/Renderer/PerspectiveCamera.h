#pragma once
#include <print>
#include <algorithm>
#include "Camera.h"
#include "Sign/Events/Event.h"
#include "Sign/Input.h"
#include "Sign/Timestep.h"
#include "Sign/Math/SignMath.h"
namespace Sign {
	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera() = default;
		PerspectiveCamera(uint32_t width, uint32_t height);
		virtual ~PerspectiveCamera() = default;

		void OnUpdate(Timestep dt);
		void OnEvent(Event& e);

		void SetPerspective(float perspectiveFOV, float perspectiveNear, float perspectiveFar);
		void SetViewPortSize(uint32_t width, uint32_t height);


		float GetPerspectiveFOV() const { return m_PerspectiveFOV; }
		void SetPerspectiveFOV(float fov) { m_PerspectiveFOV = fov; RecalculateProjection(); }
		float GetPerspectiveFarClip() const { return m_PerspectiveFarClip; }
		void SetPerspectiveFarClip(float farClip) { m_PerspectiveFarClip = farClip; RecalculateProjection(); }
		float GetPerspectiveNearClip() const { return m_PerspectiveNearClip; }
		void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNearClip = nearClip; RecalculateProjection(); }
		void ResetDragState() { m_IsDragging = false; }

		Vector3D GetForwardDirection() const;
		Vector3D GetRightDirection() const;
		Vector3D GetUpDirection() const;
		Vector3D GetPosition() const;
		Quaternion GetOrientation() const;

		const Mat4& GetViewMatrix() const{ return m_ViewMatrix; }
		Mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }
		
	private:
		void RecalculateProjection();
		void RecalculateView();

		float MoveSpeed();
		float RotationSpeed();

	private:
		Mat4 m_ViewMatrix;
		Vector3D  m_Position = { 0.0f,0.0f,0.0f };
		Vector3D  m_FocalPoint = { 0.0f,0.0f,0.0f };
		Vector2D m_InitialMousePosition = { 0.0f,0.0f };

		float m_PerspectiveFOV = DirectX::XMConvertToRadians(45.0f);
		float m_PerspectiveFarClip = 1000.0f;
		float m_PerspectiveNearClip = 0.01f;

		float m_AspectRatio = 0.0f;

		float m_Yaw = 0.0f;
		float m_Pitch = 0.0f;

		bool m_IsDragging = false;
	};
}

