#include "signpch.h"
#include "SceneSerializer.h"

#include <fstream>
#include <yaml-cpp/yaml.h>
#include <json/json.h>

#include "Sign/Project/Project.h"
#include "Sign/Renderer/Primitive3D.h"
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
        if (string == "Static") return RigidBody3D::BodyType::Static;
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
        case Sign::MeshRendererComponent::PrimitiveType::Capsule: return "Capsule";
        case Sign::MeshRendererComponent::PrimitiveType::Plane: return "Plane";

        }

        return {};
    }
    static MeshRendererComponent::PrimitiveType PrimitiveTypeFromString(std::string_view string) {
        if (string == "None") return MeshRendererComponent::PrimitiveType::None;
        if (string == "Cube") return MeshRendererComponent::PrimitiveType::Cube;
        if (string == "Sphere") return MeshRendererComponent::PrimitiveType::Sphere;
        if (string == "Capsule") return MeshRendererComponent::PrimitiveType::Capsule;
        if (string == "Plane") return MeshRendererComponent::PrimitiveType::Plane;

        return MeshRendererComponent::PrimitiveType::None;
    }

    static SceneFormat DetectFormatFromFile(std::string_view filepath)
    {
        if (filepath.ends_with(".sign"))
            return SceneFormat::YAML;
        else if (filepath.ends_with(".level"))
            return SceneFormat::JSON;

        return SceneFormat::YAML;

    }
}
namespace YAMLBackend {

    YAML::Emitter& operator<<(YAML::Emitter& out, const Sign::Vector3D& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;

        return out;
    }
    YAML::Emitter& operator<<(YAML::Emitter& out, const Sign::Quaternion& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;

        return out;
    }

    static void SerializeEntity(YAML::Emitter& out, Sign::EntityECS entity) {
        out << YAML::BeginMap;
        out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID(); //TO DO ENTITY ID


        if (entity.HasComponent<Sign::TagComponent>()) {
            out << YAML::Key << "TagComponent";
            out << YAML::BeginMap;

            auto& tag = entity.GetComponent<Sign::TagComponent>().Tag;
            out << YAML::Key << "Tag" << YAML::Value << tag;
            out << YAML::EndMap;
        }

        if (entity.HasComponent<Sign::TransformComponent>()) {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap;

            auto& transform = entity.GetComponent<Sign::TransformComponent>();
            out << YAML::Key << "Translation" << YAML::Value << transform.Translation;
            out << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
            out << YAML::Key << "Scale" << YAML::Value << transform.Scale;

            out << YAML::EndMap;
        }

        if (entity.HasComponent<Sign::RigidBody3D>()) {
            out << YAML::Key << "RigidBody3DComponent";
            out << YAML::BeginMap;

            auto& rb3d = entity.GetComponent<Sign::RigidBody3D>();
            out << YAML::Key << "Type" << YAML::Value << Sign::Rigidbody3DBodTypeToString(rb3d.Type);

            out << YAML::EndMap;
        }

        if (entity.HasComponent<Sign::Box3DColliderComponent>()) {
            out << YAML::Key << "Box3DColliderComponent";
            out << YAML::BeginMap;

            auto& bc3d = entity.GetComponent<Sign::Box3DColliderComponent>();
            out << YAML::Key << "Offset" << YAML::Value << bc3d.Offset;
            out << YAML::Key << "Size" << YAML::Value << bc3d.Size;
            out << YAML::Key << "Density" << YAML::Value << bc3d.Density;
            out << YAML::Key << "Friction" << YAML::Value << bc3d.Friction;
            out << YAML::Key << "Restitution" << YAML::Value << bc3d.Restitution;
            out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc3d.RestitutionThreshold;

            out << YAML::EndMap;
        }

        if (entity.HasComponent<Sign::SphereColliderComponent>()) {
            out << YAML::Key << "SphereColliderComponent";
            out << YAML::BeginMap;

            auto& sc3d = entity.GetComponent<Sign::SphereColliderComponent>();
            out << YAML::Key << "Offset" << YAML::Value << sc3d.Offset;
            out << YAML::Key << "Radius" << YAML::Value << sc3d.Radius;
            out << YAML::Key << "Density" << YAML::Value << sc3d.Density;
            out << YAML::Key << "Friction" << YAML::Value << sc3d.Friction;
            out << YAML::Key << "Restitution" << YAML::Value << sc3d.Restitution;
            out << YAML::Key << "RestitutionThreshold" << YAML::Value << sc3d.RestitutionThreshold;

            out << YAML::EndMap;
        }
        if (entity.HasComponent<Sign::CapsuleColliderComponent>()) {
            out << YAML::Key << "SphereColliderComponent";
            out << YAML::BeginMap;

            auto& cc3d = entity.GetComponent<Sign::CapsuleColliderComponent>();
            out << YAML::Key << "Offset" << YAML::Value << cc3d.Offset;
            out << YAML::Key << "Radius" << YAML::Value << cc3d.Radius;
            out << YAML::Key << "Height" << YAML::Value << cc3d.Height;
            out << YAML::Key << "Density" << YAML::Value << cc3d.Density;
            out << YAML::Key << "Friction" << YAML::Value << cc3d.Friction;
            out << YAML::Key << "Restitution" << YAML::Value << cc3d.Restitution;
            out << YAML::Key << "RestitutionThreshold" << YAML::Value << cc3d.RestitutionThreshold;

            out << YAML::EndMap;
        }

        if (entity.HasComponent<Sign::MeshRendererComponent>())
        {
            out << YAML::Key << "MeshRendererComponent";
            out << YAML::BeginMap;

            auto& meshComponent = entity.GetComponent<Sign::MeshRendererComponent>();

            out << YAML::Key << "MeshHandle" << YAML::Value << meshComponent.MeshA;
            out << YAML::Key << "TextureHandle" << YAML::Value << meshComponent.TextureA;
            out << YAML::Key << "SourceType" << YAML::Value << Sign::SourceTypeToString(meshComponent.Type);
            out << YAML::Key << "PrimitiveType" << YAML::Value << Sign::PrimitiveTypeToString(meshComponent.PType);

            out << YAML::EndMap;
        }

        out << YAML::EndMap;
    }

