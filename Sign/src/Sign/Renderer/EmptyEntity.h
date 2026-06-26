#pragma once

#include "Sign/Renderer/Entity.h"

namespace Sign {
	class EmptyEntity : public Entity
	{
	public:
		EmptyEntity() {};
		virtual ~EmptyEntity() = default;

		// Inherited via Entity
		void OnUpdate(Timestep dt) override;
	};
}

