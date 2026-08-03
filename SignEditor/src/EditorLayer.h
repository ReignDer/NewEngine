#pragma once
#include <Sign.h>
#include "Sign/Scene/SceneHierarchy.h"
#include "Panels/ContentBrowserPanel.h"

namespace Sign {
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual void OnAttach() override;
		virtual void OnDettach() override;
		virtual void OnUpdate(Timestep dt) override;
		virtual void OnEvent(Event& event) override;
		virtual void OnRender() override;
		virtual void OnImGuiRender() override;

		void ColorPicker();

		bool OnWindowResizedEvent(WindowResizedEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
	private:
		void NewProject();
		bool OpenProject();
		void OpenProject(const std::filesystem::path& path);
		void SaveProject();

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();
	private:
		ShaderLibrary m_ShaderLibrary;
		bool StartSimulation = false;

		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<FrameBuffer> m_FrameBuffer;

		Vector2D m_ViewportSize = { 0.0f,0.0f };
		Vector2D m_ViewportBounds[2];
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		bool m_PickRequest = false;
		Vector2D m_PickCoords;

		std::shared_ptr<Scene> m_ActiveScene;
		EntityECS m_SelectedEntity;
		int32_t m_SelectedFaceID;
		SceneHierarchy m_SceneHierarchyPanel;
		std::unique_ptr<ContentBrowserPanel> m_ContentBrowserPanel;

		enum class SceneState{Edit = 0, Play = 1, Simulate = 2};

		SceneState m_SceneState = SceneState::Edit;

		std::shared_ptr<Texture2D> m_Texture2D;

		std::shared_ptr<Shader> m_Shader;
		std::vector<std::shared_ptr<Entity>> m_Meshes;
		std::vector<PrimitiveType> m_PendingMeshes;
		int m_InitialEntityCount = 0;
		EditorHistory m_EditorHistory;

		PerspectiveCamera m_EditorCamera;
	};
}

