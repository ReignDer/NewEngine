#include "signpch.h"
#include "ContentBrowserPanel.h"
#include <imgui.h>

#include "Sign/Project/Project.h"
#include "Sign/Asset/TextureImporter.h"
namespace Sign {

	ContentBrowserPanel::ContentBrowserPanel() : m_BaseDirectory(Project::GetAssetDirectory()), m_CurrentDirectory(m_BaseDirectory)
	{
		m_TreeNodes.push_back(TreeNode(".", 0));
		m_DirectoryIcon = TextureImporter::LoadTexture2D("SignEditor/Resources/Icon/Directoryfolder.png");
		m_FileIcon = TextureImporter::LoadTexture2D("SignEditor/Resources/Icon/FileIcon.png");

		RefreshAssetTree();

		m_Mode = Mode::FileSystem;
	}
	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		const char* label = m_Mode == Mode::Asset ? "Asset" : "File";
		if (ImGui::Button(label))
		{
			m_Mode = m_Mode == Mode::Asset ? Mode::FileSystem : Mode::Asset;
		}
		
		if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory))
		{
			ImGui::SameLine();
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();

			}
		}
		float padding = 16.0f;
		float thumbnailSize = 126;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;
		if (ImGui::BeginTable("Table", columnCount, ImGuiTableFlags_NoBordersInBody)) {
			
			if (m_Mode == Mode::Asset) {

				TreeNode* node = &m_TreeNodes[0];

				auto currentDir = std::filesystem::relative(m_CurrentDirectory, Project::GetActiveAssetDirectory());

				for (const auto& p : currentDir)
				{
					if (node->Path == currentDir)
						break;

					if (node->Children.find(p) != node->Children.end())
					{
						node = &m_TreeNodes[node->Children[p]];
						continue;
					}
					else
					{

					}
				}

				for (const auto& [item, treeNodeIndex] : node->Children)
				{
					ImGui::TableNextColumn();
					bool isDirectory = std::filesystem::is_directory(Project::GetActiveAssetDirectory() / item);

					std::string itemStr = item.generic_string();

					ImGui::PushID(itemStr.c_str());
					std::shared_ptr<Texture2D> icon = isDirectory ? m_DirectoryIcon : m_FileIcon;
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
					ImGui::ImageButton(itemStr.c_str(), (ImTextureID)icon->GetGpuHandle().ptr, { thumbnailSize,thumbnailSize });

					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("Delete"))
						{
							m_PendingDeleteHandle = m_TreeNodes[treeNodeIndex].Handle;
						}
						ImGui::EndPopup();
					}
					if (ImGui::BeginDragDropSource())
					{
						AssetHandle handle = m_TreeNodes[treeNodeIndex].Handle;
						ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", &handle, sizeof(AssetHandle));
						ImGui::EndDragDropSource();
					}
					ImGui::PopStyleColor();
					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						if (isDirectory)
						{
							m_CurrentDirectory /= item.filename();
						}
					}

					ImGui::TextWrapped(itemStr.c_str());

					ImGui::PopID();

				}
	
			}
			else
			{

				for (auto& it : std::filesystem::directory_iterator(m_CurrentDirectory))
				{
					ImGui::TableNextColumn();
					const auto& path = it.path();
					std::string filenameString = path.filename().string();

					ImGui::PushID(filenameString.c_str());
					

					std::shared_ptr<Texture2D> icon = it.is_directory() ? m_DirectoryIcon : m_FileIcon;
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
					ImGui::ImageButton(filenameString.c_str(), (ImTextureID)icon->GetGpuHandle().ptr, { thumbnailSize,thumbnailSize });

					ImGui::PopStyleColor();
					auto relativePath = std::filesystem::relative(path, Project::GetActiveAssetDirectory());
			

					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("Import"))
						{
							m_PendingImportPath = relativePath;

						}
						ImGui::EndPopup();
					}
					

					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						if (it.is_directory())
						{
							m_CurrentDirectory /= path.filename();
						}
					}

					ImGui::TextWrapped(filenameString.c_str());
					
					ImGui::PopID();
				
					

				}
			}
			ImGui::EndTable();
		}

		if (m_PendingDeleteHandle != 0)
		{
			Project::GetActive()->GetEditorAssetManager()->DeleteAsset(m_PendingDeleteHandle);
			m_PendingDeleteHandle = 0;
			RefreshAssetTree();
		}
		if (!m_PendingImportPath.empty())
		{
			Project::GetActive()->GetEditorAssetManager()->ImportAsset(m_PendingImportPath);
			m_PendingImportPath.clear();
			RefreshAssetTree();
		}
		ImGui::End();
	}
	void ContentBrowserPanel::RefreshAssetTree()
	{
		m_TreeNodes.clear();
		m_TreeNodes.push_back(TreeNode(".", 0));
		const auto& assetRegistry = Project::GetActive()->GetEditorAssetManager()->GetAssetRegistry();

		for (const auto& [handle, metadata] : assetRegistry)
		{
			uint32_t currentNodeIndex = 0;
			std::println("Filepath for {}", metadata.Filepath.string());
			for (const auto& p : metadata.Filepath)
			{
				auto it = m_TreeNodes[currentNodeIndex].Children.find(p.generic_string());

				if (it != m_TreeNodes[currentNodeIndex].Children.end())
				{
					currentNodeIndex = it->second;
				}
				else
				{
					TreeNode newNode(p, handle);
					newNode.Parent = currentNodeIndex;
					m_TreeNodes.push_back(newNode);

					m_TreeNodes[currentNodeIndex].Children[p] = m_TreeNodes.size() - 1;
					currentNodeIndex = m_TreeNodes.size() - 1;
				}
				std::println("To {}", p.string());
			}
			
		}
	}
}