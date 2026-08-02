#pragma once
#include "Asset.h"

#include <filesystem>
namespace Sign {
	struct AssetMetaData {
		AssetType Type = AssetType::None;

		std::filesystem::path Filepath;

		operator bool() const { return Type != AssetType::None; }
	};
}