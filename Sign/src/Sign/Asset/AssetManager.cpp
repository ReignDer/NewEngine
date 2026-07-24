#include "signpch.h"
#include "AssetManager.h"

namespace Sign {
	std::shared_ptr<Asset> AssetManager::GetAsset(AssetHandle handle)
	{
		return std::shared_ptr<Asset>();
	}
	bool AssetManager::IsAssetHandleValid(AssetHandle handle) const
	{
		return false;
	}
	bool AssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		return false;
	}
	AssetType AssetManager::GetAssetType(AssetHandle handle) const
	{
		return AssetType();
	}
}