#pragma once

#include <array>
#include "Sign/Renderer/Entity.h"
#include "Sign/Renderer/Primitive3D.h"
namespace Sign {
	class CubeEntity : public Entity
	{
	public:
		CubeEntity();
		CubeEntity(const std::array<Vector3D, 8>& color);

		

		// Inherited via Entity
		void OnUpdate(Timestep dt) override;
	private:
		float m_LerpTime = 0.0f;
		inline static std::shared_ptr<Mesh> s_DefaultCubeMesh = nullptr;
	};
}

