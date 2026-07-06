#pragma once
#include "Sign/Renderer/Entity.h"
#include "Sign/Renderer/Primitive2D.h"
namespace Sign {
	class CircleEntity : public Entity
	{
	public:
		CircleEntity();
		~CircleEntity();
		// Inherited via Entity
		void OnUpdate(Timestep dt) override;
	private:
		inline static std::shared_ptr<Mesh> s_DefaultCircleMesh = nullptr;
	};
}

