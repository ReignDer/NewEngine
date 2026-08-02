#pragma once
#include "Sign/Renderer/Texture.h"
#include "Asset.h"
#include "AssetMetaData.h"
namespace Sign {
	class TextureImporter
	{
	public:
		static std::shared_ptr<Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetaData& metadata);
		static std::shared_ptr<Texture2D> LoadTexture2D(const std::filesystem::path& path);
	};
}

