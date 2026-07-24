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

    void Scene::RenderScene(EntityID selectedEntity, uint32_t selectedFaceID)
    {
        m_SelectedFaceID = selectedFaceID;
        auto& meshPool = m_Registry.GetPool<MeshRendererComponent>();
        auto& transform = m_Registry.GetPool<TransformComponent>();

        for (auto& renderer : meshPool) {
            EntityID entity = renderer.m_entity;

            TransformComponent* component = transform.Get(entity);

            if (!component)
                continue;

            auto texture = (renderer.Texture && renderer.Texture->IsLoaded()) ? renderer.Texture : Renderer::GetWhiteTexture();
            Renderer::Submit(
                renderer.Mesh->GetVertexArray(),
                *renderer.Shader,
                component->GetTransform(),
                *texture
            );
        }
    }
}