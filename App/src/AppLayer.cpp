#include "AppLayer.h"

AppLayer::AppLayer()
{
	std::println("App Layer Created");
}

void AppLayer::OnAttach()
{
	m_EditorCamera = Sign::PerspectiveCamera(Sign::Application::Get().GetWindow().GetWidth(), Sign::Application::Get().GetWindow().GetHeight());
	m_EditorCamera.SetPerspective(DirectX::XMConvertToRadians(45.0f), 0.1f, 1000.0f);

	m_Shader = std::make_shared<Sign::Shader>(L"Shader/VertexShader.hlsl", L"Shader/PixelShader.hlsl");
	Sign::PipelineSpecifications pSpecs = {};
	pSpecs.Shader = m_Shader;
	pSpecs.InputLayout = { { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, };
	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	rtvFormats.NumRenderTargets = 1;
	rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pSpecs.RTVFormats = rtvFormats;
	pSpecs.DepthTest = TRUE;
	pSpecs.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	m_GraphicsPipeline = std::make_unique<Sign::GraphicsPipeline>(Sign::Renderer::s_Context->GetDevice(), pSpecs);

	VertexPosColor triangleVertices[3] =
	{
		{DirectX::XMFLOAT3(0.0,0.5,0.0), DirectX::XMFLOAT3(1.0,0.0,0.0)},
		{DirectX::XMFLOAT3(0.5,-0.5,0.0), DirectX::XMFLOAT3(0.0,1.0,0.0)},
		{DirectX::XMFLOAT3(-0.5,-0.5,0.0), DirectX::XMFLOAT3(0.0,0.0,1.0)}
	};

	m_VertexArray = std::make_shared<Sign::VertexArray>();

	std::shared_ptr<Sign::VertexBuffer> triangleVB;
	std::shared_ptr<Sign::IndexBuffer> triangleIB;

	triangleVB = std::make_shared<Sign::VertexBuffer>(triangleVertices, _countof(triangleVertices));
	m_VertexArray->AddVertexBuffers(triangleVB);
	triangleIB = std::make_shared<Sign::IndexBuffer>(trianlgeIndices, _countof(trianlgeIndices));
	m_VertexArray->SetIndexBuffer(triangleIB);


	auto Cube = Sign::Primitive::Cube3D::Create({ 0.0f,0.0f,1.f }, { 0.5f,0.5f,0.5f }, { 0.0f, 0.0f, 0.0f,0.0f }, {
		{{ 0.0f, 1.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f }
		} });
	auto Cube2 = Sign::Primitive::Cube3D::Create();
	Cube2->SetTranslation({ 0.0f,0.0f,10.0f });

	auto plane = Sign::Primitive::Plane::Create();
	m_Meshes.push_back(Cube);
	m_Meshes.push_back(Cube2);
	m_Meshes.push_back(plane);


	Sign::Renderer::CPUSyncToGPU();
	Sign::Renderer::ResizeDepthbuffer(Sign::Application::Get().GetWindow().GetWidth(), Sign::Application::Get().GetWindow().GetHeight());
}

void AppLayer::OnUpdate(float ts)
{
	if (Sign::Input::IsKeyPressed(Sign::Key::A)) {
		std::println("A");
	}
	
	m_EditorCamera.OnUpdate(ts);
	//std::println("{} {}", Sign::Input::GetMouseX(), Sign::Input::GetMouseY());
}

void AppLayer::OnEvent(Sign::Event& event)
{
	if (event.GetEventType() == Sign::EventType::KeyPressed) {
		auto& e = (Sign::KeyPressedEvent&)event;
		std::println("{}", (char)e.GetKeyCode());
	}
	Sign::EventDispatcher dispatch(event);
	dispatch.Dispatch<Sign::WindowResizedEvent>([this](Sign::WindowResizedEvent& event) {return OnWindowResizedEvent(event); });

}

void AppLayer::OnRender()
{
	FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
	Sign::Renderer::BeginScene(clearColor, m_EditorCamera);
	Sign::Renderer::Submit(m_VertexArray, *m_GraphicsPipeline, DirectX::XMMatrixIdentity());
	for (auto& mesh : m_Meshes) {
		Sign::Renderer::Submit(mesh->GetVertexArray(), *m_GraphicsPipeline, mesh->GetTransform());
	}
	Sign::Renderer::EndScene();
}

bool AppLayer::OnWindowResizedEvent(Sign::WindowResizedEvent& e)
{
	m_EditorCamera.SetViewPortSize(e.GetWidth(), e.GetHeight());
	return false;
}
