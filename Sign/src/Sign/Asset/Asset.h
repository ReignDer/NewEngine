#pragma once
#include "Sign/UUID.h"
namespace Sign {
	using AssetHandle = UUID;

	enum class AssetType {
		None = 0,
		Scene,
		Mesh,
		Texture2D
	};

	std::string_view AssetTypeToString(AssetType type);
	AssetType AssetTypeFromString(std::string_view assetType);
	class Asset
	{
	public:
		AssetHandle Handle;

		virtual AssetType GetType() const = 0;
	};
}

