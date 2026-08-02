#include "signpch.h"
#include "SceneHierarchy.h"
#include <imgui.h>
#include <imgui_internal.h>

#include "Sign/Asset/AssetManager.h"
#include "Sign/Renderer/Primitive3D.h"
namespace Sign {
	SceneHierarchy::SceneHierarchy(const std::shared_ptr<Scene>& scene)
	{
		SetContext(scene);
	}
	SceneHierarchy::~SceneHierarchy()
	{
	}
	void SceneHierarchy::SetContext(const std::shared_ptr<Scene>& scene)
	{
		m_Context = scene;
		m_SelectedEntity = {};
	}
	void SceneHierarchy::OnImGuiRender()
	{

		ImGui::Begin("Scene Hierarchy");

		m_Context->m_Registry.each([&](EntityID entityID) {
			EntityECS entity(entityID, m_Context.get());
			DrawTreeNode(entity);
			
		});

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
			m_SelectedEntity = {};
		}

		if (ImGui::BeginPopupContextWindow("PopupContext"))
		{
			if (ImGui::MenuItem("Create Empty Entity")) {
				m_Context->CreateEntity("Empty Entity");
			}

			
			if (ImGui::BeginMenu("Primitives"))
			{
				if (ImGui::MenuItem("Create Cube"))
				{
					EntityECS entity = m_Context->CreateEntity();
					entity.AddComponent<MeshRendererComponent>();
					auto& mesh = entity.GetComponent<MeshRendererComponent>();
					auto cube = Primitive::Cube3D::Create();

					
				}

				if (ImGui::MenuItem("Create Sphere"))
				{

				}

				if (ImGui::MenuItem("Create Plane"))
				{

				}

				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
		ImGui::End();

		ImGui::Begin("Inspector");
		if (m_SelectedEntity) {
			DrawComponents(m_SelectedEntity);
		}
		ImGui::End();
	}
	void SceneHierarchy::DrawTreeNode(EntityECS entity)
	{
		auto& tag= entity.GetComponent<TagComponent>().Tag;
		ImGuiTreeNodeFlags flags = (m_SelectedEntity == entity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked()) {
			m_SelectedEntity = entity;
		}

		if (opened) {
			ImGui::TreePop();

		}
	}
	static void DrawVec3Control(std::string_view label, Vector3D& vec, float resetValue = 0.0f, float columnWidth = 100.0f) {
		if (ImGui::BeginTable("Table", 2, ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_SizingFixedFit)) {

			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 2.0f); // Default twice larger

			ImGui::TableNextColumn();
			ImGui::PushID(label.data());
			ImGui::Text(label.data());
			ImGui::TableNextColumn();

			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

			float lineHeight = ImGui::GetFontSize() + GImGui->Style.FramePadding.y * 2.0f;

			ImVec2 buttonSize = { lineHeight + 2.0f, lineHeight };
			if (ImGui::Button("X", buttonSize))
				vec.x = resetValue;
			ImGui::SameLine();
			ImGui::DragFloat("##X", &vec.x, 0.1f, 0.0f, 0.0f,"%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();

			if (ImGui::Button("Y", buttonSize))
				vec.y = resetValue;
			ImGui::SameLine();
			ImGui::DragFloat("##Y", &vec.y, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();


			if (ImGui::Button("Z", buttonSize))
				vec.z = resetValue;
			ImGui::SameLine();
			ImGui::DragFloat("##Z", &vec.z, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();

			ImGui::PopStyleVar();

			ImGui::PopID();
			

			ImGui::EndTable();
		}
		
	}

	template <typename T, typename UIFunction>
	static void DrawComponent(std::string_view name, EntityECS entity, UIFunction uiFunction) {
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_FramePadding;

		if (entity.HasComponent<T>()) {
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
			float lineHeight = ImGui::GetFontSize() + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.data());
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5);
			if (ImGui::Button("+", ImVec2{ lineHeight,lineHeight })) {
				ImGui::OpenPopup("ComponentSettings");
			}
			ImGui::PopStyleVar();

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings")) {
				if (ImGui::MenuItem("Remove Component"))
					removeComponent = true;
				ImGui::EndPopup();
			}

			if (open) {
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent) {
				entity.RemoveComponent<T>();
			}
		}
	}

