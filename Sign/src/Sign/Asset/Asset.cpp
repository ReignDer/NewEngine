#include "signpch.h"
#include "Asset.h"


namespace Sign {
	std::string_view Sign::AssetTypeToString(AssetType type)
	{
		switch (type)
		{
			case AssetType::None: return "AssetType::None";
			case AssetType::Texture2D: return "AssetType::Texture2D";
			case AssetType::Mesh: return "AssetType::Mesh";
		}

		return "AssetType::<Invalid>";
	}

	AssetType Sign::AssetTypeFromString(std::string_view assetType)
	{
		if (assetType == "AssetType::None") return AssetType::None;
		if (assetType == "AssetType::Texture2D") return AssetType::Texture2D;
		if (assetType == "AssetType::Mesh") return AssetType::Mesh;

		return AssetType::None;
	}
}