    static bool Deserialize(std::string_view filepath)
    {
        
    }
}

namespace JSONBackend {
    static Json::Value WriteVec3(const Sign::Vector3D& vector)
    {
        Json::Value arr(Json::arrayValue);
        arr.append(vector.x);
        arr.append(vector.y);
        arr.append(vector.z);
        return arr;
    }
    static Sign::Vector3D ReadVec3(const Json::Value& node)
    {
        Sign::Vector3D vector{};
        if (node.isArray() && node.size() == 3)
        {
            vector.x = node[0].asFloat();
            vector.y = node[1].asFloat();
            vector.z = node[2].asFloat();
        }

        return vector;
    }
    static Json::Value SerializeEntity(Sign::EntityECS entity)
    {
        Json::Value entityNode(Json::objectValue);
        entityNode["Entity"] = (Json::UInt64)entity.GetUUID();

        if (entity.HasComponent<Sign::TagComponent>()) {
            Json::Value tag(Json::objectValue);
            tag["Tag"] = entity.GetComponent<Sign::TagComponent>().Tag;
            entityNode["TagComponent"] = tag;
        }

        if (entity.HasComponent<Sign::TransformComponent>()) {
            auto& transform = entity.GetComponent<Sign::TransformComponent>();
            Json::Value tc(Json::objectValue);
            tc["Translation"] = WriteVec3(transform.Translation);
            tc["Rotation"] = WriteVec3(MathUtils::ConvertToDegreesVec3(transform.Rotation));
            tc["Scale"] = WriteVec3(transform.Scale);

            entityNode["TransformComponent"] = tc;
        }

        if (entity.HasComponent<Sign::RigidBody3D>()) {
            auto& rb3d = entity.GetComponent<Sign::RigidBody3D>();
            Json::Value rb(Json::objectValue);
            rb["Type"] = Sign::Rigidbody3DBodTypeToString(rb3d.Type);
            entityNode["RigidBody3DComponent"] = rb;
        }

        if (entity.HasComponent<Sign::Box3DColliderComponent>()) {
            auto& bc3d = entity.GetComponent<Sign::Box3DColliderComponent>();
            Json::Value bc(Json::objectValue);
            bc["Offset"] = WriteVec3(bc3d.Offset);
            bc["Size"] = WriteVec3(bc3d.Size);
            bc["Density"] = bc3d.Density;
            bc["Friction"] = bc3d.Friction;
            bc["Restitution"] = bc3d.Restitution;
            bc["RestitutionThreshold"] = bc3d.RestitutionThreshold;
            entityNode["Box3DColliderComponent"] = bc;
        }

        if (entity.HasComponent<Sign::SphereColliderComponent>()) {
            auto& sc3d = entity.GetComponent<Sign::SphereColliderComponent>();
            Json::Value sc(Json::objectValue);
            sc["Offset"] = WriteVec3(sc3d.Offset);
            sc["Radius"] = sc3d.Radius;
            sc["Density"] = sc3d.Density;
            sc["Friction"] = sc3d.Friction;
            sc["Restitution"] = sc3d.Restitution;
            sc["RestitutionThreshold"] = sc3d.RestitutionThreshold;
            entityNode["SphereColliderComponent"] = sc;
        }
        if (entity.HasComponent<Sign::CapsuleColliderComponent>()) {
            auto& cc3d = entity.GetComponent<Sign::CapsuleColliderComponent>();
            Json::Value sc(Json::objectValue);
            sc["Offset"] = WriteVec3(cc3d.Offset);
            sc["Radius"] = cc3d.Radius;
            sc["Height"] = cc3d.Height;
            sc["Density"] = cc3d.Density;
            sc["Friction"] = cc3d.Friction;
            sc["Restitution"] = cc3d.Restitution;
            sc["RestitutionThreshold"] = cc3d.RestitutionThreshold;
            entityNode["CapsuleColliderComponent"] = sc;
        }

        if (entity.HasComponent<Sign::MeshRendererComponent>())
        {
            auto& meshComponent = entity.GetComponent<Sign::MeshRendererComponent>();
            Json::Value mesh(Json::objectValue);
            mesh["MeshHandle"] = (Json::UInt64)meshComponent.MeshA;
            mesh["TextureHandle"] = (Json::UInt64)meshComponent.TextureA;
            mesh["SourceType"] = Sign::SourceTypeToString(meshComponent.Type);
            mesh["PrimitiveType"] = Sign::PrimitiveTypeToString(meshComponent.PType);
            entityNode["MeshRendererComponent"] = mesh;
           
        }

        return entityNode;
    }

