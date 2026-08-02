#pragma once
#include <string>
#include <filesystem>

#include "Sign/Asset/AssetMangerBase.h"
#include "Sign/Asset/EditorAssetManager.h"

namespace Sign {
	struct ProjectConfig {
		std::string Name = "Untitled";

		std::filesystem::path StartScene;

		std::filesystem::path AssetDirectory;
		std::filesystem::path AssetRegistryPath;
		std::filesystem::path MeshPath;

	};
	class Project {
	public:
		Project();

		static const std::filesystem::path& GetProjectDirectory()
		{
			return s_ActiveProject->m_ProjectDirectory;
		}

		static std::filesystem::path GetAssetDirectory()
		{
			return GetProjectDirectory() / s_ActiveProject->m_Configs.AssetDirectory;
		}

		static std::filesystem::path GetAssetRegistryPath() { return GetAssetDirectory() / s_ActiveProject->m_Configs.AssetRegistryPath; }

		static std::filesystem::path GetAssetAbsolutePath(const std::filesystem::path& path);

		static const std::filesystem::path& GetActiveProjectDirectory()
		{
			return s_ActiveProject->GetProjectDirectory();
		}
		static std::filesystem::path GetActiveAssetDirectory()
		{
			return s_ActiveProject->GetAssetDirectory();
		}

		static std::filesystem::path GetActiveAssetRegistryPath()
		{
			return s_ActiveProject->GetAssetRegistryPath();
		}

		static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path)
		{
			return GetAssetDirectory() / path;
		}

		
		ProjectConfig& GetConfig()
		{
			return m_Configs;
		}

		static std::shared_ptr<Project> GetActive() { return s_ActiveProject; }
		std::shared_ptr<AssetMangerBase> GetAssetManager() { return m_AssetManager; }
		std::shared_ptr<EditorAssetManager> GetEditorAssetManager() { return std::static_pointer_cast<EditorAssetManager>(m_AssetManager); }

		static std::shared_ptr<Project> New();
		static std::shared_ptr<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);
	private:
		ProjectConfig m_Configs;
		std::filesystem::path m_ProjectDirectory;
		std::shared_ptr<AssetMangerBase> m_AssetManager;
		inline static std::shared_ptr<Project> s_ActiveProject;
	};
}