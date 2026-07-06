#include "PlaneEntity.h"

namespace Sign {
	PlaneEntity::PlaneEntity()
	{
		if (s_DefaultPlaneMesh == nullptr) {
			s_DefaultPlaneMesh = Primitive::Plane::Create();
		}
		SetMesh(s_DefaultPlaneMesh);
		SetScale(Vector3D(10.f, 1.f, 10.0f));
	}
	PlaneEntity::~PlaneEntity()
	{
		s_DefaultPlaneMesh.reset();
	}
	void PlaneEntity::OnUpdate(Timestep dt)
	{

	}
}