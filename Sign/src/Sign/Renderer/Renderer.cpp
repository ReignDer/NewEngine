#include "Renderer.h"



namespace Sign
{


	struct RendererData {
		D3D12Context* m_Context = nullptr;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2>	m_CommandList;

		std::unordered_map<std::string, std::shared_ptr<FrameBuffer>> m_ActiveFrameBuffers;

		D3D12_VIEWPORT					m_Viewport{};
		D3D12_RECT						m_ScissorsRect{};
		std::shared_ptr<ConstantBuffer>	m_CameraConstantBuffer;
		CameraData						m_CameraData{};

		std::shared_ptr<Texture2D> m_WhiteTexture;
	};

	static RendererData* s_Data = nullptr;

	void Renderer::Init(D3D12Context* context)
	{
		s_Data = new RendererData();

		s_Data->m_Context = context;

		SetViewPort(0, 0, s_Data->m_Context->GetWidth(), s_Data->m_Context->GetHeight());
		
		s_Data->m_CameraConstantBuffer = std::make_shared<ConstantBuffer>(sizeof(CameraData), 0);

		
	}

	void Renderer::ShutDown()
	{
		s_Data->m_Context->FlushCommandQueue();
		s_Data->m_ActiveFrameBuffers.clear();
		s_Data->m_CameraConstantBuffer.reset();
		s_Data->m_CommandList.Reset();
		s_Data->m_WhiteTexture.reset();
		s_Data->m_Context = nullptr;
		delete s_Data->m_Context;
		delete s_Data;
		s_Data = nullptr;
		std::println("Renderer Shutdown");

	}

	void Renderer::BeginInitializationCommand()
	{
		s_Data->m_CommandList = s_Data->m_Context->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)->GetCommandList();

		TextureSpecifications specs = {};
		specs.Format = ImageFormat::RGBA8;
		specs.GenerateMips = false;
		specs.Width = 1;
		specs.Height = 1;

