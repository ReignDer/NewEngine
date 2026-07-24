#pragma once
#include "Asset.h"

#include <map>

namespace Sign {
	using AssetMap = std::map<AssetHandle, std::shared_ptr<Asset>>;

	class AssetMangerBase
	{
	public:
		virtual std::shared_ptr<Asset> GetAsset(AssetHandle handle) = 0;
		virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
		virtual bool IsAssetLoaded(AssetHandle handle) const = 0;
		virtual AssetType GetAssetType(AssetHandle handle) const = 0;

	};
}

