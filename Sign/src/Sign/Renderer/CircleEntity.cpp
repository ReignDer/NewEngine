#include "CircleEntity.h"

namespace Sign {
	CircleEntity::CircleEntity()
	{
		if (s_DefaultCircleMesh == nullptr) {
			s_DefaultCircleMesh = Primitive::Circle::Create(Vector3D(1.0f, 1.0f, 2.0f), Vector3D(2.0f, 0.0f, 1.0f));
		}
		SetMesh(s_DefaultCircleMesh);
	}
	CircleEntity::~CircleEntity()
	{
		s_DefaultCircleMesh.reset();
		s_DefaultCircleMesh = nullptr;
	}
	void CircleEntity::OnUpdate(Timestep dt)
	{
	}
}
