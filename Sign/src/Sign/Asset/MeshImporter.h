#pragma once

#include <filesystem>
#include "Sign/Renderer/Mesh.h"
#include "Asset.h"
namespace Sign{
class MeshImporter
{
public:

	static std::shared_ptr<Mesh> ImportMesh(AssetHandle handle);
	static std::shared_ptr<Mesh> LoadMesh(const std::filesystem::path& path);
};
}
