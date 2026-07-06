#pragma once

#include "Sign/Renderer/Entity.h"
#include "Sign/Renderer/Primitive3D.h"

namespace Sign {
	class SphereEntity : public Entity
	{
	public:
		SphereEntity();
		~SphereEntity();

		// Inherited via Entity
		void OnUpdate(Timestep dt) override;

	private:
		inline static std::shared_ptr<Mesh> s_DefaultSphereMesh = nullptr;
	};
}

