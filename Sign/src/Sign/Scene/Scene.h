#pragma once
#include <string>
#include <reactphysics3d/reactphysics3d.h>
#include "Sign/Scene/ECS/Registry.h"
#include "Sign/Scene/ECS/Components.h"
#include "Sign/Renderer/Renderer.h"
#include "Sign/Physics/PhysicsUtils.h"
#include "Sign/UUID.h"
#include "Sign/Renderer/PerspectiveCamera.h"
namespace Sign {

	class EntityECS;
	class Scene
	{
	public:
		Scene();
		~Scene();

		static std::shared_ptr<Scene> Copy(std::shared_ptr<Scene> other);

		EntityECS CreateEntity(std::string_view name = std::string());
		EntityECS CreateEntityWithUUID(UUID uuid, std::string_view name);

		void DestroyEntity(EntityECS entity);

		Registry& GetRegistry() { return m_Registry; }

		uint32_t GetSelectedFaceID() const { return m_SelectedFaceID; }
	//Make render Scene private in the FUTURE
	public:
		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, PerspectiveCamera& editorCamera);
		void RenderScene(EntityID selectedEntity, uint32_t selectedFaceID);
		void OnRuntimeStart();
		void OnRuntimeStop();

		bool IsRunning() const { return m_Running; }
		bool IsPaused() const { return m_IsPaused; }

		void SetPaused(bool paused) { m_IsPaused = paused; }
		void Step(int frames = 1);
	private:
		template<typename T>
		void OnComponentAdded(EntityECS entity, T& component);

		void OnPhysics3DStart();
		void OnPhysics3DStop();
		void OnCreatePhysicsBody(EntityECS entity);
		void OnDestroyPhysicsBody(EntityECS entity);

		EntityECS DuplicateEntity(EntityECS entity);
	private:
		Registry m_Registry;

		reactphysics3d::PhysicsCommon m_PhysicsCommon;
		reactphysics3d::PhysicsWorld* m_PhysicsWorld = nullptr;

		uint32_t m_SelectedFaceID;

		bool m_Running = false;
		bool m_IsPaused = false;
		int m_StepFrame = 0;

		std::shared_ptr<Shader> m_DefaultShader;
		std::unordered_map<UUID, EntityECS> m_EntityMap;

		friend class EntityECS;
		friend class SceneSerializer;
		friend class SceneHierarchy;
	};
}

