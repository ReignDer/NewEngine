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
    template<typename... Component>
    static void CopyComponent(Registry& dst, Registry& src, const std::unordered_map<UUID,EntityECS>& entityMap)
    {
        ([&]()
        {
            for (auto& srcComponent : src.GetPool<Component>())
            {
                EntityID srcEntity = srcComponent.m_entity;
                UUID id = src.GetComponent<IDComponent>(srcEntity)->ID;
                EntityECS dstEntity = entityMap.at(id);

                dst.AddOrReplaceComponent<Component>(dstEntity, srcComponent);
            }
        }(), ...

        );
    }
    template<typename... Component>
    static void CopyComponent(ComponentGroup<Component...>, Registry& dst, Registry& src, const std::unordered_map<UUID,EntityECS>& entityMap)
    {
 
        CopyComponent<Component...>(dst, src, entityMap);
       
    }
    template<typename... Component>
    static void CopyComponentIfExists(EntityECS dst, EntityECS src)
    {
        ([&]()
        {
            if (src.HasComponent<Component>())
                dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
        }(),...);

    }
    template<typename... Component>
    static void CopyComponentIfExists(ComponentGroup<Component...>, EntityECS dst, EntityECS src)
    {
        CopyComponentIfExists<Component...>(dst, src);

    }
    std::shared_ptr<Scene> Scene::Copy(std::shared_ptr<Scene> other)
    {
        std::shared_ptr<Scene> newScene = std::make_shared<Scene>();

        auto& srcSceneRegistry = other->m_Registry;
        auto& dstSceneRegistry = newScene->m_Registry;
        std::unordered_map<UUID, EntityECS> entityMap;

        auto idView = srcSceneRegistry.GetPool<IDComponent>();

        for (auto e : idView)
        {
            UUID uuid = e.ID;
            EntityID entityID = e.m_entity;
            const auto& name = srcSceneRegistry.GetComponent<TagComponent>(entityID)->Tag;
            EntityECS newEntity = newScene->CreateEntityWithUUID(uuid, name);
            entityMap[uuid] = newEntity;
        }
        CopyComponent(AllComponents{},dstSceneRegistry,srcSceneRegistry,entityMap);
        return newScene;
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
        if (m_Running)
            OnDestroyPhysicsBody(entity);
        m_EntityMap.erase(entity.GetUUID());
        m_Registry.DestroyEntity(entity);
    }

    void Scene::OnUpdateRuntime(Timestep ts)
    {
        if (!m_IsPaused || m_StepFrame-- > 0)
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
                    if (!body) continue;
                    const auto& bodyTransform = body->getTransform();
                    auto pos = body->getTransform().getPosition();
                    transform.Translation.x = bodyTransform.getPosition().x;
                    transform.Translation.y = bodyTransform.getPosition().y;
                    transform.Translation.z = bodyTransform.getPosition().z;
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
    }

    void Scene::OnUpdateEditor(Timestep ts, PerspectiveCamera& editorCamera)
    {
    }

    void Scene::RenderScene(EntityID selectedEntity, uint32_t selectedFaceID)
    {
        m_SelectedFaceID = selectedFaceID;
        auto& meshPool = m_Registry.GetPool<MeshRendererComponent>();
        auto& transform = m_Registry.GetPool<TransformComponent>();
        auto& tag = m_Registry.GetPool<TagComponent>();

        {
            std::vector<GPULight> gpuLights;
            auto& lightPool = m_Registry.GetPool<LightComponent>();
            for (auto& light : lightPool)
            {
                auto* tc = transform.Get(light.m_entity);
                if (!tc) continue;
                GPULight gl{};
                gl.Position = tc->Translation;
                gl.Type = (float)light.Type;
                Vector3D forward = { 0.0f,-1.0f,0.0f };
                Quaternion q = Quaternion::FromEulerAngles(tc->Rotation);
                gl.Direction = q.rotate(forward);
                gl.Color = light.Color;
                gl.Range = light.Range;
                gl.Intensity = light.Intensity;
                gl.InnerConeCos = std::cosf(MathUtils::ConvertToRadians(light.InnerConeAngle));
                gl.OuterConeCos = std::cosf(MathUtils::ConvertToRadians(light.OuterConeAngle));
                gpuLights.push_back(gl);

            }
            Renderer::SetLights(gpuLights);
        }
        for (auto& renderer : meshPool) {
            EntityID entity = renderer.m_entity;

            TransformComponent* component = transform.Get(entity);
            TagComponent* name = tag.Get(entity);
            if (!component)
                continue;
            if (!renderer.Shader) {
                if (!m_DefaultShader)
                {
                    ShaderLibrary lib;
                    m_DefaultShader = lib.GetDefault();
                }
                renderer.Shader = m_DefaultShader;
            }


            if (renderer.MeshA) {
                std::shared_ptr<Mesh> mesh;
                if (renderer.Type == MeshRendererComponent::SourceType::Primitive)
                {
                    switch (renderer.PType)
                    {
                        case MeshRendererComponent::PrimitiveType::Cube: mesh = Primitive::Cube3D::Create(); break;
                        case MeshRendererComponent::PrimitiveType::Sphere: mesh = Primitive::Sphere::Create(); break;
                        case MeshRendererComponent::PrimitiveType::Plane: mesh = Primitive::Plane::Create(); break;
                        case MeshRendererComponent::PrimitiveType::None: mesh = AssetManager::GetAsset<Mesh>(renderer.MeshA); break;
                    }
                }
                else
                {
                    mesh = AssetManager::GetAsset<Mesh>(renderer.MeshA);
                }

                if (!mesh) continue;

                auto texture = (renderer.TextureA) ? AssetManager::GetAsset<Texture2D>(renderer.TextureA) : Renderer::GetWhiteTexture();

                if (!texture) continue;
                Renderer::Submit(
                    mesh->GetVertexArray(),
                    *renderer.Shader,
                    component->GetTransform(),
                    *texture
                );
            }

          
        }
    }
    void Scene::OnRuntimeStart()
    {
        m_Running = true;
        OnPhysics3DStart();
    }
    void Scene::OnRuntimeStop()
    {
        m_Running = false;
        OnPhysics3DStop();
    }
    void Scene::Step(int frames)
    {
        m_StepFrame = frames;
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

            OnCreatePhysicsBody(entity);

        }
    }
    void Scene::OnPhysics3DStop()
    {
        m_PhysicsCommon.destroyPhysicsWorld(m_PhysicsWorld);
        m_PhysicsWorld = nullptr;
    }

    void Scene::OnCreatePhysicsBody(EntityECS entity)
    {
        if (!m_PhysicsWorld) return;
        if (!entity.HasComponent<RigidBody3D>()) return;
        auto& transform = entity.GetComponent<TransformComponent>();
        auto& rb3d = entity.GetComponent<RigidBody3D>();

        if (rb3d.RuntimeBody != nullptr) return;
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

    void Scene::OnDestroyPhysicsBody(EntityECS entity)
    {
        if (!m_PhysicsWorld) return;
        if (!entity.HasComponent<RigidBody3D>()) return;

        auto& rb3d = entity.GetComponent<RigidBody3D>();
        if (rb3d.RuntimeBody) {
            m_PhysicsWorld->destroyRigidBody((reactphysics3d::RigidBody*)rb3d.RuntimeBody);
            rb3d.RuntimeBody = nullptr;
        }
    }

    EntityECS Scene::DuplicateEntity(EntityECS entity)
    {
        std::string name = entity.GetName().data();
        EntityECS newEntity = CreateEntity(name);
        CopyComponentIfExists(AllComponents{}, newEntity, entity);
        return newEntity;

    }

    template<typename T>
    void Scene::OnComponentAdded(EntityECS entity, T& component)
    {
        static_assert(sizeof(T) == 0);
    }

    template<>
    void Scene::OnComponentAdded<IDComponent>(EntityECS entity, IDComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<TransformComponent>(EntityECS entity, TransformComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<MeshRendererComponent>(EntityECS entity, MeshRendererComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<TagComponent>(EntityECS entity, TagComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<RigidBody3D>(EntityECS entity, RigidBody3D& component)
    {
        if (m_Running)
            OnCreatePhysicsBody(entity);
    }

    template<>
    void Scene::OnComponentAdded<Box3DColliderComponent>(EntityECS entity, Box3DColliderComponent& component)
    {
        if (m_Running && entity.HasComponent<RigidBody3D>()) {
            auto& rb3d = entity.GetComponent<RigidBody3D>();
            if (rb3d.RuntimeBody) {
                OnDestroyPhysicsBody(entity);
                OnCreatePhysicsBody(entity);
            }
        }
    }

    template<>
    void Scene::OnComponentAdded<SphereColliderComponent>(EntityECS entity, SphereColliderComponent& component)
    {
        if (m_Running && entity.HasComponent<RigidBody3D>()) {
            auto& rb3d = entity.GetComponent<RigidBody3D>();
            if (rb3d.RuntimeBody) {
                OnDestroyPhysicsBody(entity);
                OnCreatePhysicsBody(entity);
            }
        }
    }
    template<>
    void Scene::OnComponentAdded<LightComponent>(EntityECS entity, LightComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<CameraComponent>(EntityECS entity, CameraComponent& component)
    {
        
    }

}