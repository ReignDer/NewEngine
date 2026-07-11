#include "signpch.h"
#include "Scene.h"
#include "Sign/Scene/EntityECS.h"
namespace Sign {
    Scene::Scene()
    {
    }

    Scene::~Scene()
    {
    }

    EntityECS Scene::CreateEntity(std::string_view name)
    {
        EntityID id = m_Registry.CreateEntity();
        EntityECS entity(id, this);
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name);

        return entity;
    }

    void Scene::DestroyEntity(EntityECS entity)
    {
        m_Registry.DestroyEntity(entity.GetID());
    }

    void Scene::RenderScene()
    {
        auto& meshPool = m_Registry.GetPool<MeshRendererComponent>();

        for (auto& renderer : meshPool) {
            EntityID entity = renderer.m_entity;

            TransformComponent* component = m_Registry.GetComponent<TransformComponent>(entity);

            if (!component)
                continue;

            
            Renderer::Submit(
                renderer.Mesh->GetVertexArray(),
                *renderer.Shader,
                component->GetTransform(),
                entity
            );
        }
    }
}