    static bool Deserialize(std::string_view filepath)
    {

    }
}
namespace Sign {
  

    SceneSerializer::SceneSerializer(const std::shared_ptr<Scene>& scene) : m_Scene(scene)
    {
    }

   
    void SceneSerializer::Serialize(std::string_view filepath)
    {
        SceneFormat format = DetectFormatFromFile(filepath);
        if (format == SceneFormat::YAML)
            SerializeYAML(filepath);
        else if(format == SceneFormat::JSON)
            SerializeJSON(filepath);
    }
    void SceneSerializer::SerializeRuntime(std::string_view filepath)
    {
    }
    bool SceneSerializer::Deserialize(std::string_view filepath)
    {

        SceneFormat format = DetectFormatFromFile(filepath);
        if (format == SceneFormat::YAML)
            return DeserializeYAML(filepath);
        if (format == SceneFormat::JSON)
            return DeserializeJSON(filepath);

        return DeserializeYAML(filepath);
    }
    bool SceneSerializer::DeserializeRuntime(std::string_view filepath)
    {

        return false;
    }
    void SceneSerializer::SerializeYAML(std::string_view filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled";
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

        m_Scene->m_Registry.each([&](EntityID entityID) {
            EntityECS entity = { entityID, m_Scene.get() };
            if (!entity) return;

            YAMLBackend::SerializeEntity(out, entity);
            });

        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(filepath.data());
        fout << out.c_str();
    }
    void SceneSerializer::SerializeJSON(std::string_view filepath)
    {
        Json::Value root(Json::objectValue);
        root["Scene"] = "Untitled";
        Json::Value entities(Json::arrayValue);
        m_Scene->m_Registry.each([&](EntityID entityID) {
            EntityECS entity = { entityID, m_Scene.get() };
            if (!entity) return;

            entities.append(JSONBackend::SerializeEntity(entity));
            });
        root["Entities"] = entities;
        Json::StreamWriterBuilder builder;
        builder["indentation"] = "  ";
        const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
        std::ofstream fout(filepath.data());
        writer->write(root, &fout);

    }
    bool SceneSerializer::DeserializeYAML(std::string_view filepath)
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
                EntityECS deserializedEntity;
                if(uuid == 0) deserializedEntity = m_Scene->CreateEntity(name);
                else deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

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
                auto capsuleColliderComponent = entity["CapsuleColliderComponent"];
                if (capsuleColliderComponent) {
                    auto& cc3d = deserializedEntity.AddComponent<CapsuleColliderComponent>();

                    cc3d.Offset = sphereColliderComponent["Offset"].as<Vector3D>();
                    cc3d.Radius = sphereColliderComponent["Radius"].as<float>();
                    cc3d.Density = sphereColliderComponent["Density"].as<float>();
                    cc3d.Friction = sphereColliderComponent["Friction"].as<float>();
                    cc3d.Restitution = sphereColliderComponent["Restitution"].as<float>();
                    cc3d.RestitutionThreshold = sphereColliderComponent["RestitutionThreshold"].as<float>();
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
    bool SceneSerializer::DeserializeJSON(std::string_view filepath)
    {
        std::ifstream stream(filepath.data());
        if (!stream.is_open())
            return false;
        Json::Value root;
        Json::CharReaderBuilder builder;
        JSONCPP_STRING errs;
        if (!Json::parseFromStream(builder, stream, &root, &errs))
            return false;
        if (!root.isMember("Scene"))
            return false;

        if (root.isMember("Entities") && root["Entities"].isArray())
        {
            for (auto& entity : root["Entities"])
            {
                uint64_t uuid = entity["Entity"].asUInt64();
                std::string name;

                if (entity.isMember("TagComponent"))
                {
                    name = entity["TagComponent"]["Tag"].asString();
                }

                EntityECS deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

                if (entity.isMember("TransformComponent"))
                {
                    auto& tc = entity["TransformComponent"];
                    auto& transform = deserializedEntity.GetComponent<TransformComponent>();
                    transform.Translation = JSONBackend::ReadVec3(tc["Translation"]);
                    transform.Rotation = MathUtils::ConvertToRadiansVec3(JSONBackend::ReadVec3(tc["Rotation"]));
                    transform.Scale = JSONBackend::ReadVec3(tc["Scale"]);
                }

                if (entity.isMember("RigidBody3DComponent"))
                {
                    auto& rb = entity["RigidBody3DComponent"];
                    auto& rb3d = deserializedEntity.AddComponent<RigidBody3D>();
                    rb3d.Type = Rigidbody3DBodTypeFromString(rb["Type"].asString());

                }

                if (entity.isMember("Box3DColliderComponent"))
                {
                    auto& bc = entity["Box3DColliderComponent"];
                    auto& bc3d = deserializedEntity.AddComponent<Box3DColliderComponent>();
                    bc3d.Offset = JSONBackend::ReadVec3(bc["Offset"]);
                    bc3d.Size = JSONBackend::ReadVec3(bc["Size"]);
                    bc3d.Density = bc["Density"].asFloat();
                    bc3d.Friction = bc["Friction"].asFloat();
                    bc3d.Restitution = bc["Restitution"].asFloat();
                    bc3d.RestitutionThreshold = bc["RestitutionThreshold"].asFloat();
                }
                if (entity.isMember("SphereColliderComponent"))
                {
                    auto& sc = entity["SphereColliderComponent"];
                    auto& sc3d = deserializedEntity.AddComponent<SphereColliderComponent>();
                    sc3d.Offset = JSONBackend::ReadVec3(sc["Offset"]);
                    sc3d.Radius = sc["Radius"].asFloat();
                    sc3d.Density = sc["Density"].asFloat();
                    sc3d.Friction = sc["Friction"].asFloat();
                    sc3d.Restitution = sc["Restitution"].asFloat();
                    sc3d.RestitutionThreshold = sc["RestitutionThreshold"].asFloat();
                }
                if (entity.isMember("CapsuleColliderComponent"))
                {
                    auto& cc = entity["CapsuleColliderComponent"];
                    auto& cc3d = deserializedEntity.AddComponent<CapsuleColliderComponent>();
                    cc3d.Offset = JSONBackend::ReadVec3(cc["Offset"]);
                    cc3d.Radius = cc["Radius"].asFloat();
                    cc3d.Height = cc["Height"].asFloat();
                    cc3d.Density = cc["Density"].asFloat();
                    cc3d.Friction = cc["Friction"].asFloat();
                    cc3d.Restitution = cc["Restitution"].asFloat();
                    cc3d.RestitutionThreshold = cc["RestitutionThreshold"].asFloat();
                }

                if (entity.isMember("MeshRendererComponent"))
                {
                    auto& mesh = entity["MeshRendererComponent"];
                    auto& meshComponent = deserializedEntity.AddComponent<MeshRendererComponent>();
                    meshComponent.MeshA = (AssetHandle)mesh["MeshHandle"].asUInt64();
                    meshComponent.TextureA = (AssetHandle)mesh["TextureHandle"].asUInt64();
                    meshComponent.Type = SourceTypeFromString(mesh["SourceType"].asString());
                    meshComponent.PType = PrimitiveTypeFromString(mesh["PrimitiveType"].asString());

                    if (meshComponent.MeshA == 0 && meshComponent.Type == MeshRendererComponent::SourceType::Primitive)
                    {
                       
                        switch (meshComponent.PType)
                        {
                        case MeshRendererComponent::PrimitiveType::Cube: meshComponent.MeshA = Project::GetActive()->GetEditorAssetManager()->CreatePrimitiveAsset(PrimitiveTypes::Cube); break;
                        case MeshRendererComponent::PrimitiveType::Sphere: meshComponent.MeshA = Project::GetActive()->GetEditorAssetManager()->CreatePrimitiveAsset(PrimitiveTypes::Sphere); break;
                        case MeshRendererComponent::PrimitiveType::Plane: 
                            meshComponent.MeshA = Project::GetActive()->GetEditorAssetManager()->CreatePrimitiveAsset(PrimitiveTypes::Plane);
                            break;
                        case MeshRendererComponent::PrimitiveType::Capsule: meshComponent.MeshA = Project::GetActive()->GetEditorAssetManager()->CreatePrimitiveAsset(PrimitiveTypes::Capsule); break;
                        }
                    }
                }
            }
        }
       
        return true;
    }
}