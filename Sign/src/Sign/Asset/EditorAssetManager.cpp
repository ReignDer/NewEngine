#include "signpch.h"
#include "EditorAssetManager.h"
#include "AssetImporter.h"

#include "Sign/Project/Project.h"

#include <yaml-cpp/yaml.h>
#include <fstream>
namespace Sign {
	static std::map<std::filesystem::path, AssetType> s_AssetExtensionMap =
	{
		{".obj", AssetType::Mesh},
		{".sign", AssetType::Scene},
		{".png", AssetType::Texture2D},
		{".jpg", AssetType::Texture2D},
		{".jpeg", AssetType::Texture2D}
	};
	static AssetType GetAssetFileTypeFromExtension(const std::filesystem::path& extension)
	{
		if (s_AssetExtensionMap.find(extension) == s_AssetExtensionMap.end())
		{
			return AssetType::None;
		}

		return s_AssetExtensionMap.at(extension);
	}

	static std::string PrimitiveTypeToString(PrimitiveTypes type)
	{
		switch (type)
		{
			case PrimitiveTypes::None: return "None";
			case PrimitiveTypes::Cube: return "Cube";
			case PrimitiveTypes::Sphere: return "Sphere";
			case PrimitiveTypes::Plane: return "Plane";
		}
		return "None";
	}
	static PrimitiveTypes PrimitiveTypeFromString(std::string_view type)
	{
		if (type == "None") return PrimitiveTypes::None;
		if (type == "Cube") return PrimitiveTypes::Cube;
		if (type == "Sphere") return PrimitiveTypes::Sphere;
		if (type == "Plane") return PrimitiveTypes::Plane;

		return PrimitiveTypes::None;
	}
	bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle) const
	{

		return handle != 0 && m_AssetRegistry.contains(handle);
	}
	bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		return m_LoadedAssets.contains(handle);
	}
	AssetType EditorAssetManager::GetAssetType(AssetHandle handle) const
	{
		if (!IsAssetHandleValid(handle))
			return AssetType::None;
		return m_AssetRegistry.at(handle).Type;
	}
	void EditorAssetManager::ImportAsset(const std::filesystem::path& filepath)
	{
		AssetHandle handle;
		AssetMetaData metadata;
		metadata.Filepath = filepath;
		metadata.Type = GetAssetFileTypeFromExtension(filepath.extension());
		std::shared_ptr<Asset> asset = AssetImporter::ImportAsset(handle, metadata);

		if (asset) {
			asset->Handle = handle;
			m_LoadedAssets[handle] = asset;
			m_AssetRegistry[handle] = metadata;
			SerializeAssetRegistry();
		}
			
	}
	AssetHandle EditorAssetManager::CreatePrimitiveAsset(PrimitiveTypes type)
	{
		AssetHandle handle;
		AssetMetaData metadata;
		metadata.Filepath = "";
		metadata.PrimitiveType = type;
		metadata.Type = AssetType::Mesh;
		std::shared_ptr<Asset> asset = AssetImporter::ImportAsset(handle, metadata);

		if (!asset)
			return 0;
		
		asset->Handle = handle;
		m_LoadedAssets[handle] = asset;
		m_AssetRegistry[handle] = metadata;
		//SerializeAssetRegistry();

		return handle;
		
	}
	const AssetMetaData& EditorAssetManager::GetMetaData(AssetHandle handle) const
	{
		static AssetMetaData s_NullMetadata;
		auto it = m_AssetRegistry.find(handle);
		if (it == m_AssetRegistry.end())
			return s_NullMetadata;

		return it->second;
	}
	void EditorAssetManager::DeleteAsset(AssetHandle handle)
	{
		if (!IsAssetHandleValid(handle))
			return;

		m_LoadedAssets.erase(handle);
		m_AssetRegistry.erase(handle);
		SerializeAssetRegistry();
	}
	void EditorAssetManager::SerializeAssetRegistry()
	{
		auto path = Project::GetActiveAssetRegistryPath();

		YAML::Emitter out;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "AssetRegistry" << YAML::Value;

			out << YAML::BeginSeq;

			for (const auto& [handle, metadata] : m_AssetRegistry)
			{
				out << YAML::BeginMap;
				out << YAML::Key << "Handle" << YAML::Value << handle;
				std::string filepathStr = metadata.Filepath.generic_string();
				out << YAML::Key << "FilePath" << YAML::Value << filepathStr;
				out << YAML::Key << "Type" << YAML::Value << AssetTypeToString(metadata.Type);
				out << YAML::Key << "PrimitiveType" << YAML::Value << PrimitiveTypeToString(metadata.PrimitiveType);
				out << YAML::EndMap;
			}

			out << YAML::EndSeq;
			out << YAML::EndMap;
		}

		std::ofstream fout(path);

		fout << out.c_str();
	}
	bool EditorAssetManager::DeserializeAssetRegistry()
	{
		auto path = Project::GetActiveAssetRegistryPath();
		YAML::Node data;

		try {
			data = YAML::LoadFile(path.string());

		}
		catch (YAML::ParserException e) {
			return false;
		}

		auto rootNode = data["AssetRegistry"];
		if (!rootNode)
			return false;

		for (const auto& node : rootNode)
		{
			AssetHandle handle = node["Handle"].as<uint64_t>();
			auto& metadata = m_AssetRegistry[handle];
			metadata.Filepath = node["FilePath"].as<std::string>();
			metadata.Type = AssetTypeFromString(node["Type"].as<std::string>());
			metadata.PrimitiveType = PrimitiveTypeFromString(node["PrimitiveType"].as<std::string>());
		}

		return true;
	}
	std::shared_ptr<Asset> EditorAssetManager::GetAsset(AssetHandle handle)
	{
		if (!IsAssetHandleValid(handle))
			return nullptr;

		std::shared_ptr<Asset> asset;
		if (IsAssetLoaded(handle))
		{
			asset = m_LoadedAssets.at(handle);
		}
		else
		{
			const AssetMetaData& metadata = GetMetaData(handle);
			asset = AssetImporter::ImportAsset(handle, metadata);

			if(!asset){}

			m_LoadedAssets[handle] = asset;
		}

		return asset;
	}
}