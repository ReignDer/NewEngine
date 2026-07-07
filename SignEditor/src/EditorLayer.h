#pragma once
#include <Sign.h>


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

		bool OnWindowResizedEvent(WindowResizedEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);

	private:
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<FrameBuffer> m_FrameBuffer;

		Vector2D m_ViewportSize = { 0.0f,0.0f };

		std::shared_ptr<Shader> m_Shader;
		std::vector<std::shared_ptr<Entity>> m_Meshes;
		std::vector<PrimitiveType> m_PendingMeshes;
		int m_InitialEntityCount = 0;
		EditorHistory m_EditorHistory;

		PerspectiveCamera m_EditorCamera;
	};
}

