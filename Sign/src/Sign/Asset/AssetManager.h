#pragma once
#include "AssetMangerBase.h"
namespace Sign {
	class AssetManager : public AssetMangerBase
	{
	public:
		template<typename T>
		static std::shared_ptr<T> GetAsset(AssetHandle handle) {
			std::shared_ptr<Asset> asset;
			return std::static_pointer_cast<T>(asset);
		}

		// Inherited via AssetMangerBase
		std::shared_ptr<Asset> GetAsset(AssetHandle handle) override;
		bool IsAssetHandleValid(AssetHandle handle) const override;
		bool IsAssetLoaded(AssetHandle handle) const override;
		AssetType GetAssetType(AssetHandle handle) const override;
	};
}
