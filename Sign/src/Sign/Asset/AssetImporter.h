#pragma once
#include "Asset.h"
#include "AssetMetaData.h"
namespace Sign {
	class AssetImporter
	{
	public:
		static std::shared_ptr<Asset> ImportAsset(AssetHandle handle, const AssetMetaData& metadata);
	};
}

