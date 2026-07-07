#include "EditorLayer.h"
#include <imgui.h>

namespace Sign {
	EditorLayer::EditorLayer()
	{
		std::println("Editor Layer Created");
	}

	void EditorLayer::OnAttach()
	{
		m_EditorCamera = PerspectiveCamera(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
		m_EditorCamera.SetPerspective(MathUtils::ConvertToRadians(45.0f), 0.1f, 1000.0f);

		FrameBufferSpecifications frameSpecs = {};
		frameSpecs.m_Width = Application::Get().GetWindow().GetWidth();
		frameSpecs.m_Height = Application::Get().GetWindow().GetHeight();
		m_FrameBuffer = std::make_shared<FrameBuffer>(frameSpecs, Renderer::GetContext()->GetDevice().Get());

		Renderer::RegisterFrameBuffers("MainEditorBuffer", m_FrameBuffer);


		auto Cube = std::make_shared<CubeEntity>();
		Cube->SetTranslation({ 0.0f,0.0f,5.0f });
		auto plane = std::make_shared<PlaneEntity>();
		plane->SetTranslation({ 0.0f,-0.5f,0.0f });

		m_Meshes.push_back(Cube);
		m_Meshes.push_back(plane);
		/*PipelineSpecifications pSpecs = {};
		pSpecs.Shader = m_Shader;
		pSpecs.InputLayout = { { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, };
		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pSpecs.RTVFormats = rtvFormats;
		pSpecs.DepthTest = TRUE;
		pSpecs.DSVFormat = DXGI_FORMAT_D32_FLOAT;

		m_Shader = std::make_shared<Shader>(L"Shader/VertexShader.hlsl", L"Shader/PixelShader.hlsl", pSpecs);*/

		/*auto triangle = std::make_shared<TriangleEntity>();

		m_Meshes.push_back(triangle);
		m_InitialEntityCount++;*/
		/*std::array<Vector3D, 8> col;
		for (int i = 0; i < col.size(); i++) {
			col[i] = Vector3D(1.0f, 1.0f, 1.0f);
		}*/
		/*auto Cube = std::make_shared<CubeEntity>();
		auto Cube2 = std::make_shared<CubeEntity>();
		auto Cube3 = std::make_shared<CubeEntity>();

		Cube->SetTranslation({ 0.0f, 0.9f, 0.0f });
		Cube2->SetTranslation({ -1.5f, 2.0f, 0.0f });
		Cube3->SetTranslation({ -1.5f, 3.0f, -2.0f });

		m_Meshes.push_back(Cube);
		m_Meshes.push_back(Cube2);
		m_Meshes.push_back(Cube3);*/
		/*Cube->SetScale(Vector3D(0.5f, 0.5f, 0.5f));
		auto Cube2 = std::make_shared<CubeEntity>();
		Cube2->SetTranslation({ 0.0f,0.0f,10.0f });
		Cube2->SetScale(Vector3D(0.5f, 0.5f, 0.5f));*/


/*		for (int i = 0; i < 15; i++) {
			bool odd = (i % 2) != 0;
			auto plane = std::make_shared<PlaneEntity>();
			plane->SetScale({ 1.0f,1.0f,1.0f });

			if (i == 6 || i == 7 || i == 12) {
				m_Meshes.push_back(plane);
				continue;
			}

			if (odd) {
				if (i == 13) {
					plane->SetRotation(Quaternion(0.0f, 0.0f, MathUtils::ConvertToRadians(70.0f), 0.0f));

				}
				else
					plane->SetRotation(Quaternion(0.0f, 0.0f, MathUtils::ConvertToRadians(-70.0f), 0.0f));
				//plane->SetTranslation({ 5.0f *std::cos(MathUtils::ConvertToRadians(70.0f)) * 2 ,0,0 });
			}
			else {
				if (i == 14) {
					plane->SetRotation(Quaternion(0.0f, 0.0f, MathUtils::ConvertToRadians(-70.0f), 0.0f));

				}
				else
					plane->SetRotation(Quaternion(0.0f, 0.0f, MathUtils::ConvertToRadians(70.0f), 0.0f));
			}
			m_Meshes.push_back(plane);
		}
		float cardDistance = 0.5f * std::cos(MathUtils::ConvertToRadians(70.0f)) * 2;
		float cardDistance2 = 0.5f * std::cos(MathUtils::ConvertToRadians(70.0f));
		float cardHeight = 0.5f * std::sin(MathUtils::ConvertToRadians(70.0f));
		float currentX = cardDistance;
		float currentY = cardHeight;

		m_Meshes[1]->SetTranslation({ currentX, 0.0f,0.0f });
		currentX += cardDistance;
		float middle = currentX / 2;
		m_Meshes[2]->SetTranslation({ currentX, 0.0f,0.0f });
		currentX += cardDistance;
		m_Meshes[3]->SetTranslation({ currentX, 0.0f,0.0f });
		currentX += cardDistance;
		m_Meshes[4]->SetTranslation({ currentX, 0.0f,0.0f });
		currentX += cardDistance;
		m_Meshes[5]->SetTranslation({ currentX, 0.0f,0.0f });
		m_Meshes[6]->SetTranslation({ middle,currentY,0.0f });

		m_Meshes[7]->SetTranslation({ middle * 4,currentY,0.0f });

		currentX = 0;
		currentX = cardDistance;
		currentY += cardHeight;
		m_Meshes[8]->SetTranslation({ currentX, currentY,0.0f });
		currentX += cardDistance;
		m_Meshes[9]->SetTranslation({ currentX,currentY,0.0f });
		currentX += cardDistance;
		m_Meshes[10]->SetTranslation({ currentX, currentY,0.0f });
		currentX += cardDistance;
		m_Meshes[11]->SetTranslation({ currentX,currentY,0.0f });

		currentY += cardHeight;
		m_Meshes[12]->SetTranslation({ cardDistance2 * 5, currentY,0.0f });

		currentY += cardHeight;
		currentX = 0;
		currentX = cardDistance * 2;
		m_Meshes[13]->SetTranslation({ currentX, currentY,0.0f });
		currentX += cardDistance;
		m_Meshes[14]->SetTranslation({ currentX, currentY,0.0f });*/

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

			}90
		}*/


