#include "Entity.h"

namespace Sign {
	Entity::Entity(const std::string& name) 
		: m_EntityName(name), 
		m_Translation(Vector3D(0.0f,0.0f,0.0f)), 
		m_Scale(Vector3D(1.0f,1.0f,1.0f)), 
		m_Rotation(Quaternion(0.0f,0.0f,0.0f,0.0f))
	{
	
		RecalculateTransform();
	}
	Entity::~Entity()
	{
		s_DefaultShader.reset();
	}
	const Mat4& Entity::GetTransform()
	{
		return m_Transform;
	}
	void Entity::RecalculateTransform()
	{
		m_Transform = Mat4::scale(m_Scale)
			* Mat4::rotateX(m_Rotation.x)
			* Mat4::rotateY(m_Rotation.y)
			* Mat4::rotateZ(m_Rotation.z)
			* Mat4::translate(m_Translation);
	}
}