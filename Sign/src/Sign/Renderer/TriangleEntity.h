#pragma once
#include "Sign/Renderer/Entity.h"
#include "Sign/Renderer/Primitive2D.h"
namespace Sign {
	class TriangleEntity : public Entity
	{
	public:
		TriangleEntity();
		~TriangleEntity();
	private:
		inline static std::shared_ptr<Mesh> s_DefaultTriangleMesh = nullptr;

		// Inherited via Entity
		void OnUpdate(Timestep dt) override;
	};
}

