#include "signpch.h"
#include "SceneHierarchy.h"
#include <imgui.h>
#include <imgui_internal.h>

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

			ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };
			if (ImGui::Button("X", buttonSize))
				vec.x = resetValue;
			ImGui::SameLine();
			ImGui::DragFloat("##X", &vec.x, 0.1f);
			ImGui::PopItemWidth();
			ImGui::SameLine();

			if (ImGui::Button("Y", buttonSize))
				vec.y = resetValue;
			ImGui::SameLine();
			ImGui::DragFloat("##Y", &vec.y, 0.1f);
			ImGui::PopItemWidth();
			ImGui::SameLine();


			if (ImGui::Button("Z", buttonSize))
				vec.z = resetValue;
			ImGui::SameLine();
			ImGui::DragFloat("##Z", &vec.z, 0.1f);
			ImGui::PopItemWidth();

			ImGui::PopStyleVar();

			ImGui::PopID();
			

			ImGui::EndTable();
		}
		
	}
	void SceneHierarchy::DrawComponents(EntityECS entity)
	{
		if (entity.HasComponent<TagComponent>()) {
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());

			if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
				tag = std::string(buffer);
			}
			
		}

		if (entity.HasComponent<TransformComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,"Transform")) {
				auto& transform = entity.GetComponent<TransformComponent>();
				DrawVec3Control("Position", transform.Translation);
				Vector3D rotation = MathUtils::ConvertToDegreesVec3(transform.Rotation);
				DrawVec3Control("Rotation", rotation);
				transform.Rotation = MathUtils::ConvertToRadiansVec3(rotation);
				DrawVec3Control("Scale", transform.Scale, 1.0f);
				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<MeshRendererComponent>())
		{
			uint32_t selectedFaceID = m_Context->GetSelectedFaceID();

			if (selectedFaceID != UINT32_MAX)
			{
				if (selectedFaceID != m_LastEditedFaceID)
				{
					m_FaceEditOffset = Vector3D(0.0f, 0.0f, 0.0f);
					m_LastEditedFaceID = selectedFaceID;
				}

				if (ImGui::TreeNodeEx((void*)typeid(MeshRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Face"))
				{
					Vector3D prevOffset = m_FaceEditOffset;
					DrawVec3Control("Position", m_FaceEditOffset);
					Vector3D deltaTranslate = m_FaceEditOffset - prevOffset;

					Vector3D prevRotation = m_FaceRotationOffset;
					DrawVec3Control("Rotation", m_FaceRotationOffset);
					Vector3D deltaRotationDeg = m_FaceRotationOffset - prevRotation;

					Vector3D prevScale = m_FaceScaleOffset;
					DrawVec3Control("Scale", m_FaceScaleOffset, 1.0f);
					Vector3D deltaScale = m_FaceScaleOffset - prevScale; 

					bool changed = deltaTranslate.x != 0.0f || deltaTranslate.y != 0.0f || deltaTranslate.z != 0.0f
						|| deltaRotationDeg.x != 0.0f || deltaRotationDeg.y != 0.0f || deltaRotationDeg.z != 0.0f
						|| deltaScale.x != 0.0f || deltaScale.y != 0.0f || deltaScale.z != 0.0f;

					if (changed)
					{
						Vector3D deltaRotationRad = MathUtils::ConvertToRadiansVec3(deltaRotationDeg);
	
						Vector3D scaleMultiplier = Vector3D(1.0f, 1.0f, 1.0f) + deltaScale;

						Mat4 scaleMat = Mat4::scale(scaleMultiplier);
						Mat4 rotMat = Mat4::rotateX(deltaRotationRad.x) * Mat4::rotateY(deltaRotationRad.y) * Mat4::rotateZ(deltaRotationRad.z);
						Mat4 translateMat = Mat4::translate(deltaTranslate);

						Mat4 combinedDelta = scaleMat * rotMat * translateMat;

						auto& meshRenderer = entity.GetComponent<MeshRendererComponent>();
						meshRenderer.Mesh->TransformFace(selectedFaceID, combinedDelta);
					}

					ImGui::TreePop();
				}
			}
		}
	}
}