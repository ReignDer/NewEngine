#pragma once
#include <DirectXMath.h>
namespace Sign {
	class Camera
	{
	public:
		Camera() = default;
		Camera(const DirectX::XMMATRIX& projectionMatrix) : m_ProjectionMatrix(projectionMatrix) {}
		virtual ~Camera() = default;

		const DirectX::XMMATRIX& GetProjectionMatrix() const {return m_ProjectionMatrix;}

	protected:
		DirectX::XMMATRIX m_ProjectionMatrix = DirectX::XMMatrixIdentity();
	};
}

