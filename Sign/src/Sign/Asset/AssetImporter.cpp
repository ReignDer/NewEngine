#include "signpch.h"
#include "AssetImporter.h"

#include "TextureImporter.h"
#include "MeshImporter.h"
#include <map>
namespace Sign {

    using AssetImportFunction = std::function <std::shared_ptr<Asset>(AssetHandle, const AssetMetaData&)>;
    static std::map<AssetType, AssetImportFunction> s_AssetImportFunctions = {
        {AssetType::Texture2D, TextureImporter::ImportTexture2D },
        {AssetType::Mesh, MeshImporter::ImportMesh}
    };

    std::shared_ptr<Asset> Sign::AssetImporter::ImportAsset(AssetHandle handle, const AssetMetaData& metadata)
    {
       if(s_AssetImportFunctions.find(metadata.Type) == s_AssetImportFunctions.end())
       {
           return nullptr;
       }

       return s_AssetImportFunctions.at(metadata.Type)(handle, metadata);
    }
}
