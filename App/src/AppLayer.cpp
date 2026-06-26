#include "AppLayer.h"

AppLayer::AppLayer()
{
	std::println("App Layer Created");
}

void AppLayer::OnAttach()
{
	m_EditorCamera = Sign::PerspectiveCamera(Sign::Application::Get().GetWindow().GetWidth(), Sign::Application::Get().GetWindow().GetHeight());
	m_EditorCamera.SetPerspective(MathUtils::ConvertToRadians(45.0f), 0.1f, 1000.0f);

	
	/*Sign::PipelineSpecifications pSpecs = {};
	pSpecs.Shader = m_Shader;
	pSpecs.InputLayout = { { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, };
	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	rtvFormats.NumRenderTargets = 1;
	rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pSpecs.RTVFormats = rtvFormats;
	pSpecs.DepthTest = TRUE;
	pSpecs.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	m_Shader = std::make_shared<Sign::Shader>(L"Shader/VertexShader.hlsl", L"Shader/PixelShader.hlsl", pSpecs);*/

	/*auto triangle = std::make_shared<Sign::TriangleEntity>();

	m_Meshes.push_back(triangle);
	m_InitialEntityCount++;*/
	/*std::array<Sign::Vector3D, 8> col;
	for (int i = 0; i < col.size(); i++) {
		col[i] = Sign::Vector3D(1.0f, 1.0f, 1.0f);
	}*/
	/*auto Cube = std::make_shared<Sign::CubeEntity>();
	auto Cube2 = std::make_shared<Sign::CubeEntity>();
	auto Cube3 = std::make_shared<Sign::CubeEntity>();

	Cube->SetTranslation({ 0.0f, 0.9f, 0.0f });
	Cube2->SetTranslation({ -1.5f, 2.0f, 0.0f });
	Cube3->SetTranslation({ -1.5f, 3.0f, -2.0f });

	m_Meshes.push_back(Cube);
	m_Meshes.push_back(Cube2);
	m_Meshes.push_back(Cube3);*/
	/*Cube->SetScale(Sign::Vector3D(0.5f, 0.5f, 0.5f));
	auto Cube2 = std::make_shared<Sign::CubeEntity>();
	Cube2->SetTranslation({ 0.0f,0.0f,10.0f });
	Cube2->SetScale(Sign::Vector3D(0.5f, 0.5f, 0.5f));*/


	for (int i = 0; i < 15; i++) {
		bool odd = (i % 2) != 0;
		auto plane = std::make_shared<Sign::PlaneEntity>();

		switch (i) {
			case 6:
				m_Meshes.push_back(plane);
				continue;
			case 7:
				m_Meshes.push_back(plane);
				continue;
			case 11:
				m_Meshes.push_back(plane);
				continue;
			case 13:
				m_Meshes.push_back(plane);
				continue;
		}

		if (odd) {
			plane->SetRotation(Sign::Quaternion(0.0f, 0.0f, MathUtils::ConvertToRadians(-70.0f), 0.0f));
			//plane->SetTranslation({ 5.0f *std::cos(MathUtils::ConvertToRadians(70.0f)) * 2 ,0,0 });
		}
		else{
			plane->SetRotation(Sign::Quaternion(0.0f, 0.0f, MathUtils::ConvertToRadians(70.0f), 0.0f));
		}
		m_Meshes.push_back(plane);
	}
	float currentX = 5.0f * std::cos(MathUtils::ConvertToRadians(70.0f)) * 2;
	m_Meshes[1]->SetTranslation({ currentX, 0.0f,0.0f });
	currentX += currentX;
	m_Meshes[2]->SetTranslation({ currentX, 0.0f,0.0f });
	currentX += currentX;
	m_Meshes[3]->SetTranslation({ currentX, 0.0f,0.0f });
	currentX += currentX;
	m_Meshes[4]->SetTranslation({ currentX, 0.0f,0.0f });

	/*currentX += currentX;
	m_Meshes[5]->SetTranslation({ currentX, 0.0f,0.0f });
	currentX += currentX;
	m_Meshes[6]->SetTranslation({ currentX, 0.0f,0.0f });*/
	/*int pairTiers[] = { 3,2,1 };
	for (int tier = 0; tier < 3; tier++) {
		int pairInTier = pairTiers[tier];
		int cardsInTier = pairInTier * 2;

		float startTierX = tier;


		for (int localCard = 0; localCard < cardsInTier; localCard++) {
			float currentX = startTierX;

			m_Meshes[localCard]->SetTranslation({currentX,0,0});
			startTierX += 5.0f * std::cos(MathUtils::ConvertToRadians(70.0f)) * 2;
			
		}
	}*/


/*	auto circle = std::make_shared<Sign::CircleEntity>();
	circle->SetTranslation({ 0.0f,0.0f,5.0f });

	auto sphere = std::make_shared<Sign::SphereEntity>();
	sphere->SetTranslation({ 5.0f,0.0f,5.0f });*/

	/*m_Meshes.push_back(sphere);
	m_InitialEntityCount++;

	m_Meshes.push_back(circle);
	m_InitialEntityCount++;*/
	//m_Meshes.push_back(Cube);
	//m_Meshes.push_back(Cube2);
/*	m_Meshes.push_back(plane);
	m_InitialEntityCount++;*/

	/*for (int i = 0; i < 50; i++) {
		auto Cube = std::make_shared<Sign::CubeEntity>();
		Cube->SetTranslation({ MathUtils::Random_Float(-15.0f,15.0f),MathUtils::Random_Float(-15.0f,15.0f) ,MathUtils::Random_Float(-15.0f,15.0f) });
		Cube->SetScale(Sign::Vector3D(0.5f, 0.5f, 0.5f));
		m_Meshes.push_back(Cube);
		m_InitialEntityCount++;
	}*/

	std::println("Entity Numbers: {}", m_Meshes.size());
}