	template<typename T>
	void SceneHierarchy::DisplayAddComponentEntry(std::string_view entryName) {
		if (!m_SelectedEntity.HasComponent<T>()) {
			if (ImGui::MenuItem(entryName.data()))
			{
				m_SelectedEntity.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}
	
	void SceneHierarchy::DrawComponents(EntityECS entity)
	{
		if (entity.HasComponent<TagComponent>()) {
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());

			if (ImGui::InputText("##Name", buffer, sizeof(buffer))) {
				tag = std::string(buffer);
			}
			
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			DisplayAddComponentEntry<TransformComponent>("Transform");
			DisplayAddComponentEntry<RigidBody3D>("Rigidbody 3D");
			DisplayAddComponentEntry<Box3DColliderComponent>("Box3D Collider");
			DisplayAddComponentEntry<SphereColliderComponent>("Sphere Collider");
			DisplayAddComponentEntry<MeshRendererComponent>("Mesh Renderer");
			
			ImGui::EndPopup();
		}
		DrawComponent<TransformComponent>("Transform", entity, [](auto& component) 
		{
			DrawVec3Control("Position", component.Translation);
			Vector3D rotation = MathUtils::ConvertToDegreesVec3(component.Rotation);
			DrawVec3Control("Rotation", rotation);
			component.Rotation = MathUtils::ConvertToRadiansVec3(rotation);
			DrawVec3Control("Scale", component.Scale, 1.0f);
		});

		DrawComponent<RigidBody3D>("Rigidbody 3D", entity, [](auto& component)
		{
			const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
			const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];

			if (ImGui::BeginCombo("Body Type", currentBodyTypeString)) 
			{
				for (int i = 0; i < 3; i++) 
				{
					bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
					if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
					{
						currentBodyTypeString = bodyTypeStrings[i];
						component.Type = (RigidBody3D::BodyType)i;
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
				
			}			
		});

		DrawComponent<Box3DColliderComponent>("Box3D Collider", entity, [](auto& component)
		{
			ImGui::DragFloat3("Offset", MathUtils::value_ptr(component.Offset));
			ImGui::DragFloat3("Size", MathUtils::value_ptr(component.Size));
			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
		});

		DrawComponent<SphereColliderComponent>("Sphere Collider", entity, [](auto& component)
		{
			ImGui::DragFloat3("Offset", MathUtils::value_ptr(component.Offset));
			ImGui::DragFloat3("Radius", &component.Radius);
			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
		});

		DrawComponent<MeshRendererComponent>("Mesh Renderer", entity, [](auto& component)
		{
			std::string meshLabel = "None";
			std::string texture2DLabel = "None";
			bool isMeshValid = false;
			bool isTexture2DValid = false;


			ImGui::PushID("Mesh");
			if (component.MeshA != 0)
			{
				if (AssetManager::IsAssetHandleValid(component.MeshA)
					&& AssetManager::GetAssetType(component.MeshA) == AssetType::Mesh)
				{
					const AssetMetaData& metadata = Project::GetActive()->GetEditorAssetManager()->GetMetaData(component.MeshA);
					meshLabel = metadata.Filepath.filename().string();
					isMeshValid = true;
				}
			}
			else
			{
				meshLabel = "Invalid";
			}

			if (component.TextureA != 0)
			{
				if (AssetManager::IsAssetHandleValid(component.TextureA)
					&& AssetManager::GetAssetType(component.TextureA) == AssetType::Texture2D)
				{
					const AssetMetaData& metadata = Project::GetActive()->GetEditorAssetManager()->GetMetaData(component.TextureA);
					texture2DLabel = metadata.Filepath.filename().string();
					isTexture2DValid = true;
				}
			}
			else
			{
				texture2DLabel = "Invalid";
			}

			ImVec2 MeshbuttonLabelSize = ImGui::CalcTextSize(meshLabel.c_str());
			MeshbuttonLabelSize.x += 20.0f;
			float MeshbuttonLabelWidth = (std::max<float>)(100.0f, MeshbuttonLabelSize.x);

			ImGui::Button(meshLabel.c_str(), ImVec2(MeshbuttonLabelWidth, 0.0f));
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					AssetHandle handle = *(AssetHandle*)payload->Data;
					if (AssetManager::GetAssetType(handle) == AssetType::Mesh)
					{
						component.MeshA = handle;
					}
					else
					{

					}
				}
				ImGui::EndDragDropTarget();
			}

			if (isMeshValid)
			{
				ImGui::SameLine();
				ImVec2 xMeshLabelSize = ImGui::CalcTextSize("X");
				float MeshbuttonSize = xMeshLabelSize.y + ImGui::GetStyle().FramePadding.y * 2.0f;
				if (ImGui::Button("X", ImVec2(MeshbuttonSize, MeshbuttonSize)))
				{
					component.MeshA = 0;
				}
			}

			ImGui::SameLine();
			ImGui::Text("Mesh");
			ImGui::PopID();

			ImGui::PushID("Texture");
			ImVec2 Texture2DbuttonLabelSize = ImGui::CalcTextSize(texture2DLabel.c_str());
			Texture2DbuttonLabelSize.x += 20.0f;
			float Texture2DbuttonLabelWidth = (std::max<float>)(100.0f, Texture2DbuttonLabelSize.x);

			ImGui::Button(texture2DLabel.c_str(), ImVec2(Texture2DbuttonLabelWidth, 0.0f));
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					AssetHandle handle = *(AssetHandle*)payload->Data;
					if (AssetManager::GetAssetType(handle) == AssetType::Texture2D)
					{
						component.TextureA = handle;
					}
					else
					{

					}
				}
				ImGui::EndDragDropTarget();
			}

			if (isTexture2DValid)
			{
				ImGui::SameLine();
				ImVec2 xTexture2DLabelSize = ImGui::CalcTextSize("X");
				float Texture2DbuttonSize = xTexture2DLabelSize.y + ImGui::GetStyle().FramePadding.y * 2.0f;
				if (ImGui::Button("X", ImVec2(Texture2DbuttonSize, Texture2DbuttonSize)))
				{
					component.TextureA = 0;
				}
			}

			ImGui::SameLine();
			ImGui::Text("Texture");
			ImGui::PopID();
		});

	}
}