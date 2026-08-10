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
		m_PlayButton = TextureImporter::LoadTexture2D("SignEditor/Resources/Icon/PlayButton.png");
		m_StopButton = TextureImporter::LoadTexture2D("SignEditor/Resources/Icon/StopButton.png");
		m_PauseButton = TextureImporter::LoadTexture2D("SignEditor/Resources/Icon/PauseButton.png");
		m_FrameStepButton = TextureImporter::LoadTexture2D("SignEditor/Resources/Icon/FrameStepButton.png");

		m_EditorScene = std::make_shared<Scene>();
		m_ActiveScene = m_EditorScene;
		
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


		std::println("Entity Numbers: {}", m_ActiveScene->GetRegistry().GetPool<TagComponent>().Size());
	}

	void EditorLayer::OnDettach()
	{
		m_Meshes.clear();
		m_PendingMeshes.clear();
		m_PendingMeshes.clear();
		m_Meshes.shrink_to_fit();
		m_VertexArray.reset();

		m_Texture2D.reset();
		m_PlayButton.reset();
		m_StopButton.reset();
		m_PauseButton.reset();
		m_FrameStepButton.reset();
		m_ActiveScene.reset();
		m_FrameBuffer.reset();
		m_Shader.reset();
	}

	void EditorLayer::OnUpdate(Timestep dt)
	{
		//std::println("Delta Time: {} {}", dt.GetSeconds(), dt.GetMilliseconds());

		if (Input::IsKeyPressed(Key::Esc)) {
			Application::Get().Stop();
		}

		
		if (m_ViewportFocused)
			m_EditorCamera.OnUpdate(dt);
		else
			m_EditorCamera.ResetDragState();

		switch (m_SceneState)
		{
		case SceneState::Edit:
			break;
		case SceneState::Play:
			m_ActiveScene->OnUpdateRuntime(dt);
			break;
		}
			

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

		

		/*******ECS********/
		m_ActiveScene->RenderScene(m_SelectedEntity ? m_SelectedEntity : INVALID_ENTITY_ID, m_SelectedFaceID);
		/*****************/


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
				AssetHandle handle = *(AssetHandle*)payload->Data;
				//OpenScene(path);
			}

			ImGui::EndDragDropTarget();
		}

		ImVec2 ImageMin = ImGui::GetItemRectMin();
		ImVec2 ImageMax = ImGui::GetItemRectMax();
		m_ViewportBounds[0] = { ImageMin.x, ImageMin.y };
		m_ViewportBounds[1] = { ImageMax.x, ImageMax.y };
		ImGui::End();
		ImGui::PopStyleVar();

		UI_ToolBar();
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
			if (control && shift)
			{
				SaveSceneAs();
			}
			break;
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
		std::shared_ptr<Scene> newScene = std::make_shared<Scene>();
		m_EditorScene = newScene;
		m_SceneHierarchyPanel.SetContext(m_EditorScene);
		m_ActiveScene = m_EditorScene;

	}
	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Sign Scene (*.sign, *.level) \0*.sign;*.level\0");
		if (!filepath.empty()) {
			OpenScene(filepath);
		}
	}
	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		Renderer::GetContext()->FlushCommandQueue();
		

		//Resize Viewport if we have a scene camera here
		
		std::shared_ptr<Scene> newScene = std::make_shared<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			m_EditorScene = newScene;
			m_SceneHierarchyPanel.SetContext(m_EditorScene);
			m_ActiveScene = m_EditorScene;
			
		}
	}
	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Sign Scene (*.sign, *.level) \0*.sign;*.level\0");
		if (!filepath.empty()) {

			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(filepath);
		}
	}
	void EditorLayer::UI_ToolBar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0,0,0));

		auto& colors = ImGui::GetStyle().Colors;
		auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x,buttonHovered.y,buttonHovered.z,0.5f));
		auto& buttonActive= colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z,0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
		float size = ImGui::GetWindowHeight() - 5.0f;
		std::shared_ptr<Texture2D> icon = m_SceneState == SceneState::Edit ? m_PlayButton : m_StopButton;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x) * 0.5 - (size * 0.5f));
		
		if (ImGui::ImageButton("##icon", (ImTextureID)icon->GetGpuHandle().ptr, ImVec2(size, size)))
		{
			if (m_SceneState == SceneState::Edit)
				OnScenePlay();
			else if (m_SceneState == SceneState::Play)
				OnSceneStop();



		}

		if(m_SceneState != SceneState::Edit)
		{
			bool isPaused = m_ActiveScene->IsPaused();
			ImGui::SameLine();
			std::shared_ptr<Texture2D> icon = m_PauseButton;
			
			if (ImGui::ImageButton("##PauseIcon", (ImTextureID)icon->GetGpuHandle().ptr, ImVec2(size, size)))
			{
				m_ActiveScene->SetPaused(!isPaused);

			}
			if (isPaused) {
				ImGui::SameLine();
				std::shared_ptr<Texture2D> icon = m_FrameStepButton;
				bool isPaused = m_ActiveScene->IsPaused();
				if (ImGui::ImageButton("##FrameIcon", (ImTextureID)icon->GetGpuHandle().ptr, ImVec2(size, size)))
				{
					m_ActiveScene->Step(1);

				}
			}
		}
		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}
	void EditorLayer::OnScenePlay()
	{
		Renderer::GetContext()->FlushCommandQueue();
		m_SceneState = SceneState::Play;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		
	}
	void EditorLayer::OnSceneStop()
	{
		Renderer::GetContext()->FlushCommandQueue();
		m_SceneState = SceneState::Edit;
		m_ActiveScene->OnRuntimeStop();
		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene); 
	}
	void EditorLayer::OnScenePause()
	{
		if (m_SceneState == SceneState::Edit)
			return;

		m_ActiveScene->SetPaused(true);
	}
}