#include "signpch.h"
#include "SceneSerializer.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace YAML {
    template<>
    struct convert<Sign::Vector3D>
    {
        static Node encode(const Sign::Vector3D& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, Sign::Vector3D& rhs)
        {
            if (!node.IsSequence() || node.size() != 3) return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();

            return true;
        }
    };

    template<>
    struct convert<Sign::UUID>
    {
        static Node encode(const Sign::UUID& uuid)
        {
            Node node;
            node.push_back(uuid);
            return node;
        }

        static bool decode(const Node& node, Sign::UUID& uuid)
        {
            uuid = node.as<uint64_t>();
            return true;
        }
    };
}
namespace Sign {
    
    
    YAML::Emitter& operator<<(YAML::Emitter& out, const Vector3D& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;

        return out;
    }
    YAML::Emitter& operator<<(YAML::Emitter& out, const Quaternion& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;

        return out;
    }

    static std::string Rigidbody3DBodTypeToString(RigidBody3D::BodyType type) {
        switch (type)
        {
        case Sign::RigidBody3D::BodyType::Static: return "Static";
        case Sign::RigidBody3D::BodyType::Dynamic: return "Dynamic";
        case Sign::RigidBody3D::BodyType::Kinematic: return "Kinematic";
        
        }

        return {};
    }
    static RigidBody3D::BodyType Rigidbody3DBodTypeFromString(std::string_view string) {
        if (string == "String") return RigidBody3D::BodyType::Static;
        if (string == "Dynamic") return RigidBody3D::BodyType::Dynamic;
        if (string == "Kinematic") return RigidBody3D::BodyType::Kinematic;

        return RigidBody3D::BodyType::Static;
    }
    static std::string SourceTypeToString(MeshRendererComponent::SourceType type) {
        switch (type)
        {
        case Sign::MeshRendererComponent::SourceType::Asset: return "Asset";
        case Sign::MeshRendererComponent::SourceType::Primitive: return "Primitive";

        }

        return {};
    }
    static MeshRendererComponent::SourceType SourceTypeFromString(std::string_view string) {
        if (string == "Asset") return MeshRendererComponent::SourceType::Asset;
        if (string == "Primitive") return MeshRendererComponent::SourceType::Primitive;

        return MeshRendererComponent::SourceType::Asset;
    }

    static std::string PrimitiveTypeToString(MeshRendererComponent::PrimitiveType type) {
        switch (type)
        {
        case Sign::MeshRendererComponent::PrimitiveType::None: return "None";
        case Sign::MeshRendererComponent::PrimitiveType::Cube: return "Cube";
        case Sign::MeshRendererComponent::PrimitiveType::Sphere: return "Sphere";
        case Sign::MeshRendererComponent::PrimitiveType::Plane: return "Plane";

        }

        return {};
    }
    static MeshRendererComponent::PrimitiveType PrimitiveTypeFromString(std::string_view string) {
        if (string == "None") return MeshRendererComponent::PrimitiveType::None;
        if (string == "Cube") return MeshRendererComponent::PrimitiveType::Cube;
        if (string == "Sphere") return MeshRendererComponent::PrimitiveType::Sphere;
        if (string == "Plane") return MeshRendererComponent::PrimitiveType::Plane;

        return MeshRendererComponent::PrimitiveType::None;
    }
    SceneSerializer::SceneSerializer(const std::shared_ptr<Scene>& scene) : m_Scene(scene)
    {
    }

