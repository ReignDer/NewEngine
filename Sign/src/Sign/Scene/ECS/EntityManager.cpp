#include "EntityManager.h"

namespace Sign {
    EntityID EntityManager::CreateEntity()
    {
        EntityID id;
        if (!m_AvailableEntities.empty()) {
            id = m_AvailableEntities.back();
            m_AvailableEntities.pop_back();
        }
        else {
            id = m_NextEntityID++;
        }

        m_AliveIndex[id] = m_AliveEntities.size();
        m_AliveEntities.push_back(id);

        return id;
    }

    void EntityManager::DestroyEntity(EntityID entity)
    {
        m_AvailableEntities.push_back(entity);
        m_Signatures[entity].reset();
        
        auto it = m_AliveIndex.find(entity);
        if (it == m_AliveIndex.end())
            return;

        size_t idx = it->second;
        size_t lastIdx = m_AliveEntities.size() - 1;

        if (idx != lastIdx) {
            m_AliveEntities[idx] = m_AliveEntities[lastIdx];
            m_AliveIndex[m_AliveEntities[idx]] = idx;
        }

        m_AliveEntities.pop_back();
        m_AliveIndex.erase(entity);
    }
    void EntityManager::SetSignature(EntityID entity, Signature signature)
    {
        if (entity >= m_Signatures.size())
            m_Signatures.resize(entity + 1);
        m_Signatures[entity] = signature;
    }
    Signature EntityManager::GetSignature(EntityID entity)
    {
        if (entity >= m_Signatures.size())
            return{};
        return m_Signatures[entity];
    }
}
