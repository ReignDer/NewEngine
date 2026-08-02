#include "signpch.h"
#include "TextureImporter.h"
#include "Sign/Project/Project.h"

namespace Sign {
    std::shared_ptr<Texture2D> Sign::TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetaData& metadata)
    {
        return LoadTexture2D(Project::GetActiveAssetDirectory() / metadata.Filepath);
    }

    std::shared_ptr<Texture2D> Sign::TextureImporter::LoadTexture2D(const std::filesystem::path& path)
    {
        return std::make_shared<Texture2D>(path.string());
    }
}
