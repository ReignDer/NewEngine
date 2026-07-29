#pragma once
#include "Sign/Scene/Scene.h"
#include "Sign/Scene/ECS/ECSTypes.h"
#include "Sign/Scene/ECS/Components.h"
#include "Sign/UUID.h"
namespace Sign {
	class EntityECS {
	public:
		EntityECS() = default;
		EntityECS(EntityID entity, Scene* scene);
		EntityECS(const EntityECS& other) = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) {
			return m_Scene->GetRegistry().AddComponent<T>(m_EntityHandle, args...);
		}

		template<typename T>
		T& GetComponent() {
			// TODO: insert return statement here
			return *m_Scene->GetRegistry().GetComponent<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent() {
			return m_Scene->GetRegistry().HasComponent<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent() {
			m_Scene->GetRegistry().RemoveComponent<T>(m_EntityHandle);
		}

		UUID GetUUID() { return GetComponent<IDComponent>().ID; }
		operator bool() const { return m_EntityHandle != INVALID_ENTITY_ID; }
		operator EntityID() const { return m_EntityHandle; }

		bool operator==(const EntityECS& other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
		bool operator!=(const EntityECS& other) const { return !(*this == other); }

		std::string_view GetName() { return GetComponent<TagComponent>().Tag; }

	private:
		EntityID m_EntityHandle = INVALID_ENTITY_ID;
		Scene* m_Scene = nullptr;
	};
}