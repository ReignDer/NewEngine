#pragma once
#include "Asset.h"
#include "Sign/Renderer/Primitive3D.h"
#include <filesystem>
namespace Sign {
	struct AssetMetaData {
		AssetType Type = AssetType::None;

		std::filesystem::path Filepath;

		PrimitiveTypes PrimitiveType = PrimitiveTypes::None;

		operator bool() const { return Type != AssetType::None; }
	};
}