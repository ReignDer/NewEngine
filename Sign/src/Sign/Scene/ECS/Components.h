#pragma once
#include <string>
#include "Sign/Scene/ECS/ECSTypes.h"
#include "Sign/Scene/ECS/SparseSet.h"
#include "Sign/Math/SignMath.h"
#include "Sign/Renderer/Mesh.h"
#include "Sign/Shader/Shader.h"
#include "Sign/Renderer/Texture.h"
namespace Sign {

	inline ComponentType GetNextComponentTypeID() {
		static ComponentType nextID = 0;
		return nextID++;
	}

	template<typename T>
	ComponentType GetComponentTypeID() {
		static ComponentType typeID = GetNextComponentTypeID();
		return typeID;
	}
	struct TransformComponent {
		Vector3D Translation = { 0.0f,0.0f,0.0f };
		Vector3D Rotation = { 0.0f,0.0f,0.0f};
		Vector3D Scale = { 1.0f,1.0f,1.0f };
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const Vector3D& translation) : Translation(translation) {}

		Mat4 GetTransform() const {
			Mat4 rotation = Mat4::fromQuaternion(Quaternion::FromEulerAngles(Rotation));
			return	Mat4::scale(Scale)
				* rotation
				* Mat4::translate(Translation);
		}


		EntityID m_entity = INVALID_ENTITY_ID;
	};
	
	struct MeshRendererComponent {
		std::shared_ptr<Mesh> Mesh;
		std::shared_ptr<Shader> Shader;
		std::shared_ptr<Texture2D> Texture;
		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;
		MeshRendererComponent(const std::shared_ptr<Sign::Mesh>& mesh, const std::shared_ptr<Sign::Shader>& shader, const std::shared_ptr<Texture2D>& texture = nullptr) 
			: Mesh(mesh), Shader(shader), Texture(texture) {}
		EntityID m_entity = INVALID_ENTITY_ID;
	};

	struct TagComponent {
		std::string Tag;
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(std::string_view tag) : Tag(tag) {}
		EntityID m_entity = INVALID_ENTITY_ID;
	};



}