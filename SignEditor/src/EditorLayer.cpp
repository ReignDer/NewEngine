#include "EditorLayer.h"
#include <imgui.h>


#include "Sign/PlatformUtils/WindowsPlatformUtils.h"
#include "Sign/Asset/TextureImporter.h"
#include "Sign/Asset/MeshImporter.h"
namespace Sign {
	static bool p_open = false;
	static bool p_Credits = false;
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
		frameSpecs.m_ColorFormats = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32_SINT };
		m_FrameBuffer = std::make_shared<FrameBuffer>(frameSpecs, Renderer::GetContext()->GetDevice().Get());

		Renderer::RegisterFrameBuffers("MainEditorBuffer", m_FrameBuffer);

		m_Texture2D = TextureImporter::LoadTexture2D("SignEditor/assets/dlsu-logo.png");
		m_TeapotTexture = TextureImporter::LoadTexture2D("SignEditor/assets/brick.png");
		m_TeapotMesh = MeshImporter::LoadMesh("SignEditor/assets/teapot.obj");
		/*************** ECS VERSION ********************/
		/*m_ActiveScene = std::make_shared<Scene>();

		auto CubeECS = m_ActiveScene->CreateEntity("Cube1");
		CubeECS.AddComponent<MeshRendererComponent>(Primitive::Cube3D::Create(), m_ShaderLibrary.GetDefault());
		auto& CubeTransform = CubeECS.GetComponent<TransformComponent>();
		CubeTransform.Translation = { 0.0f,5.0f,3.0f };

		CubeECS.AddComponent<RigidBody3D>();
		auto& CubeRb = CubeECS.GetComponent<RigidBody3D>();
		CubeRb.Type = RigidBody3D::BodyType::Dynamic;

		CubeECS.AddComponent<Box3DColliderComponent>();

		auto CubeECS2 = m_ActiveScene->CreateEntity("Cube2");
		CubeECS2.AddComponent<MeshRendererComponent>(Primitive::Cube3D::Create(), m_ShaderLibrary.GetDefault());
		auto& CubeTransform2 = CubeECS2.GetComponent<TransformComponent>();
		CubeTransform2.Translation = { -5.0f,0.0f,5.0f };

		auto Plane = m_ActiveScene->CreateEntity("Plane");
		Plane.AddComponent<MeshRendererComponent>(Primitive::Plane::Create(), m_ShaderLibrary.GetDefault());
		auto& PlaneTransform = Plane.GetComponent<TransformComponent>();
		PlaneTransform.Scale = { 10.0f,0.005f,10.0f };
		PlaneTransform.Translation = { 0.0f,-0.5f,0.0f };

		Plane.AddComponent<RigidBody3D>();
		auto& PlaneRb = Plane.GetComponent<RigidBody3D>();
		PlaneRb.Type = RigidBody3D::BodyType::Static;

		Plane.AddComponent<Box3DColliderComponent>();
		auto& PlaneCol = Plane.GetComponent<Box3DColliderComponent>();
		PlaneCol.Size = { 0.5f, 0.005f, 0.5f };

		PipelineSpecifications specs = {};
		specs.InputLayout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "FACEID", 0, DXGI_FORMAT_R32_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} };
		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		specs.RTVFormats = rtvFormats;
		specs.DepthTest = TRUE;
		specs.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		

		auto teapot = m_ActiveScene->CreateEntity("Teapot");
		teapot.AddComponent<MeshRendererComponent>(MeshImporter::LoadMesh("SignEditor/assets/teapot.obj"), m_ShaderLibrary.Load("MeshShader", L"Shader/VertexMeshShader.hlsl", L"Shader/PixelTextureShader.hlsl", specs), m_TeapotTexture);
		auto& teapotTransform = teapot.GetComponent<TransformComponent>();
		teapotTransform.Translation = { 0.0f,0.0f,5.0f };

		auto bunny = m_ActiveScene->CreateEntity("Bunny");
		bunny.AddComponent<MeshRendererComponent>(MeshImporter::LoadMesh("SignEditor/assets/bunny.obj"), m_ShaderLibrary.Get("MeshShader"));
		auto& bunnyTransform = bunny.GetComponent<TransformComponent>();
		bunnyTransform.Translation = { 3.0f,0.0f,5.0f };

		auto armadillo = m_ActiveScene->CreateEntity("Armadillo");
		armadillo.AddComponent<MeshRendererComponent>(MeshImporter::LoadMesh("SignEditor/assets/armadillo.obj"), m_ShaderLibrary.Get("MeshShader"));
		auto& armadilloTransform = armadillo.GetComponent<TransformComponent>();
		armadilloTransform.Translation = { -3.0f,0.0f,5.0f };*/
		//int index = 0;
		//for (int i = 0; i < 10000; i++) {
		//	
		//	auto CubeECS = m_ActiveScene->CreateEntity("Cube" + std::to_string(index));
		//	CubeECS.AddComponent<MeshRendererComponent>(Primitive::Cube3D::Create());
		//	auto& CubeTransform = CubeECS.GetComponent<TransformComponent>();
		//	CubeTransform.Translation = { MathUtils::Random_Float(-100.f,100.f),MathUtils::Random_Float(-100.f,100.f),MathUtils::Random_Float(-100.f,100.f) };
		//	index++;
		//}
		OpenProject("SignEditor/SignProject/SignBox.sproj");
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		
		/***********************************************/

		/*************** OOP VERSION ********************/
		auto Cube = std::make_shared<CubeEntity>();
		Cube->SetTranslation({ 0.0f,0.0f,5.0f });
		auto plane = std::make_shared<PlaneEntity>();
		plane->SetTranslation({ 0.0f,-0.5f,0.0f });

		m_Meshes.push_back(Cube);
		m_Meshes.push_back(plane);
		/***********************************************/



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

		/*for (int i = 0; i < 5000; i++) {
			auto Cube = std::make_shared<CubeEntity>();
			Cube->SetTranslation({ MathUtils::Random_Float(-15.0f,15.0f),MathUtils::Random_Float(-15.0f,15.0f) ,MathUtils::Random_Float(-15.0f,15.0f) });
			Cube->SetScale(Vector3D(0.5f, 0.5f, 0.5f));
			m_Meshes.push_back(Cube);
			m_InitialEntityCount++;
		}*/

		std::println("Entity Numbers: {}", m_ActiveScene->GetRegistry().GetPool<TagComponent>().Size());
	}

	void EditorLayer::OnDettach()
	{
		m_Meshes.clear();
		m_PendingMeshes.clear();
		m_PendingMeshes.clear();
		m_Meshes.shrink_to_fit();
		m_VertexArray.reset();

		m_ActiveScene.reset();
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


		
		if (m_ViewportFocused)
			m_EditorCamera.OnUpdate(dt);
		else
			m_EditorCamera.ResetDragState();

		if(StartSimulation)
			m_ActiveScene->OnUpdateRuntime(dt);

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
		dispatch.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& event) {return OnMouseButtonPressedEvent(event); });

	}

	void EditorLayer::OnRender()
	{
		FLOAT clearColor[] = { 0.15f, 0.14f, 0.13f, 1.0f };

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

		

		/*******ECS********/
		m_ActiveScene->RenderScene(m_SelectedEntity ? m_SelectedEntity : INVALID_ENTITY_ID, m_SelectedFaceID);
		/*****************/



		/*for (auto& mesh : m_Meshes) {
			if (mesh->HasMesh())
				Renderer::Submit(mesh->GetMesh()->GetVertexArray(), *mesh->GetShader(), mesh->GetTransform());
		}*/

		if (m_PickRequest) {
			PixelData pixelData = m_FrameBuffer->ReadPixel(1,(int)m_PickCoords.x, (int)m_PickCoords.y);

			if (pixelData.entityID == -1) {
				std::println("Pixel Data: {}", pixelData.entityID);
				m_SelectedEntity = EntityECS();
				m_SelectedFaceID = -1;
			}
			else {
				EntityID id = (uint32_t)pixelData.entityID;
				m_SelectedEntity = EntityECS(id, m_ActiveScene.get());
				m_SelectedFaceID = pixelData.faceID;
				std::println("Entity: {}", m_SelectedEntity.GetName());
				std::println("FaceID: {}", m_SelectedFaceID);
			}
			m_PickRequest = false;
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

		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.f;

		ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_None);

		style.WindowMinSize.x = minWinSizeX;

		// Refocus our window to minimize perceived loss of focus when changing mode (caused by the fact that each use a different window, which would not happen in a real app)
		if (opt_demo_mode_changed)
			ImGui::SetNextWindowFocus();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N")) {
					NewScene();
					
				}
				if (ImGui::MenuItem("Open...", "Ctrl+O")) {
					OpenScene();
					
				}

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
					SaveSceneAs();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools")) {
				ImGui::MenuItem("Color Picker", NULL, &p_open);
			
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("About")) {
				ImGui::MenuItem("Credits", NULL, &p_Credits);

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (p_open) {
			ColorPicker();
		}
		if (p_Credits) {
			ImGui::SetNextWindowSize(ImVec2(800, 900), ImGuiCond_FirstUseEver);
			if (!ImGui::Begin("Credits", &p_Credits))
			{
				ImGui::End();
				return;
			}
			float contentWidth = ImGui::GetContentRegionAvail().x;

			float texWidth = (float)m_Texture2D->GetWidth();
			float texHeight = (float)m_Texture2D->GetHeight();
			float aspectHeight = (texHeight / texWidth) * contentWidth;
			ImVec2 displaySize(contentWidth, aspectHeight);
			ImGui::Image((ImTextureID)m_Texture2D->GetGpuHandle().ptr, displaySize);
			ImGui::Text("About");
			ImGui::Text("DX12 Engine by Mathieu Marc I. Pobre");
			ImGui::NewLine();
			ImGui::Text("Acknoledgements:");
			ImGui::Text("The Cherno Hazel  Game Engine Tutorial");
			ImGui::Text("Dr. Neil De Gallego and Sir Martin Laureta's GDENG03 course");
			ImGui::End();
		}
		

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

		m_SceneHierarchyPanel.OnImGuiRender();
		m_ContentBrowserPanel->OnImGuiRender();
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

		ImGuiWindowClass window_class;
		window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;

		ImGui::SetNextWindowClass(&window_class);
		ImGui::Begin("Viewport", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();

		m_ViewportHovered = ImGui::IsWindowHovered();
		m_ViewportFocused = ImGui::IsWindowFocused();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);
		
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x,viewportPanelSize.y };
		UINT64 coloraAttachment = m_FrameBuffer->GetTextureID();
		ImGui::Image((ImTextureID)coloraAttachment, ImVec2(m_ViewportSize.x, m_ViewportSize.y));



		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
				const wchar_t* path = (const wchar_t*)payload->Data;

				OpenScene(path);
			}

			ImGui::EndDragDropTarget();
		}

		ImVec2 ImageMin = ImGui::GetItemRectMin();
		ImVec2 ImageMax = ImGui::GetItemRectMax();
		m_ViewportBounds[0] = { ImageMin.x, ImageMin.y };
		m_ViewportBounds[1] = { ImageMax.x, ImageMax.y };
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::ColorPicker()
	{
		static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 255.0f / 255.0f);
		static ImGuiColorEditFlags base_flags = ImGuiColorEditFlags_None;
		ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Color Picker", &p_open))
		{
			ImGui::End();
			return;
		}
		ImGui::ColorPicker3("##MyColor##6", (float*)&color, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
		ImGui::SameLine();
		ImGui::Text("Color");
		ImGui::SameLine();
		ImVec2 size = ImVec2(100.f, 100.f);
		ImGui::ColorButton("##MyColor##6", color, ImGuiColorEditFlags_NoAlpha,size);
		ImGui::End();
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
		bool shift = Input::IsKeyPressed(Key::LShift) || Input::IsKeyPressed(Key::RShift);

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
		case Key::N:
		{
			if (control)
			{
				NewScene();
			}
			break;
		}
		case Key::O:
		{
			if (control)
			{
				OpenScene();
			}
			break;
		}
		case Key::S:
		{

			if (control) {
				if (shift) {
					SaveSceneAs();
				}
				if (!StartSimulation) {
					std::println("Sim Start");
					m_ActiveScene->OnPhysics3DStart();
					StartSimulation = true;

				}
				else {
					std::println("Sim Stop");
					m_ActiveScene->OnPhysics3DStop();
					StartSimulation = false;
				}
			}break;
		}
		}
		return false;
	}
	bool EditorLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& e)
	{
		switch (e.GetMouseButton()) 
		{
		case Mouse::RightButton:
		{
			auto [mx, my] = ImGui::GetMousePos();
			mx -= m_ViewportBounds[0].x;
			my -= m_ViewportBounds[0].y;

			Vector2D viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
			
			int mouseX = (int)mx;
			int mouseY = (int)my;

			
			if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y) {
				std::println("mx: {}, my: {}, viewportSize: {}x{}, framebuffer: {}x{}",
					mx, my, viewportSize.x, viewportSize.y,
					m_FrameBuffer->GetSpecifications().m_Width, m_FrameBuffer->GetSpecifications().m_Height);
				m_PickCoords.x = mouseX;
				m_PickCoords.y = mouseY;

				m_PickRequest = true;
				
			}
			break;
		}
		}

		return false;
	}
	void EditorLayer::NewProject()
	{
		Project::New();
	}
	bool EditorLayer::OpenProject()
	{
		std::string filepath = FileDialogs::OpenFile("Hazel Project(*.sproj)\0*.sproj\0");
		if (filepath.empty())
			return false;

		OpenProject(filepath);
		return true;
	}
	void EditorLayer::OpenProject(const std::filesystem::path& path)
	{
		std::println("{}", std::filesystem::absolute(path).string());
		if (Project::Load(path)) {
			auto startScenePath = Project::GetAssetFileSystemPath(Project::GetActive()->GetConfig().StartScene);
			OpenScene(startScenePath);
			m_ContentBrowserPanel = std::make_unique<ContentBrowserPanel>();
		}
	}
	void EditorLayer::SaveProject()
	{
		//Project::SaveActive();
	}
	void EditorLayer::NewScene()
	{
		Renderer::GetContext()->FlushCommandQueue();
		m_ActiveScene = std::make_shared<Scene>();
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}
	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Sign Scene (*.sign) \0*.sign\0");
		if (!filepath.empty()) {
			OpenScene(filepath);
		}
	}
	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		Renderer::GetContext()->FlushCommandQueue();
		m_ActiveScene = std::make_shared<Scene>();

		//Resize Viewport if we have a scene camera here
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		SceneSerializer serializer(m_ActiveScene);
		serializer.Deserialize(path.string());
	}
	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Sign Scene (*.sign) \0*.sign\0");
		if (!filepath.empty()) {

			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(filepath);
		}
	}
}