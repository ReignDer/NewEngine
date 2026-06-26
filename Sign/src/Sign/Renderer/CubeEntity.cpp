#include "CubeEntity.h"

namespace Sign {
	CubeEntity::CubeEntity()
	{
		if (s_DefaultCubeMesh == nullptr) {
			s_DefaultCubeMesh = Primitive::Cube3D::Create({
								{{ 0.0f, 1.0f, 0.0f },
								{ 1.0f, 1.0f, 0.0f },
								{ 1.0f, 0.0f, 0.0f },
								{ 0.0f, 0.0f, 1.0f },
								{ 0.0f, 1.0f, 1.0f },
								{ 1.0f, 1.0f, 1.0f },
								{ 1.0f, 0.0f, 1.0f },
								{ 1.0f, 1.0f, 1.0f },
								} });
		}
		SetMesh(s_DefaultCubeMesh);
	}
	CubeEntity::CubeEntity(const std::array<Vector3D, 8>& color)
	{
		SetMesh(Primitive::Cube3D::Create(color));
	}

	void CubeEntity::OnUpdate(Timestep dt)
	{
/*		Quaternion rotation = m_Rotation;
		rotation.x += MathUtils::ConvertToRadians(90.f) * dt;
		rotation.y += MathUtils::ConvertToRadians(90.f) * dt;
		rotation.z += MathUtils::ConvertToRadians(90.f) * dt;*/

		/*Vector3D pos = m_Translation;
		pos.x = std::sin(Time::GetTime() * 0.5) * 1.0f;
		pos.y = std::cos(Time::GetTime() * 0.5) *1.0f;*/

		/*float duration = 5.0f;
		
		m_LerpTime += dt.GetSeconds() / duration;
		m_LerpTime = std::clamp(m_LerpTime, 0.0f, 1.0f);

		auto scale = MathUtils::lerp(m_Scale, { 5.0f, 0.0001f,5.0f }, m_LerpTime);*/
		//X and Z 5x bigger

		/*std::println("Pos: {} {} {}", pos.x, pos.y, pos.z);
		std::println("Scale: {} {} {}", scale.x,scale.y,scale.z);*/
		//SetScale(scale);
		//SetRotation(rotation);
		//SetTranslation(pos);
	}
}