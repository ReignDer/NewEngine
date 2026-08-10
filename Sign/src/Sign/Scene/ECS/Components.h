#pragma once
#include <string>
#include <reactphysics3d/reactphysics3d.h>
#include "Sign/Scene/ECS/ECSTypes.h"
#include "Sign/Scene/ECS/SparseSet.h"
#include "Sign/Math/SignMath.h"
#include "Sign/Renderer/Mesh.h"
#include "Sign/Shader/Shader.h"
#include "Sign/Renderer/Texture.h"
#include "Sign/UUID.h"
#include "Sign/Asset/Asset.h"
#include "Sign/Renderer/SceneCamera.h"

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

	struct IDComponent {
		UUID ID;
		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(const UUID& id) : ID(id) {}

		EntityID m_entity = INVALID_ENTITY_ID;
	};

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
		enum class SourceType {Asset = 0, Primitive};
		enum class PrimitiveType {None = 0, Cube, Sphere, Plane};
		std::shared_ptr<Mesh> Mesh;
		AssetHandle MeshA = 0;
		AssetHandle TextureA = 0;
		std::shared_ptr<Shader> Shader;
		std::shared_ptr<Texture2D> Texture;
		SourceType Type = SourceType::Primitive;
		PrimitiveType PType = PrimitiveType::None;
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

	struct RigidBody3D {
		enum class BodyType{ Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		RigidBody3D() = default;
		RigidBody3D(const RigidBody3D&) = default;

		void* RuntimeBody = nullptr;
		EntityID m_entity = INVALID_ENTITY_ID;
	};

	struct Box3DColliderComponent {
		Vector3D Offset = { 0.0f,0.0f,0.0f };
		Vector3D Size = { 0.5f, 0.5f ,0.5f};

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.5f;
		float RestitutionThreshold = 0.5f;
		Box3DColliderComponent() = default;
		Box3DColliderComponent(const Box3DColliderComponent&) = default;

		void* RuntimeFixture = nullptr;
		EntityID m_entity = INVALID_ENTITY_ID;
	};

	struct SphereColliderComponent
	{
		Vector3D Offset = { 0.0f,0.0f,0.0f };
		float Radius =  0.5f;

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.5f;
		float RestitutionThreshold = 0.5f;
		SphereColliderComponent() = default;
		SphereColliderComponent(const SphereColliderComponent&) = default;

		void* RuntimeFixture = nullptr;
		EntityID m_entity = INVALID_ENTITY_ID;
	};

	struct LightComponent
	{
		enum class LightType{Directional, Spot, Point };

		LightType Type = LightType::Directional;

		Vector3D Color = { 1.0f,1.0f,1.0f };

		float Intensity = 1.f;

		float Range = 10.f;

		float InnerConeAngle = 12.5f;
		float OuterConeAngle = 17.5f;

		LightComponent() = default;
		LightComponent(const LightComponent&) = default;

		EntityID m_entity = INVALID_ENTITY_ID;
	};
	
	struct CameraComponent
	{
		SceneCamera sceneCamera;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;

		EntityID m_entity = INVALID_ENTITY_ID;
	};
	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<TransformComponent, MeshRendererComponent, RigidBody3D, Box3DColliderComponent, SphereColliderComponent, LightComponent, CameraComponent>;

}