		/*auto circle = std::make_shared<CircleEntity>();
		circle->SetTranslation({ 0.0f,0.0f,5.0f });

		auto sphere = std::make_shared<SphereEntity>();
		sphere->SetTranslation({ 5.0f,0.0f,5.0f });

		m_Meshes.push_back(sphere);
		m_InitialEntityCount++;

		m_Meshes.push_back(circle);
		m_InitialEntityCount++;*/
		//m_Meshes.push_back(Cube);
		//m_Meshes.push_back(Cube2);
	/*	m_Meshes.push_back(plane);
		m_InitialEntityCount++;*/

		/*for (int i = 0; i < 50; i++) {
			auto Cube = std::make_shared<CubeEntity>();
			Cube->SetTranslation({ MathUtils::Random_Float(-15.0f,15.0f),MathUtils::Random_Float(-15.0f,15.0f) ,MathUtils::Random_Float(-15.0f,15.0f) });
			Cube->SetScale(Vector3D(0.5f, 0.5f, 0.5f));
			m_Meshes.push_back(Cube);
			m_InitialEntityCount++;
		}*/

		std::println("Entity Numbers: {}", m_Meshes.size());
	}

	void EditorLayer::OnDettach()
	{
		m_Meshes.clear();
		m_PendingMeshes.clear();
		m_PendingMeshes.clear();
		m_Meshes.shrink_to_fit();
		m_VertexArray.reset();
		m_FrameBuffer.reset();
		m_Shader.reset();
	}

	void EditorLayer::OnUpdate(Timestep dt)
	{
		//std::println("Delta Time: {} {}", dt.GetSeconds(), dt.GetMilliseconds());

		if (Input::IsKeyPressed(Key::A)) {
			std::println("A");
		}

		if (Input::IsKeyPressed(Key::Esc)) {
			Application::Get().Stop();
		}

		m_EditorCamera.OnUpdate(dt);

		for (auto& entity : m_Meshes) {
			entity->OnUpdate(dt);
		}
		//std::println("{} {}", Input::GetMouseX(), Input::GetMouseY());
	}

	void EditorLayer::OnEvent(Event& event)
	{
		if (event.GetEventType() == EventType::KeyPressed) {
			auto& e = (KeyPressedEvent&)event;
			std::println("{}", (char)e.GetKeyCode());
		}
		EventDispatcher dispatch(event);
		dispatch.Dispatch<WindowResizedEvent>([this](WindowResizedEvent& event) {return OnWindowResizedEvent(event); });
		dispatch.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& event) {return OnKeyPressedEvent(event); });

	}

	void EditorLayer::OnRender()
	{
		FLOAT clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		Renderer::BeginFrame();
		if (FrameBufferSpecifications spec = m_FrameBuffer->GetSpecifications();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(spec.m_Width != m_ViewportSize.x || spec.m_Height != m_ViewportSize.y))
		{
			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.SetViewPortSize(m_ViewportSize.x, m_ViewportSize.y);
		}
		m_FrameBuffer->TransitionTo(Renderer::GetCommandList().Get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_FrameBuffer->Bind();

		//Renderer::RenderClearCommand(clearColor);

		m_FrameBuffer->ClearAttchment(clearColor);

		Renderer::BeginScene(m_EditorCamera);

		//Need for open commandlist, might have to make a separate func for this
		for (auto& pending : m_PendingMeshes) {
			CreateObjectCommand* command = new CreateObjectCommand(m_Meshes, PrimitiveType::Cube);
			command->Execute();
			m_EditorHistory.Record(command);
		}
		m_PendingMeshes.clear();

		//Renderer::Submit(m_VertexArray, *m_Shader, Mat4::identity());

		for (auto& mesh : m_Meshes) {
			if (mesh->HasMesh())
				Renderer::Submit(mesh->GetMesh()->GetVertexArray(), *mesh->GetShader(), mesh->GetTransform());
		}

		Renderer::EndScene();
	}

	void EditorLayer::OnImGuiRender()
	{
		static int opt_demo_mode = 0;
		static bool opt_demo_mode_changed = false;
		static bool dockSpaceOpen = true;
		bool IsFullscreen = true;
		bool KeepWindowPadding = true;

		ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_None);

		// Refocus our window to minimize perceived loss of focus when changing mode (caused by the fact that each use a different window, which would not happen in a real app)
		if (opt_demo_mode_changed)
			ImGui::SetNextWindowFocus();
		ImGui::Begin("Examples: Dockspace", &dockSpaceOpen, ImGuiWindowFlags_MenuBar);

		opt_demo_mode_changed = false;
		opt_demo_mode_changed |= ImGui::RadioButton("Basic demo mode", &opt_demo_mode, 0);
		opt_demo_mode_changed |= ImGui::RadioButton("Advanced demo mode", &opt_demo_mode, 1);

		ImGui::SeparatorText("Options");

		if (opt_demo_mode == 0)
		{
			ImGuiDockNodeFlags args = ImGuiDockNodeFlags_None;
			args &= ImGuiDockNodeFlags_PassthruCentralNode; // Allowed flags
			ImGui::CheckboxFlags("Flag: PassthruCentralNode", &args, ImGuiDockNodeFlags_PassthruCentralNode);
		}
		else if (opt_demo_mode == 1)
		{
			ImGuiDockNodeFlags args = ImGuiDockNodeFlags_None;
			ImGui::Checkbox("Fullscreen", &IsFullscreen);
			ImGui::Checkbox("Keep Window Padding", &KeepWindowPadding);
			ImGui::SameLine();
			//HelpMarker("This is mostly exposed to facilitate understanding that a DockSpace() is _inside_ a window.");
			ImGui::BeginDisabled(IsFullscreen == false);
			ImGui::CheckboxFlags("Flag: PassthruCentralNode", &args, ImGuiDockNodeFlags_PassthruCentralNode);
			ImGui::EndDisabled();
			ImGui::CheckboxFlags("Flag: NoDockingOverCentralNode", &args, ImGuiDockNodeFlags_NoDockingOverCentralNode);
			ImGui::CheckboxFlags("Flag: NoDockingSplit", &args, ImGuiDockNodeFlags_NoDockingSplit);
			ImGui::CheckboxFlags("Flag: NoUndocking", &args, ImGuiDockNodeFlags_NoUndocking);
			ImGui::CheckboxFlags("Flag: NoResize", &args, ImGuiDockNodeFlags_NoResize);
			ImGui::CheckboxFlags("Flag: AutoHideTabBar", &args, ImGuiDockNodeFlags_AutoHideTabBar);
		}

		// Show demo options and help
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Help"))
			{
				ImGui::TextUnformatted(
					"This demonstrates the use of ImGui::DockSpace() which allows you to manually\ncreate a docking node _within_ another window." "\n"
					"The \"Basic\" version uses the ImGui::DockSpaceOverViewport() helper. Most applications can probably use this.");
				ImGui::Separator();
				ImGui::TextUnformatted("When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!" "\n"
					"- Drag from window title bar or their tab to dock/undock." "\n"
					"- Drag from window menu button (upper-left button) to undock an entire node (all windows)." "\n"
					"- Hold SHIFT to disable docking (if io.ConfigDockingWithShift == false, default)" "\n"
					"- Hold SHIFT to enable docking (if io.ConfigDockingWithShift == true)");
				ImGui::Separator();
				ImGui::TextUnformatted("More details:"); ImGui::Bullet(); ImGui::SameLine(); ImGui::TextLinkOpenURL("Docking Wiki page", "https://github.com/ocornut/imgui/wiki/Docking");
				ImGui::BulletText("Read comments in ShowExampleAppDockSpace()");
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x,viewportPanelSize.y };
		UINT64 coloraAttachment = m_FrameBuffer->GetTextureID();
		ImGui::Image((ImTextureID)coloraAttachment, ImVec2(m_ViewportSize.x, m_ViewportSize.y));
		ImGui::End();
		ImGui::PopStyleVar();
	}

	bool EditorLayer::OnWindowResizedEvent(WindowResizedEvent& e)
	{
		return false;
	}

	bool EditorLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		if (e.IsRepeated())
			return false;

		switch (e.GetKeyCode())
		{
		case Key::Esc:
		{
			Application::Get().Stop();
			break;
		}
		case Key::Spacebar:
		{
			m_PendingMeshes.push_back(PrimitiveType::Cube);
			std::println("Entitiy Number: {}", m_Meshes.size());
			break;
		}
		case Key::Backspace:
		{
			RemoveObjectCommand* command = new RemoveObjectCommand(m_Meshes);
			command->Execute();
			std::println("Entitiy Number: {}", m_Meshes.size());
			m_EditorHistory.Record(command);
			break;
		}
		case Key::Delete:
		{
			DeleteAllCommand* command = new DeleteAllCommand(m_Meshes, m_InitialEntityCount);
			command->Execute();
			std::println("Entitiy Number: {}", m_Meshes.size());
			m_EditorHistory.Record(command);
		}
		}

		bool control = Input::IsKeyPressed(Key::LControl) || Input::IsKeyPressed(Key::RControl);

		switch (e.GetKeyCode())
		{
		case Key::Z:
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
}