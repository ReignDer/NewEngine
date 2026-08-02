#include "signpch.h"
#include "Scene.h"
#include "Sign/Scene/EntityECS.h"

#include "Sign/Asset/AssetManager.h"
namespace Sign {
    Scene::Scene()
    {
    }

    Scene::~Scene()
    {
        if(m_PhysicsWorld)
            m_PhysicsCommon.destroyPhysicsWorld(m_PhysicsWorld);
    }

    EntityECS Scene::CreateEntity(std::string_view name)
    {
        return CreateEntityWithUUID(UUID(), name);
    }

    EntityECS Scene::CreateEntityWithUUID(UUID uuid, std::string_view name)
    {
        EntityID id = m_Registry.CreateEntity();
        EntityECS entity(id, this);
        entity.AddComponent<IDComponent>(uuid);
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name);

        m_EntityMap[uuid] = entity;

        return entity;
    }

    void Scene::DestroyEntity(EntityECS entity)
    {
        m_EntityMap.erase(entity.GetUUID());
        m_Registry.DestroyEntity(entity);
    }

    void Scene::OnUpdateRuntime(Timestep ts)
    {
        {
            std::println("Physics Update");
            std::println("Timestep: {}", static_cast<float>(ts));

            m_PhysicsWorld->update(ts);


            auto& rbPool = m_Registry.GetPool<RigidBody3D>();
            for (auto& e : rbPool) {
                EntityECS entity = { e.m_entity, this };

                auto& transform = entity.GetComponent<TransformComponent>();
                auto& rb3d = entity.GetComponent<RigidBody3D>();

                reactphysics3d::RigidBody* body = (reactphysics3d::RigidBody*)rb3d.RuntimeBody;

                const auto& bodyTransform = body->getTransform();
                auto pos = body->getTransform().getPosition();
                transform.Translation.x  = bodyTransform.getPosition().x;
                transform.Translation.y  = bodyTransform.getPosition().y;
                transform.Translation.z  = bodyTransform.getPosition().z;
                std::println("Gravity enabled: {}", body->isGravityEnabled());
                switch (rb3d.Type) {
                case RigidBody3D::BodyType::Static:   std::println("BodyType: Static"); break;
                case RigidBody3D::BodyType::Dynamic: std::println("BodyType: Dynamic"); break;
                case RigidBody3D::BodyType::Kinematic:  std::println("BodyType: Kinematic"); break;
                default: std::println("BodyType: Kinematic");
                }
                std::println("Body pos: {} {} {}", pos.x, pos.y, pos.z);
                reactphysics3d::Quaternion q = bodyTransform.getOrientation();
                Quaternion quat(q.x, q.y, q.z, q.w);
                transform.Rotation = quat.ToEulerAngles();
            }
        }
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
            if (!renderer.Shader) {
                ShaderLibrary lib;
                renderer.Shader = lib.GetDefault();
            }

            if (renderer.MeshA) {
                auto mesh = AssetManager::GetAsset<Mesh>(renderer.MeshA);
                if (!mesh) continue;
                auto texture = (renderer.TextureA) ? AssetManager::GetAsset<Texture2D>(renderer.TextureA) : Renderer::GetWhiteTexture();
                std::println("Entity {} TextureA handle={} GpuHandle.ptr={}", (uint32_t)entity, (uint64_t)renderer.TextureA, texture->GetGpuHandle().ptr);
                if (!texture->IsLoaded()) continue;
                Renderer::Submit(
                    mesh->GetVertexArray(),
                    *renderer.Shader,
                    component->GetTransform(),
                    *texture
                );
            }

          
        }
    }
    void Scene::OnPhysics3DStart()
    {
        reactphysics3d::PhysicsWorld::WorldSettings settings;
        const int32_t velocityIterations = 6;
        const int32_t positionIterations = 2;
        settings.defaultPositionSolverNbIterations = positionIterations;
        settings.defaultVelocitySolverNbIterations = velocityIterations;
        settings.gravity = reactphysics3d::Vector3(0.0f,-9.81f,0.0f);
        m_PhysicsWorld = m_PhysicsCommon.createPhysicsWorld(settings);

        auto& rbPool = m_Registry.GetPool<RigidBody3D>();

        for (auto& e : rbPool) {
            EntityECS entity = { e.m_entity, this };

            auto& transform = entity.GetComponent<TransformComponent>();
            auto& rb3d = entity.GetComponent<RigidBody3D>();

            reactphysics3d::Vector3 position(transform.Translation.x, transform.Translation.y, transform.Translation.z);

            Quaternion quat = Quaternion::FromEulerAngles(transform.Rotation);
            reactphysics3d::Quaternion quaternion(quat.x, quat.y, quat.z, quat.w);

            reactphysics3d::Transform rTransform = { position, quaternion };

            reactphysics3d::RigidBody* body = m_PhysicsWorld->createRigidBody(rTransform);
            body->setType(PhysicsUtils::RigidBody3DTypeToReactType(rb3d.Type));
            rb3d.RuntimeBody = body;
            if (entity.HasComponent<Box3DColliderComponent>()) {
                auto& bc3d = entity.GetComponent<Box3DColliderComponent>();
                const reactphysics3d::Vector3 halfExtents(bc3d.Size.x * transform.Scale.x, bc3d.Size.y * transform.Scale.y, bc3d.Size.z * transform.Scale.z);
                reactphysics3d::BoxShape* boxShape = m_PhysicsCommon.createBoxShape(halfExtents);
                
                reactphysics3d::Collider* collider;
                reactphysics3d::Vector3 offset(bc3d.Offset.x, bc3d.Offset.y, bc3d.Offset.z);
                reactphysics3d::Quaternion localOrientation = reactphysics3d::Quaternion::identity();
                reactphysics3d::Transform colTransform(offset, localOrientation);
                collider = body->addCollider(boxShape, colTransform);
                reactphysics3d::Material& material = collider->getMaterial();

                material.setMassDensity(bc3d.Density);
                material.setFrictionCoefficient(bc3d.Friction);
                material.setBounciness(bc3d.Restitution);

            }

            if (entity.HasComponent<SphereColliderComponent>())
            {
                auto& sc3d = entity.GetComponent<SphereColliderComponent>();

                reactphysics3d::SphereShape* sphereShape = m_PhysicsCommon.createSphereShape(sc3d.Radius * transform.Scale.x);

                reactphysics3d::Collider* collider;
                reactphysics3d::Vector3 offset(sc3d.Offset.x, sc3d.Offset.y, sc3d.Offset.z);
                reactphysics3d::Quaternion localOrientation = reactphysics3d::Quaternion::identity();
                reactphysics3d::Transform colTransform(offset, localOrientation);
                collider = body->addCollider(sphereShape, colTransform);
                reactphysics3d::Material& material = collider->getMaterial();

                material.setMassDensity(sc3d.Density);
                material.setFrictionCoefficient(sc3d.Friction);
                material.setBounciness(sc3d.Restitution);
            }


        }
    }
    void Scene::OnPhysics3DStop()
    {
        m_PhysicsCommon.destroyPhysicsWorld(m_PhysicsWorld);
        m_PhysicsWorld = nullptr;
    }
}