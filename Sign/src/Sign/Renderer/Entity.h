#pragma once

#include <memory>
#include <string>
#include "Sign/Math/SignMath.h"
#include "Sign/Renderer/Mesh.h"
#include "Sign/Shader/Shader.h"
#include "Sign/Time.h"
#include "Sign/Timestep.h"

namespace Sign {
	enum class PrimitiveType {
		Cube,
		Plane,
		Circle,
		Sphere
	};
	class Entity
	{
	public:
		Entity(const std::string& name = "Entity");
		virtual ~Entity();

		virtual void OnUpdate(Timestep dt) = 0;
		const Mat4& GetTransform();
		const Vector3D& GetTranslation() const { return m_Translation; }
		const Vector3D& GetScale() const { return m_Scale; }
		const Quaternion& GetRotation() const { return m_Rotation; }
		const std::string GetName() const { return m_EntityName; }
		const std::shared_ptr<Mesh>& GetMesh() const{ return m_Mesh; }
		const std::shared_ptr<Shader>& GetShader() const{ return m_Shader; }
		const bool HasMesh() const{ return m_HasMesh; }

		void SetTranslation(const Vector3D& pos) { m_Translation = pos; RecalculateTransform(); }
		void SetScale(const Vector3D& scale) { m_Scale = scale; RecalculateTransform(); };
		void SetRotation(const Quaternion& rotation) { m_Rotation = rotation; RecalculateTransform(); }
		void SetMesh(const std::shared_ptr<Mesh>& mesh) { m_Mesh = mesh; m_HasMesh = true; }
		void SetShader(const std::shared_ptr<Shader>& shader) { m_Shader = shader; }

	private:
		void RecalculateTransform();
	protected:
		std::string m_EntityName;

		Vector3D m_Translation;
		Vector3D m_Scale;
		Quaternion m_Rotation;
		Mat4 m_Transform;

		bool m_Dirty = true;
		bool m_HasMesh = false;

		std::shared_ptr<Mesh> m_Mesh = nullptr;
		std::shared_ptr<Shader> m_Shader = nullptr;
	private:
		inline static std::shared_ptr<Shader> s_DefaultShader = nullptr;
	};
}

