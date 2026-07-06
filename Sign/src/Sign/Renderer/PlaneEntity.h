#pragma once
#include "Sign/Renderer/Entity.h"
#include "Sign/Renderer/Primitive3D.h"
namespace Sign {
	class PlaneEntity : public Entity
	{
	public:
		PlaneEntity();
		~PlaneEntity();


		// Inherited via Entity
		void OnUpdate(Timestep dt) override;
	private:
		inline static std::shared_ptr<Mesh> s_DefaultPlaneMesh = nullptr;

	};
}

