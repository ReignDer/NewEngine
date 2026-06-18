#pragma once
#include <print>
#include <algorithm>
#include "Camera.h"
#include "Sign/Events/Event.h"
#include "Sign/Input.h"
namespace Sign {
	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera() = default;
		PerspectiveCamera(uint32_t width, uint32_t height);
		virtual ~PerspectiveCamera() = default;

		void OnUpdate(float ts);
		void OnEvent(Event& e);

		void SetPerspective(float perspectiveFOV, float perspectiveNear, float perspectiveFar);
		void SetViewPortSize(uint32_t width, uint32_t height);


		float GetPerspectiveFOV() const { return m_PerspectiveFOV; }
		void SetPerspectiveFOV(float fov) { m_PerspectiveFOV = fov; RecalculateProjection(); }
		float GetPerspectiveFarClip() const { return m_PerspectiveFarClip; }
		void SetPerspectiveFarClip(float farClip) { m_PerspectiveFarClip = farClip; RecalculateProjection(); }
		float GetPerspectiveNearClip() const { return m_PerspectiveNearClip; }
		void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNearClip = nearClip; RecalculateProjection(); }

		DirectX::XMFLOAT3 GetForwardDirection() const;
		DirectX::XMFLOAT3 GetRightDirection() const;
		DirectX::XMFLOAT3 GetUpDirection() const;
		DirectX::XMFLOAT3 GetPosition() const;
		DirectX::XMVECTOR GetOrientation() const;

		const DirectX::XMMATRIX& GetViewMatrix() const{ return m_ViewMatrix; }
		DirectX::XMMATRIX GetViewProjectionMatrix() const { return DirectX::XMMatrixMultiply(m_ProjectionMatrix, m_ViewMatrix); }
		
	private:
		void RecalculateProjection();
		void RecalculateView();

		float MoveSpeed();
		float RotationSpeed();

	private:
		DirectX::XMMATRIX m_ViewMatrix;
		DirectX::XMFLOAT3  m_Position = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT3  m_FocalPoint = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT2 m_InitialMousePosition = { 0.0f,0.0f };

		float m_PerspectiveFOV = DirectX::XMConvertToRadians(45.0f);
		float m_PerspectiveFarClip = 1000.0f;
		float m_PerspectiveNearClip = 0.01f;

		float m_AspectRatio = 0.0f;

		float m_Yaw = 0.0f;
		float m_Pitch = 0.0f;
	};
}

