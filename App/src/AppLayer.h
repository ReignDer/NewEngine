#pragma once
#include "Sign/Application.h"
#include "Sign/Layer.h"
#include <print>
#include <vector>
#include "Sign/Input.h"
#include "Sign/Events/Event.h"
#include "Sign/Events/KeyEvents.h"
#include "Sign/Events/MouseEvents.h"
#include "Sign/Events/WindowEvents.h"
#include "Sign/Renderer/Renderer.h"
#include "Sign/Shader/Shader.h"
#include "Sign/Buffers/VertexBuffer.h"
#include "Sign/Buffers/IndexBuffer.h"
#include "Sign/Buffers/VertexArray.h"
#include "Sign/Renderer/MeshUtils.h"
#include "Sign/Renderer/Pipeline.h"
#include "Sign/Renderer/PerspectiveCamera.h"
#include "Sign/Renderer/Primitive3D.h"

class AppLayer : public Sign::Layer
{
public:
	AppLayer();
	virtual void OnAttach() override;
	virtual void OnUpdate(float ts) override;
	virtual void OnEvent(Sign::Event& event) override;
	virtual void OnRender() override;
	bool OnWindowResizedEvent(Sign::WindowResizedEvent& e);

private:
	std::shared_ptr<Sign::VertexArray> m_VertexArray;
	std::unique_ptr<Sign::GraphicsPipeline> m_GraphicsPipeline;
	std::shared_ptr<Sign::Shader> m_Shader;
	std::vector<std::shared_ptr<Sign::Mesh>> m_Meshes;

	Sign::PerspectiveCamera m_EditorCamera;
};

