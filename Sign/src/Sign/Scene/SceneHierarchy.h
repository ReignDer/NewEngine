#pragma once
#include "Sign/Scene/Scene.h"
#include "Sign/Scene/EntityECS.h"
#include "Sign/Math/MathUtils.h"
namespace Sign {
	class SceneHierarchy
	{
	public:
		SceneHierarchy() = default;
		SceneHierarchy(const std::shared_ptr<Scene>& scene);
		~SceneHierarchy();

		void SetContext(const std::shared_ptr<Scene>& scene);

		void OnImGuiRender();
	private:
		void DrawTreeNode(EntityECS entity);
		void DrawComponents(EntityECS entity);

	private:
		std::shared_ptr<Scene> m_Context;
		EntityECS m_SelectedEntity;
		uint32_t m_LastEditedFaceID;
		Vector3D m_FaceEditOffset;
		Vector3D m_FaceRotationOffset;
		Vector3D m_FaceScaleOffset;
	};
}