		s_Data->m_WhiteTexture = std::make_shared<Texture2D>(specs);
		uint32_t whitePixel = 0xffffffff;
		s_Data->m_WhiteTexture->SetData(&whitePixel, sizeof(whitePixel));
	}

	void Renderer::EndInitializationCommand()
	{
		CPUSyncToGPU();
		s_Data->m_CommandList = nullptr;
	}

	void Renderer::SetCommandlist(D3D12_COMMAND_LIST_TYPE type)
	{
		auto CommandQueue = s_Data->m_Context->GetCommandQueue(type);
		s_Data->m_CommandList = CommandQueue->GetCommandList();
	}

	void Renderer::BeginFrame()
	{
		s_Data->m_Context->Get_CBV_SRV_UAV_Allocator().ProcessDeferredFrees();
		auto CommandQueue = s_Data->m_Context->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
		s_Data->m_CommandList = CommandQueue->GetCommandList();

		s_Data->m_CommandList->RSSetViewports(1, &s_Data->m_Viewport);
		s_Data->m_CommandList->RSSetScissorRects(1, &s_Data->m_ScissorsRect);

		auto rtv = s_Data->m_Context->GetCurrentTargetView();
		auto dsv = s_Data->m_Context->GetDSVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
		auto backBuffer = s_Data->m_Context->GetBackBuffer(s_Data->m_Context->GetCurrentBackBuffer());
		FLOAT depth = 1.0f;
		//Clear
		{
			D3D12Utils::TransitionResource(
				s_Data->m_CommandList,
				backBuffer,
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_RENDER_TARGET);
		}
	}

	void Renderer::RenderClearCommand(FLOAT* clearColor)
	{
		
		auto rtv = s_Data->m_Context->GetCurrentTargetView();
		auto dsv = s_Data->m_Context->GetDSVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
		auto backBuffer = s_Data->m_Context->GetBackBuffer(s_Data->m_Context->GetCurrentBackBuffer());
		FLOAT depth = 1.0f;
		//Clear
		{
			D3D12Utils::TransitionResource(
				s_Data->m_CommandList,
				backBuffer,
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_RENDER_TARGET);

			s_Data->m_CommandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
			
			s_Data->m_CommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
			s_Data->m_CommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);

		}
	}

	void Renderer::BeginScene(const PerspectiveCamera& camera)
	{

		s_Data->m_CameraData.viewMatrix = Mat4::transpose(camera.GetViewMatrix());
		s_Data->m_CameraData.projectionMatrix = Mat4::transpose(camera.GetProjectionMatrix());
		s_Data->m_CameraConstantBuffer->setData(&s_Data->m_CameraData, sizeof(CameraData));
		
	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const Shader& shader, const Mat4& transform)
	{
		shader.Bind(s_Data->m_CommandList);
		ID3D12DescriptorHeap* heaps[] = { s_Data->m_Context->Get_CBV_SRV_UAV_DescriptorHeap().Get()};
		s_Data->m_CommandList->SetDescriptorHeaps(_countof(heaps), heaps);
		

		s_Data->m_CommandList->SetGraphicsRootDescriptorTable(0, s_Data->m_Context->GetGPUHandleAt(s_Data->m_Context->GetCurrentBackBuffer()));

		auto model = Mat4::transpose(transform);
		s_Data->m_CommandList->SetGraphicsRoot32BitConstants(1, sizeof(Mat4) / 4, &model, 0);

		vertexArray->Bind(s_Data->m_CommandList);


		s_Data->m_CommandList->DrawIndexedInstanced(vertexArray->GetIndexBufferCount(), 1, 0, 0, 0);
	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const Shader& shader, const Mat4& transform, const Texture2D& texture)
	{
		shader.Bind(s_Data->m_CommandList);
		ID3D12DescriptorHeap* heaps[] = { s_Data->m_Context->Get_CBV_SRV_UAV_DescriptorHeap().Get() };
		s_Data->m_CommandList->SetDescriptorHeaps(_countof(heaps), heaps);


		s_Data->m_CommandList->SetGraphicsRootDescriptorTable(0, s_Data->m_Context->GetGPUHandleAt(s_Data->m_Context->GetCurrentBackBuffer()));

		auto model = Mat4::transpose(transform);
		s_Data->m_CommandList->SetGraphicsRoot32BitConstants(1, sizeof(Mat4) / 4, &model, 0);

		texture.Bind(s_Data->m_CommandList.Get());
		vertexArray->Bind(s_Data->m_CommandList);


		s_Data->m_CommandList->DrawIndexedInstanced(vertexArray->GetIndexBufferCount(), 1, 0, 0, 0);
	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const Shader& shader, const Mat4& transform, uint32_t entity, uint32_t selectedEntityID, uint32_t selectedFace)
	{
		shader.Bind(s_Data->m_CommandList);
		ID3D12DescriptorHeap* heaps[] = { s_Data->m_Context->Get_CBV_SRV_UAV_DescriptorHeap().Get() };
		s_Data->m_CommandList->SetDescriptorHeaps(_countof(heaps), heaps);


		s_Data->m_CommandList->SetGraphicsRootDescriptorTable(0, s_Data->m_Context->GetGPUHandleAt(s_Data->m_Context->GetCurrentBackBuffer()));

		auto model = Mat4::transpose(transform);
		s_Data->m_CommandList->SetGraphicsRoot32BitConstants(1, sizeof(Mat4) / 4, &model, 0);
		s_Data->m_CommandList->SetGraphicsRoot32BitConstants(2, 1, &entity, 0);
		s_Data->m_CommandList->SetGraphicsRoot32BitConstants(3, 1, &selectedEntityID, 0);
		s_Data->m_CommandList->SetGraphicsRoot32BitConstants(4, 1, &selectedFace, 0);

		vertexArray->Bind(s_Data->m_CommandList);


		s_Data->m_CommandList->DrawIndexedInstanced(vertexArray->GetIndexBufferCount(), 1, 0, 0, 0);
	}

	void Renderer::EndScene()
	{
		auto backBuffer = s_Data->m_Context->GetBackBuffer(s_Data->m_Context->GetCurrentBackBuffer());
		{
			D3D12Utils::TransitionResource(
				s_Data->m_CommandList,
				backBuffer,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT);

			uint64_t fenceValue = s_Data->m_Context->GetCommandQueue()->ExecuteCommandList(s_Data->m_CommandList);
			s_Data->m_Context->SetFrameFenceValues(fenceValue);

		}
	}
	void Renderer::SetViewPort(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{
		s_Data->m_Viewport = CD3DX12_VIEWPORT((float)x, (float)y, (float)w, (float)h);
		s_Data->m_ScissorsRect = CD3DX12_RECT(x, y, x + w, y + h);
	}
	void Renderer::CPUSyncToGPU()
	{
		auto fenceValue = s_Data->m_Context->GetCommandQueue()->ExecuteCommandList(s_Data->m_CommandList);
		s_Data->m_Context->GetCommandQueue()->WaitForFenceValue(fenceValue);
	}
	void Renderer::Resizebuffers(int width, int height)
	{
		s_Data->m_Context->FlushCommandQueue();
		s_Data->m_Context->ResizeSwapBuffer(width, height);
		s_Data->m_Context->ResizeDepthBuffer(width, height);
	}

	void Renderer::RegisterFrameBuffers(std::string_view name, std::shared_ptr<FrameBuffer> fb)
	{
		s_Data->m_ActiveFrameBuffers[name.data()] = fb;
	}

	std::shared_ptr<FrameBuffer> Renderer::GetFrameBuffer(std::string_view name)
	{
		auto it = s_Data->m_ActiveFrameBuffers.find(name.data());

		return it != s_Data->m_ActiveFrameBuffers.end() ? it->second : nullptr;
	}
	const std::unordered_map<std::string, std::shared_ptr<FrameBuffer>> Renderer::GetAllFrameBuffers()
	{
		return s_Data->m_ActiveFrameBuffers;
	}
	D3D12Context* Renderer::GetContext()
	{
		return s_Data->m_Context;
	}
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> Renderer::GetCommandList()
	{
		return s_Data->m_CommandList;
	}
	D3D12_VIEWPORT const Renderer::GetViewPort()
	{
		return s_Data->m_Viewport;
	}
	std::shared_ptr<Texture2D> Renderer::GetWhiteTexture()
	{
		return s_Data->m_WhiteTexture;
	}
	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		if (!s_Data) return;
		if (!s_Data->m_Context) return;
		SetViewPort(0, 0, width, height);
		
		Resizebuffers(width, height);
	}
}
