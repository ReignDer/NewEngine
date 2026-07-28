#pragma once
#include <string>
#include <reactphysics3d/reactphysics3d.h>
#include "Sign/Scene/ECS/Registry.h"
#include "Sign/Scene/ECS/Components.h"
#include "Sign/Renderer/Renderer.h"
#include "Sign/Physics/PhysicsUtils.h"

namespace Sign {
	class EntityECS;
	class Scene
	{
	public:
		Scene();
		~Scene();

		EntityECS CreateEntity(std::string_view name = std::string());

		void DestroyEntity(EntityECS entity);

		Registry& GetRegistry() { return m_Registry; }

		uint32_t GetSelectedFaceID() const { return m_SelectedFaceID; }
	//Make render Scene private in the FUTURE
	public:
		void OnUpdateRuntime(Timestep ts);
		void RenderScene(EntityID selectedEntity, uint32_t selectedFaceID);

	public:
		void OnPhysics3DStart();
		void OnPhysics3DStop();
	private:
		Registry m_Registry;

		reactphysics3d::PhysicsCommon m_PhysicsCommon;
		reactphysics3d::PhysicsWorld* m_PhysicsWorld = nullptr;

		uint32_t m_SelectedFaceID;
		friend class EntityECS;
		friend class SceneHierarchy;
	};
}