    static void SerializeEntity(YAML::Emitter& out, EntityECS entity) {
        out << YAML::BeginMap;
        out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID(); //TO DO ENTITY ID


        if (entity.HasComponent<TagComponent>()) {
            out << YAML::Key << "TagComponent";
            out << YAML::BeginMap;

            auto& tag = entity.GetComponent<TagComponent>().Tag;
            out << YAML::Key << "Tag" << YAML::Value << tag;
            out << YAML::EndMap;
        }

        if (entity.HasComponent<TransformComponent>()) {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap;

            auto& transform = entity.GetComponent<TransformComponent>();
            out << YAML::Key << "Translation" << YAML::Value << transform.Translation;
            out << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
            out << YAML::Key << "Scale" << YAML::Value << transform.Scale;

            out << YAML::EndMap;
        }

        if (entity.HasComponent<RigidBody3D>()) {
            out << YAML::Key << "Rigidbody3DComponent";
            out << YAML::BeginMap;

            auto& rb3d = entity.GetComponent<RigidBody3D>();
            out << YAML::Key << "Type" << YAML::Value << Rigidbody3DBodTypeToString(rb3d.Type);
            
            out << YAML::EndMap;
        }

        if (entity.HasComponent<Box3DColliderComponent>()) {
            out << YAML::Key << "Box3DColliderComponent";
            out << YAML::BeginMap;

            auto& bc3d = entity.GetComponent<Box3DColliderComponent>();
            out << YAML::Key << "Offset" << YAML::Value << bc3d.Offset;
            out << YAML::Key << "Size" << YAML::Value << bc3d.Size;
            out << YAML::Key << "Density" << YAML::Value << bc3d.Density;
            out << YAML::Key << "Friction" << YAML::Value << bc3d.Friction;
            out << YAML::Key << "Restitution" << YAML::Value << bc3d.Restitution;
            out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc3d.RestitutionThreshold;
            
            out << YAML::EndMap;
        }

        if (entity.HasComponent<SphereColliderComponent>()) {
            out << YAML::Key << "SphereColliderComponent";
            out << YAML::BeginMap;

            auto& bc3d = entity.GetComponent<SphereColliderComponent>();
            out << YAML::Key << "Offset" << YAML::Value << bc3d.Offset;
            out << YAML::Key << "Radius" << YAML::Value << bc3d.Radius;
            out << YAML::Key << "Density" << YAML::Value << bc3d.Density;
            out << YAML::Key << "Friction" << YAML::Value << bc3d.Friction;
            out << YAML::Key << "Restitution" << YAML::Value << bc3d.Restitution;
            out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc3d.RestitutionThreshold;

            out << YAML::EndMap;
        }

        if (entity.HasComponent<MeshRendererComponent>())
        {
            out << YAML::Key << "MeshRendererComponent";
            out << YAML::BeginMap;

            auto& meshComponent = entity.GetComponent<MeshRendererComponent>();

            out << YAML::Key << "MeshHandle" << YAML::Value << meshComponent.MeshA;
            out << YAML::Key << "TextureHandle" << YAML::Value << meshComponent.TextureA;
            out << YAML::Key << "SourceType" << YAML::Value << SourceTypeToString(meshComponent.Type);
            out << YAML::Key << "PrimitiveType" << YAML::Value << PrimitiveTypeToString(meshComponent.PType);

            out << YAML::EndMap;
        }

        out << YAML::EndMap;
    }
    void SceneSerializer::Serialize(std::string_view filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled";
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

        m_Scene->m_Registry.each([&](EntityID entityID) {
            EntityECS entity = { entityID, m_Scene.get() };
            if (!entity) return;

            SerializeEntity(out, entity);
        });

        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(filepath.data());
        fout << out.c_str();
    }
    void SceneSerializer::SerializeRuntime(std::string_view filepath)
    {
    }
    bool SceneSerializer::Deserialize(std::string_view filepath)
    {

        std::ifstream stream(filepath.data());
        std::stringstream strStream;
        strStream << stream.rdbuf();

        YAML::Node data = YAML::Load(strStream.str());
        if (!data["Scene"])
            return false;

        std::string sceneName = data["Scene"].as<std::string>();

        auto entities = data["Entities"];

        if (entities) {
            for (auto entity : entities) {
                

                uint64_t uuid = entity["Entity"].as<uint64_t>();
                std::string name;
                auto tagComponent = entity["TagComponent"];
                if (tagComponent) {
                    name = tagComponent["Tag"].as<std::string>();
                }

                EntityECS deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

                auto transformComponent = entity["TransformComponent"];

                if (transformComponent) {
                    auto& transform = deserializedEntity.GetComponent<TransformComponent>();
                    transform.Translation = transformComponent["Translation"].as<Vector3D>();
                    transform.Rotation = transformComponent["Rotation"].as<Vector3D>();
                    transform.Scale = transformComponent["Scale"].as<Vector3D>();
                }

                auto rigidbody3DComponent = entity["RigidBody3DComponent"];
                if (rigidbody3DComponent) {
                    auto& rb3d = deserializedEntity.AddComponent<RigidBody3D>();
                    rb3d.Type = Rigidbody3DBodTypeFromString(rigidbody3DComponent["Type"].as<std::string>());
                }

                auto box3DColliderComponent = entity["Box3DColliderComponent"];
                if (box3DColliderComponent) {
                    auto& bc3d = deserializedEntity.AddComponent<Box3DColliderComponent>();

                    bc3d.Offset = box3DColliderComponent["Offset"].as<Vector3D>();
                    bc3d.Size = box3DColliderComponent["Size"].as<Vector3D>();
                    bc3d.Density = box3DColliderComponent["Density"].as<float>();
                    bc3d.Friction = box3DColliderComponent["Friction"].as<float>();
                    bc3d.Restitution = box3DColliderComponent["Restitution"].as<float>();
                    bc3d.RestitutionThreshold = box3DColliderComponent["RestitutionThreshold"].as<float>();
                }

                auto sphereColliderComponent = entity["SphereColliderComponent"];
                if (sphereColliderComponent) {
                    auto& sc3d = deserializedEntity.AddComponent<SphereColliderComponent>();

                    sc3d.Offset = sphereColliderComponent["Offset"].as<Vector3D>();
                    sc3d.Radius = sphereColliderComponent["Radius"].as<float>();
                    sc3d.Density = sphereColliderComponent["Density"].as<float>();
                    sc3d.Friction = sphereColliderComponent["Friction"].as<float>();
                    sc3d.Restitution = sphereColliderComponent["Restitution"].as<float>();
                    sc3d.RestitutionThreshold = sphereColliderComponent["RestitutionThreshold"].as<float>();
                }

                auto meshRendererComponent = entity["MeshRendererComponent"];

                if (meshRendererComponent)
                {
                    auto& mesh = deserializedEntity.AddComponent<MeshRendererComponent>();

                    mesh.MeshA = meshRendererComponent["MeshHandle"].as<AssetHandle>();
                    mesh.TextureA = meshRendererComponent["TextureHandle"].as<AssetHandle>();
                    mesh.Type = SourceTypeFromString(meshRendererComponent["SourceType"].as<std::string>());
                    mesh.PType = PrimitiveTypeFromString(meshRendererComponent["PrimitiveType"].as<std::string>());
                }
            }
        }
        return true;
    }
    bool SceneSerializer::DeserializeRuntime(std::string_view filepath)
    {

        return false;
    }
}