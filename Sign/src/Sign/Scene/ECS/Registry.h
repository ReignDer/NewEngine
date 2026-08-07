#pragma once
#include <memory>
#include <typeindex>
#include <unordered_map>
#include "Sign/Scene/ECS/ECSTypes.h"
#include "Sign/Scene/ECS/EntityManager.h"
#include "Sign/Scene/ECS/Components.h"
#include "Sign/Scene/ECS/SparseSet.h"
namespace Sign {
	class Registry
	{
	public:
		EntityID CreateEntity() { return m_EntityManager.CreateEntity(); }
		void DestroyEntity(EntityID entity);

		template<typename T, typename... Args>
		T& AddComponent(EntityID entity, Args&&... args);

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(EntityID entity, Args&&... args);

		template<typename T>
		T* GetComponent(EntityID entity);

		template<typename T>
		bool HasComponent(EntityID entity);

		template<typename T>
		void RemoveComponent(EntityID entity);

		template<typename T>
		SparseSet<T>& GetPool();

		Signature GetSignature(EntityID entity);

		template<typename Func>
		void each(Func&& func);


	private:
		EntityManager m_EntityManager;
		std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>> m_Pools;
	};
	template<typename T, typename... Args>
	inline T& Registry::AddComponent(EntityID entity, Args&&... args)
	{
		// TODO: insert return statement here
		auto& ent = GetPool<T>().Add(entity,T(std::forward<Args>(args)...));

		Signature sig = m_EntityManager.GetSignature(entity);
		sig.set(GetComponentTypeID<T>());
		m_EntityManager.SetSignature(entity, sig);

		return ent;
	}
	template<typename T, typename ...Args>
	inline T& Registry::AddOrReplaceComponent(EntityID entity, Args && ...args)
	{
		auto& pool = GetPool<T>();
		if (T* existing = pool.Get(entity))
		{
			*existing = T(std::forward<Args>(args)...);
			return *existing;
		}

		return AddComponent<T>(entity, std::forward<Args>(args)...);
	}
	template<typename T>
	inline T* Registry::GetComponent(EntityID entity)
	{
		return GetPool<T>().Get(entity);
	}
	template<typename T>
	inline bool Registry::HasComponent(EntityID entity)
	{
		return GetComponent<T>(entity) != nullptr;
	}
	template<typename T>
	inline void Registry::RemoveComponent(EntityID entity)
	{
		GetPool<T>().Delete(entity);
		Signature sig = m_EntityManager.GetSignature(entity);
		sig.reset(GetComponentTypeID<T>());

		m_EntityManager.SetSignature(entity, sig);
	}
	template<typename T>
	inline SparseSet<T>& Registry::GetPool()
	{
		// TODO: insert return statement here

		std::type_index index = std::type_index(typeid(T));

		auto it = m_Pools.find(index);
		if (it == m_Pools.end()) {
			auto pool = std::make_unique<SparseSet<T>>();
			SparseSet<T>* raw = pool.get();
			m_Pools.emplace(index, std::move(pool));
			return *raw;

		}

		return *static_cast<SparseSet<T>*>(it->second.get());
	}
	template<typename Func>
	inline void Registry::each(Func&& func)
	{
		for (EntityID entity : m_EntityManager.GetAliveEntities()) {
			func(entity);
		}
	}
}

