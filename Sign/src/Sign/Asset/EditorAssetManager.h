#pragma once
#include "AssetMangerBase.h"
#include <map>
#include "AssetMetaData.h"
namespace Sign {

	using AssetRegistry = std::map<AssetHandle, AssetMetaData>;

	class EditorAssetManager : public AssetMangerBase
	{
	public:
		// Inherited via AssetMangerBase
		
		virtual std::shared_ptr<Asset> GetAsset(AssetHandle handle) override;
		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;
		AssetType GetAssetType(AssetHandle handle) const override;
		void ImportAsset(const std::filesystem::path& filepath);
		AssetHandle CreatePrimitiveAsset(PrimitiveTypes type);
		const AssetMetaData& GetMetaData(AssetHandle handle) const;
		const AssetRegistry& GetAssetRegistry() const { return m_AssetRegistry; }

		void SerializeAssetRegistry();
		bool DeserializeAssetRegistry();

	private:
		AssetRegistry m_AssetRegistry;
		AssetMap m_LoadedAssets;
	};
}