void AppLayer::OnUpdate(Sign::Timestep dt)
{
	//std::println("Delta Time: {} {}", dt.GetSeconds(), dt.GetMilliseconds());

	if (Sign::Input::IsKeyPressed(Sign::Key::A)) {
		std::println("A");
	}

	if (Sign::Input::IsKeyPressed(Sign::Key::Esc)) {
		Sign::Application::Get().Stop();
	}
	
	m_EditorCamera.OnUpdate(dt);

	for (auto& entity : m_Meshes) {
		entity->OnUpdate(dt);
	}
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
	dispatch.Dispatch<Sign::KeyPressedEvent>([this](Sign::KeyPressedEvent& event) {return OnKeyPressedEvent(event); });

}

void AppLayer::OnRender()
{
	FLOAT clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	Sign::Renderer::BeginScene(clearColor, m_EditorCamera);

	//Need for open commandlist, might have to make a separate func for this
	for (auto& pending : m_PendingMeshes) {
		Sign::CreateObjectCommand* command = new Sign::CreateObjectCommand(m_Meshes, Sign::PrimitiveType::Cube);
		command->Execute();
		m_EditorHistory.Record(command);
	}
	m_PendingMeshes.clear();

	//Sign::Renderer::Submit(m_VertexArray, *m_Shader, Sign::Mat4::identity());

	for (auto& mesh : m_Meshes) {
		if(mesh->HasMesh())
			Sign::Renderer::Submit(mesh->GetMesh()->GetVertexArray(), *mesh->GetShader(), mesh->GetTransform());
	}

	Sign::Renderer::EndScene();
}

bool AppLayer::OnWindowResizedEvent(Sign::WindowResizedEvent& e)
{
	m_EditorCamera.SetViewPortSize(e.GetWidth(), e.GetHeight());
	return false;
}

bool AppLayer::OnKeyPressedEvent(Sign::KeyPressedEvent& e)
{
	if (e.IsRepeated())
		return false;

	switch (e.GetKeyCode()) 
	{
		case Sign::Key::Esc: 
		{
			Sign::Application::Get().Stop();
			break;
		}
		case Sign::Key::Spacebar: 
		{
			m_PendingMeshes.push_back(Sign::PrimitiveType::Cube);
			std::println("Entitiy Number: {}", m_Meshes.size());
			break;
		}
		case Sign::Key::Backspace:
		{
			Sign::RemoveObjectCommand* command = new Sign::RemoveObjectCommand(m_Meshes);
			command->Execute();
			std::println("Entitiy Number: {}", m_Meshes.size());
			m_EditorHistory.Record(command);
			break;
		}
		case Sign::Key::Delete:
		{
			Sign::DeleteAllCommand* command = new Sign::DeleteAllCommand(m_Meshes, m_InitialEntityCount);
			command->Execute();
			std::println("Entitiy Number: {}", m_Meshes.size());
			m_EditorHistory.Record(command);
		}
	}

	bool control = Sign::Input::IsKeyPressed(Sign::Key::LControl) || Sign::Input::IsKeyPressed(Sign::Key::RControl);

	switch (e.GetKeyCode()) 
	{
		case Sign::Key::Z: 
		{
			if (control) {
				m_EditorHistory.Undo();
				std::println("Undo");
			}
			break;
		}
	}
	return false;
}
